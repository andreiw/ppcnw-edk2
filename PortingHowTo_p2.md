# Making the build system aware of the new architecture

Start by creating a directory for the architecture-specific assets. In
my case that's [`PPCPkg`](PPCPkg). We'll separate architecture and
platform-specific code.
```
pbg4:~/src/edk2/ ls PPCPkg
Applications/  PPCPkg.dec  PPCPkg.dsc
pbg4:~/src/edk2/ ls PPCPkg/Applications/
Hello/
pbg4:~/src/edk2/ ls PPCPkg/Applications/Hello/
Hello.c  Hello.inf
```

The `DSC` is used to drive the entire build. The most important items here for now are `SUPPORTED_ARCHITECTURES`.
`Applications/Hello` is a dummy UEFI app with an empty main function, i.e. without any dependencies.
```
pbg4:~/src/edk2/ build -a PPC -p PPCPkg/PPCPkg.dsc
Usage: build.exe [options] [all|fds|genc|genmake|clean|cleanall|cleanlib|modules|libraries|run]

build.exe: error: option -a: invalid choice: 'PPC' (choose from 'IA32', 'X64', 'IPF', 'EBC', 'ARM', 'AARCH64')
```

[`build`](BaseTools/Source/Python/build/build.py) controls the build
progress, parsing DSC, FDF and INF and creating Autogen and make
files. We need to teach it about PPC. This is mechanical work, and I'm
copying support for other 32-bit ISAs.
```
pbg4:~/src/edk2/ build -a PPC -p PPCPkg/PPCPkg.dsc 
Build environment: Linux-3.16.0-4-powerpc-ppc-with-debian-8.8
Build start time: 17:09:51, Jul.04 2017

WORKSPACE        = /home/andreiw/src/edk2
ECP_SOURCE       = /home/andreiw/src/edk2/EdkCompatibilityPkg
EDK_SOURCE       = /home/andreiw/src/edk2/EdkCompatibilityPkg
EFI_SOURCE       = /home/andreiw/src/edk2/EdkCompatibilityPkg
EDK_TOOLS_PATH   = /home/andreiw/src/edk2/BaseTools
CONF_PATH        = /home/andreiw/src/edk2/Conf


Architecture(s)  = PPC
Build target     = DEBUG
Toolchain        = MYTOOLS

Active Platform          = /home/andreiw/src/edk2/PPCPkg/PPCPkg.dsc

Processing meta-data ......... 


build.py...
 : error C0DE: Unknown fatal error when processing
 [/home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.inf]
 
(Please send email to edk2-devel@lists.01.org for help, attaching
following call stack trace!)

(Python 2.7.9 on linux2) Traceback (most recent call last):
  File
  "/home/andreiw/src/edk2/BaseTools/BinWrappers/PosixLike/../../Source/Python/build/build.py",
  line 2290, in Main
    MyBuild.Launch()
  File
  "/home/andreiw/src/edk2/BaseTools/BinWrappers/PosixLike/../../Source/Python/build/build.py",
  line 2042, in Launch
    self._MultiThreadBuildPlatform()
  File
  "/home/andreiw/src/edk2/BaseTools/BinWrappers/PosixLike/../../Source/Python/build/build.py",
  line 1873, in _MultiThreadBuildPlatform
    Ma.CreateMakeFile(True)
  File
  "/home/andreiw/src/edk2/BaseTools/Source/Python/AutoGen/AutoGen.py",
  line 4134, in CreateMakeFile
    if Makefile.Generate():
  File
  "/home/andreiw/src/edk2/BaseTools/Source/Python/AutoGen/GenMake.py",
  line 184, in Generate
    FileContent = self._TEMPLATE_.Replace(self._TemplateDict)
  File
  "/home/andreiw/src/edk2/BaseTools/Source/Python/AutoGen/GenMake.py",
  line 526, in _CreateTemplateDict
    RespDict = self.CommandExceedLimit()
  File
  "/home/andreiw/src/edk2/BaseTools/Source/Python/AutoGen/GenMake.py",
  line 714, in CommandExceedLimit
    SingleCommandLength +=
  len(self._AutoGenObject._BuildOption[Tool]['PATH'])
KeyError: 'CC'


- Failed -
Build end time: 17:10:12, Jul.04 2017
Build total time: 00:00:22
```

Maybe I need to define the build tools now? This is in the
[`tools_def.template`](BaseTools/Conf/tools_def.template), which gets
copied to `Conf/tools_def.txt` by `edksetup.sh`.

The target compiler is GCC 4.9, so I need to add entries that begin with `GCC49_PPC_`. This
is also fairly mechanical, although you sort of have to know what to
put in. If you're targetting 32-bit, use IA32 and ARM as examples. If
you're targetting 64-bit, use X64 and AARCH64 as examples. Probably,
if you're porting UEFI to an architecture, you're comfortable and know
how to build free-standing code. Look at boot loaders, firmware,
kernel Makefiles for reference. For 32-bit architectures,
`PECOFF_HEADER_SIZE` is 0x220. For 64-bit architectures,
`PECOFF_HEADER_SIZE` is 0x228.

Last step is teaching `build` to use the toolchain. This can be done
either by passing the `-t` parameter:
```
build -a PPC -p PPCPkg/PPCPkg.dsc -t GCC49
```

...or modify `Conf/target.txt`, which is derived from [`BaseTools/Conf/target.template`](BaseTools/Conf/target.template):
```
...
ACTIVE_PLATFORM       = PPCPkg/PPCPkg.dsc
TARGET_ARCH           = PPC
TOOL_CHAIN_TAG        = GCC49
...
```

Okay, great progress:
```
pbg4:~/src/edk2/ build
Build environment: Linux-3.16.0-4-powerpc-ppc-with-debian-8.8
Build start time: 17:43:00, Jul.04 2017

WORKSPACE        = /home/andreiw/src/edk2
ECP_SOURCE       = /home/andreiw/src/edk2/EdkCompatibilityPkg
EDK_SOURCE       = /home/andreiw/src/edk2/EdkCompatibilityPkg
EFI_SOURCE       = /home/andreiw/src/edk2/EdkCompatibilityPkg
EDK_TOOLS_PATH   = /home/andreiw/src/edk2/BaseTools
CONF_PATH        = /home/andreiw/src/edk2/Conf


Architecture(s)  = PPC
Build target     = DEBUG
Toolchain        = GCC49

Active Platform          = /home/andreiw/src/edk2/PPCPkg/PPCPkg.dsc

Processing meta-data ..... done!
Building ... /home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.inf [PPC]
"gcc"   -g -Os -fshort-wchar -fno-builtin -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -fno-common -fno-short-enums -save-temps -fverbose-asm -fsigned-char -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-builtin -Wno-address -fno-asynchronous-unwind-tables -fno-stack-protector -mno-multiple -mno-pointers-to-nested-functions -mno-strict-align -msoft-float -g -fshort-wchar -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -DSTRING_ARRAY_NAME=HelloStrings -O0 -c -o /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/./Hello.obj -I/home/andreiw/src/edk2/PPCPkg/Applications/Hello -I/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG -I/home/andreiw/src/edk2/MdePkg -I/home/andreiw/src/edk2/MdePkg/Include -I/home/andreiw/src/edk2/MdeModulePkg -I/home/andreiw/src/edk2/MdeModulePkg/Include /home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.c
In file included from /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/AutoGen.h:16:0,
                 from <command-line>:0:
/home/andreiw/src/edk2/MdePkg/Include/Base.h:28:27: fatal error: ProcessorBind.h: No such file or directory
 #include <ProcessorBind.h>
                           ^
compilation terminated.
GNUmakefile:372: recipe for target '/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/Hello.obj' failed
make: *** [/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/Hello.obj] Error 1


build.py...
 : error 7000: Failed to execute command
 make tbuild [/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello]


build.py...
 : error F002: Failed to build module
 /home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.inf [PPC, GCC49, DEBUG]

- Failed -
Build end time: 17:43:22, Jul.04 2017
Build total time: 00:00:22
```

Now we're hitting missing headers! Good. `ProcessorBind.h` we've seen
before when building tools, but this copy belongs in
[`MdePkg/Include/PPC/ProcessorBind.h`](MdePkg/Include/PPC/ProcessorBind.h). Eventually
we'll add a few more thing to it, but for now it can be an equivalent
copy.

Also, need to modify [`MdePkg/MdePkg.dec`](MdePkg/MdePkg.dec):
```
--- a/MdePkg/MdePkg.dec
+++ b/MdePkg/MdePkg.dec
@@ -45,6 +45,9 @@
 [Includes.ARM]
   Include/Arm
 
[Includes.PPC]
  Include/PPC
```

Now we're seeing `error: #error Unknown Processor Type` errors from
common headers.

We're done with part 2.
