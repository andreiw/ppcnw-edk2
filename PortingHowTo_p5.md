# Platform package and the SEC

Here is where we realize that any code we add for architecture support
is generic from any platforms targetted. The NewWorld platform-specific
code will live in PPCNWPlatformPkg.


I'll modify `Conf/target.txt` to reflect the new build target:
```
ACTIVE_PLATFORM = PPCNWPlatformPkg/PPCNWPlatformPkg.dsc
```

This is also where things quickly spiral out of control, as we
struggle to produce something that is actually valid and can
boot. Hypothetically, this step could have been split into
several steps. In practice though, because this really is
supposed to be a genuine examination of the kinds of steps
(and missteps) one could take porting UEFI - I am not going to do so.
Tough.

The first real binary we'll work on is our SEC phase. Actually,
I prefer to call it the IPL, since we'll bypass the PEI phase
and go to DXE directly.

Don't forget the `MODULE_TYPE = SEC` in [`Ipl.inf`](PPCNWPlatformPkg/Ipl/Ipl.inf), which
is also enhanced with the architecture-specific source section, e.g.:
```
[Sources.PPC]
  PPC/ModuleEntryPoint.S
```

[`ModuleEntryPoint.S`](PPCNWPlatformPkg/Ipl/ModuleEntryPoint.S)
of course needs to set up a stack, and since we're building position-independent code, the `r1` load
needs to be PC-relative. Then it branches to the C portion.


The [C portion of the Ipl](PPCNWPlatformPkg/Ipl/Ipl.c) for now will just do an OF call to print
a hello world message.

If we just add the `Ipl` component to our platform `DSC`, similar to the dummy `Hello` application we've
seen before, we should be left with an `Ipl.efi`. This is not something we can boot in OpenFirmware,
because it is in a format unknown to OpenFirmware. Moreover, eventually we'll have more UEFI
components, so we might as well focus on packaging it all up in an `FD`, which is a container
of `FVs` or firmware volumes. An `FD` and its associated `FVs` are specified in a flash
definition file, which must be referenced by the `DSC`. E.g.:
```
   OUTPUT_DIRECTORY               = Build/PPCNWPlatform
   SUPPORTED_ARCHITECTURES        = PPC
   BUILD_TARGETS                  = DEBUG|RELEASE
+  FLASH_DEFINITION               = PPCNWPlatformPkg/PPCNWPlatformPkg.fdf
   SKUID_IDENTIFIER               = DEFAULT
```

Of course just creating this "firmware archive" doesn't quite do it for us either. How do we give
control to the IPL? This is very platform and architecture specific. On some platforms, the
flash ROM would be at a well-known address, such that the CPU reset vector would fall into the
range occupied by a bit of generated code in `FD` that would branch to the `SEC`. The solution
I've taken here (inspired by the AArch64 Xen UEFI support) is an ELF wrapper around the FD.
The [ELF wrapper](PPCPkg/FDLoader) immediately preceeds the firmware volume. The `FD` loader
knows to look for the address of the `SEC` at the very beginning of the embedded FV. Of course,
we need to modify the `GenFv` tool to place it there for us. This is pretty straightforward
code inside [`GenFvInternalLib.c`](BaseTools/Source/C/GenFv/GenFvInternalLib.c).

Are we done yet? Nope. If we added some logging, we would see that that the reset vector
is never created. Why? Our IPL is never found. Why? The `FV` is corrupted. Why?
Because of endian unsafeness of the worst kind - interpreting byte arrays as integers
by casting. Apparently, the length for internal structures is stored as 3 bytes. This
is easy to patch around, as long as you're willing to deal files built on BE and LE machines
being completely incompatible. This is pretty horrible considering Tiano was developed
in 2000s, not the 60s. In practice, it means you can't use IBM z to build Tiano
really fast for LE machines like X64 or AArch64.
```
--- a/BaseTools/Source/C/Common/FvLib.c
+++ b/BaseTools/Source/C/Common/FvLib.c
@@ -832,8 +832,9 @@ Returns:
     return 0;
   }
 
-  Length  = *((UINT32 *) ThreeByteLength);
-  Length  = Length & 0x00FFFFFF;
+  Length = ThreeByteLength[2] << 16 |
+    ThreeByteLength[1] << 8 |
+    ThreeByteLength[0];
```

So now we get error messages about invalid PE files - we're on the right track. The issue
is the `PeImageClass` inside [`BaseTools/Source/Python/Common/Misc.py`](BaseTools/Source/Python/Common/Misc.py
). We have to teach it recognise our weird BE "PE32" file, where everything is backwards - 'ZM' instead the 'MZ'
signature, and o on.

So now we build it and try booting it from, say, a USB flash drive. For PowerPC NewWorld Macs, this means:
- An Apple Partition Map on the device.
- An HFS partition (say of type Apple_Bootstrap).
- A blessed directory (say, the root).
- A file with HFS type 'tbxi'.

Once we create the partition map, we can:
```
root@pbg4:/home/andreiw/src/edk2# hformat /dev/your_device
root@pbg4:/home/andreiw/src/edk2# hattrib -b :
root@pbg4:/home/andreiw/src/edk2# hcopy /home/andreiw/src/edk2/Build/PPCNWPlatform/DEBUG_GCC49/FV/PPCNW.fd :uefi.elf 
root@pbg4:/home/andreiw/src/edk2# hattrib -t tbxi -c TBXI :uefi.elf
root@pbg4:/home/andreiw/src/edk2# humount
```

The OpenFirmware boot command on my machine is `boot usb1/disk:,uefi.elf`. Did it work? Nope. What are we missing?

Relocations! But I thought we were building code in a position-independent way? Well, we are, but data access
is done through the GOT. The GOT is accessed in a PC-relative fashion, but the GOT contains absolute addresses
for data. So we first split the GOT out into a separate section:
```
--- a/BaseTools/Scripts/GccBase.lds
+++ b/BaseTools/Scripts/GccBase.lds
@@ -29,7 +29,6 @@ SECTIONS {
   .text : ALIGN(CONSTANT(COMMONPAGESIZE)) {
     *(.text .text.* .stub .gnu.linkonce.t.*)
     *(.rodata .rodata.* .gnu.linkonce.r.*)
-    *(.got .got.*)
 
     /*
      * The contents of AutoGen.c files are mostly constant from the POV of the
@@ -42,6 +41,10 @@ SECTIONS {
     *:AutoGen.obj(.data.g*Guid)
   }
 
+  .got ALIGN(ALIGNOF(.text)) : ALIGN(CONSTANT(COMMONPAGESIZE)) {
+    *(.got .got.*)
+  }
+
```

Second, we fix the `GenFw` tool to go through the GOT section and fixup the GOT (handling the XIP case like our
`SEC`-phae Ipl) and create PE relocations for each of the entries to accomodate later, runtime-relocated UEFI
components. This is mostly inside [the 32-bit ELF converter code](BaseTools/Source/C/GenFw/Elf32Convert.c).

And it works! Our `uefi.elf` prints the hello message and halts back inside OF.
