# Bringing in real logging to the IPL

We start out by making [OF support a real separate library](PPCNWPlatformPkg/Library/OFWLib).
This library will be linked into every part of our firmware,
and will allow us to make calls from UEFI into OF. Of course,
this means that it has to somehow know where the CIF entry
is, regardless of whether it is being used in the IPL or
a `DXE` driver. In practice we'll need to cache more
values, so we'll introduce the idea of a Processor
Control Region structure, which will be always available
via an ABI-reserved register (for my ABI, that's `r2`).
I placed the [PCR header](PPCNWPlatformPkg/Include/Pcr.h) in
the platform-specific package, since it would be unnecessary
or very different in a different PowerPC (32-bit) port. The
OpenFirmware (CIF) entry is saved in the PCR before control is
transferred to the C portion of the IPL.

The first "real" library we need to provide is an 
implementation of `SerialPortLib`. This is consumed
by the debugging library, and eventually the serial port
`DXE` driver. We don't have a serial port on NewWorld
machines, and instead we'll treat the OpenFirmware
standard input/output as the serial console, i.e.
it's just the simple way of getting textual
information out. Our [`OFWSerialPortLib`](PPCNWPlatform/Library/OFWLib)
will, of course, depend on our `OFWLib`.

With all of this we should be able to now do something
like:
```
  OFWIplInitialize();
  SerialPortInitialize();
  SerialPortWrite("Hello!\r\n", 8);
```

But I want more. What does it take to get propper
logging via a `printf`-like logging function,
or even the regular `DEBUG` macros?

Turns out quite a bit! We need `BaseLib` and `PrintLib`,
and this is where things quickly get harder than they
first appeared. `BaseLib` needs some [architecture-specific](MdePkg/Library/BaseLib/PPC)
implementations for things like switching stacks, `setjmp`-like
behavior, breakpoints and the like, but also 64-bit math.
Because I didn't want to figure these out myself, I ended
up using the generic [`Math64.c`](MdePkg/Library/BaseLib/Math64.c).
This caused two missing compiler intrinsics during linking,
`__udivdi3` and `__umoddi3`, which I ended up pulling in
via the FreeBSD [`qdivrem.c`](MdePkg/Library/BaseLib/PPC/qdivrem.c)
implementation. Don't forget to edit [`BaseLib.inf`](MdePkg/Library/BaseLib/BaseLib.inf)!
`BaseLib` and `PrintLib` also have a few other dependencies, so in total the set of libraries
pulled into our `DSC` looks like:
```
[LibraryClasses.common]
  OFWLib|PPCNWPlatformPkg/Library/OFWLib/OFWLib.inf
  SerialPortLib|PPCNWPlatformPkg/Library/OFWSerialPortLib/OFWSerialPortLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
```

`BasePrintLib` has another nasty issue. It's not endian safe. The routines
are backed by [a shared implementation](MdePkg/Library/BasePrintLib/PrintLibInternal.c)
that can accept input and produce output in both 8-bit ASCII and 16-bit Unicode, and it
uses a few tricks to parse and generate output. None of these tricks are good engineering
practices, though. FWIW, some careful refactoring could really sort
things out. I ended up pulling out the code to fetch the next
character out of a buffer into a `NextCharacter` routine, that
I later made work for BE as well (not that I think I did a great
job or anything, sigh).

`BasePrintLibFillBuffer` is yet another
place that must be made BE-safe, as well as any place
that positions the `ArgumentString` pointer over a portion
of a native integer-sized variable. Caveat emptor, though! I hope
I don't have to come back to this code, but I've a feeling
this isn't the last time I'll be hacking around these
incredibly difficult to debug issues. There's just no
good reason to write code like this in the 21st century.

Now the `DEBUG` macros work. Don't forget to set to something
reasonable the `DebugLib`-specific PCDs like error level, else you
might not see anything. E.g.:
```
  #  DEBUG_INIT      0x00000001  // Initialization
  #  DEBUG_WARN      0x00000002  // Warnings
  #  DEBUG_LOAD      0x00000004  // Load events
  #  DEBUG_FS        0x00000008  // EFI File system
  #  DEBUG_POOL      0x00000010  // Alloc & Free's
  #  DEBUG_PAGE      0x00000020  // Alloc & Free's
  #  DEBUG_INFO      0x00000040  // Informational debug messages
  #  DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  #  DEBUG_VARIABLE  0x00000100  // Variable
  #  DEBUG_BM        0x00000400  // Boot Manager
  #  DEBUG_BLKIO     0x00001000  // BlkIo Driver
  #  DEBUG_NET       0x00004000  // SNI Driver
  #  DEBUG_UNDI      0x00010000  // UNDI Driver
  #  DEBUG_LOADFILE  0x00020000  // UNDI Driver
  #  DEBUG_EVENT     0x00080000  // Event messages
  #  DEBUG_GCD       0x00100000  // Global Coherency Database changes
  #  DEBUG_CACHE     0x00200000  // Memory range cachability changes
  #  DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may
  #                              // significantly impact boot performance
  #  DEBUG_ERROR     0x80000000  // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
```

This leaves us with an IPL that prints a banner and a
few addresses for the FD/FV. Where to next?
