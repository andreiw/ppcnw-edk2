# Turning our IPL into a DXE loader.

All we have so far is the world's most roundabout implementation of
a "Hello World!" standalone application for NewWorld Macs. How to get
to a UEFI implementation from here? We need to hand off to the UEFI
kernel, also known as the DXE core. Usually, Tiano implementations
have an additional phase between the early boot code (SEC) and DXE,
namely PEI, which presents a modular way of performing the same
activities we need to do to hand off to DXE, namely:
* Locate and initialize RAM.
* Build the PEI Hand-off Information Table for DXE, which describes
  system RAM availability and usage, firmware volume location, etc.
* Find and hand-off to DXE kernel.

Because we are launching from OpenFirmware, we won't be performing
any real hardware initialization. All we need to do is scrape
the information we need using OF interfaces. Fortunately for us,
we can avoid PEI entirely, as libraries for building the PHIT
and other hand-off state have been made available in Tiano ever
since the addition of the 32-bit ARM port.

First of all, we need some spare RAM. This will be used for any
allocations we do, for our stack, the PCR and finally the
hand-off structures. I've decided the simplest way to do this
was to extend [the ELF FD wrapper](PPCPkg/FDLoader/PPC/ModuleEntryPoint.S)
to reserve a bit of extra memory as part of the only loaded segment.
This is done by extending the in-memory size, i.e. we don't grow
our already large FD file a single bit more. The size of
the extra space is specified by a fixed PCD, and the space
directly follows the FD. This makes it easy to rewrite
the [assembly portion of our IPL](PPCNWPlatformPkg/Ipl/PPC/ModuleEntryPoint.S)
to place the stack and PCR in this region. Finally, we
change the signature of the C entry point to pass the
full ranges of memory we occupy, use for stack and that
we can use for building hand-off structures. E.g.:
```
VOID
CEntryPoint (
             IN UINTN IplReservedBottom,
             IN UINTN StackBase,
             IN UINTN IplFreeMemoryBottom,
             IN UINTN IplMemoryTop
             )
{
   ...
}
```

The first real task the C portion of the IPL will do is
discover the base and size of RAM. As I mentioned before,
we just need to query OpenFirmware here. I've decided
to store these values in PCDs that can be patched
at runtime. Of course these need to be defined in the DEC
file and specified in the INF file for each module consuming them,
but the trick to not ending up with a linker
error is to not forget to add them to your DSC definition
under the appropriate patchable PCD category:
```
[PcdsPatchableInModule.common]
  gPPCTokenSpaceGuid.PcdSystemMemoryBase|0x0
  gPPCTokenSpaceGuid.PcdSystemMemorySize|0x0
```

I suppose that was overkill.

Now we can construct the HOB list. The trick here
is to claim the entire range of addresses that
may be interesting to us, from `0` all the way
to `PcdSystemMemoryBase + PcdSystemMemorySize`.
The actual range used for HOB list will be
the remaining available part of the spare RAM
carved up in our assembly stub. Then we
can build memory allocation hobs to describe
the in-use ranges, which today includes our FD
and the spare RAM:
```
  /*
   * Claim PHIT memory entire system RAM range:
   * this lets us use BuildMemoryAllocationHob
   * to cover reserved allocations.
   */
  HobList = HobConstructor((VOID *) 0,
                           PcdGet32(PcdSystemMemoryBase) +
                           PcdGet32(PcdSystemMemorySize),
                           (VOID *) IplFreeMemoryBottom,
                           (VOID *) IplMemoryTop);
  PrePeiSetHobList(HobList);

  /*
   * [IplReservedBottom, IplFreeMemoryBottom) is
   * RAM reserved for FD, stack and PCR.
   */
  BuildMemoryAllocationHob(IplReservedBottom,
                           IplFreeMemoryBottom -
                           IplReservedBottom,
                           EfiBootServicesData);
```

Eventually, we'll claim all the memory ranges that are used by OF itself.
Doing so now is a bit tedious because the data is returned from OF
as a variable-length array.

We do need to introduce our [own implementation of the `PrePiHobListPointerLib`](PPCNWPlatformPkg/Library/PrePiHobListPointerLib), as all kinds of interesting libraries out of the `EmbeddedPkg` want access to the hand-off structures. Keeping
it as a global is fine.

Now we build HOBs for the DXE stack, CPU, our firmware volume and boot mode. The later is always `BOOT_WITH_FULL_CONFIGURATION`, as we have no resume or recovery modes - we're just a bootloader on top of OpenFirmware, effectively.

This should leave us with an IPL that still just logs a few
interesting addresses, but at least internally does a lot more.

We're going to need a bit more elbow grease to hand-off to DXE.