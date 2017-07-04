# Preparing the environment and build tools

So the target system is a PowerPC-based NewWorld. To spice
things up, we'll also (try) make it the development host.

First, I updated to the latest (and last) Debian 8 (Jessie).
You will need the obvious set of tools: git, make, gcc, g++.

Because for me target and host are the same, I won't need
a cross-compiler.

Now let's clone the tree.
```
$ git clone https://github.com/tianocore/edk2
```

Setup the UEFI environment.
```
$ cd edk2
$ . edksetup.sh
```

We'll pick `PPC` as the architecture name. Now we need to get the
`BaseTools` module building.
```
pbg4:~/src/edk2/BaseTools/ make
make -C Source/C
make[1]: Entering directory '/home/andreiw/src/edk2/BaseTools/Source/C'
Attempting to detect ARCH from 'uname -m': ppc
Could not detected ARCH from uname results
GNUmakefile:36: *** ARCH is not defined!. Stop.
make[1]: Leaving directory '/home/andreiw/src/edk2/BaseTools/Source/C'
GNUmakefile:25: recipe for target 'Source/C' failed
make: *** [Source/C] Error 2
```

Ok. Let's fix that. We'll first need a
[`BaseTools/Source/C/Include/PPC/ProcessorBind.h`](BaseTools/Source/C/Include/PPC/ProcessorBind.h) file.

`ProcessorBind.h` I've derived from another 32-bit CPU, like IA32 or
ARM. This contains type definitions, mostly. It's boilerplate. In case
there are multiple coding conventions for your architectures and it's
not obvious which one you should be using, you might wish to specify
what the `EFIAPI` attribute will be. Like, on x86 Windows-style cdecl is
used, regardless of how you build the rest of Tiano. On most
architectures an empty define is fine.

Now appropriately hook it into [`BaseTools/Source/C/Makefiles/header.makefile`](Source/C/Makefiles/header.makefile):
```
--- a/BaseTools/Source/C/Makefiles/header.makefile
+++ b/BaseTools/Source/C/Makefiles/header.makefile
@@ -43,6 +43,10 @@ ifeq ($(ARCH), AARCH64)
 ARCH_INCLUDE = -I $(MAKEROOT)/Include/AArch64/
 endif
+ifeq ($(ARCH), PPC)
+ARCH_INCLUDE = -I $(MAKEROOT)/Include/PPC/
+endif
```

Fix the `ARCH` detection in [`BaseTools/Source/C/GNUmakefile`](BaseTools/Source/C/GNUmakefile).
```
--- a/BaseTools/Source/C/GNUmakefile
+++ b/BaseTools/Source/C/GNUmakefile
@@ -31,6 +31,9 @@ ifndef ARCH
   ifneq (,$(findstring arm,$(uname_m)))
     ARCH=ARM
   endif
  ifneq (,$(findstring ppc,$(uname_m)))
    ARCH=PPC
  endif
```

Ok, ensure you have `libuuid` headers (Debian `uuid-dev`).

And... we're are done. This gives you the tools need to help build UEFI.
