# Adding PPC support to common headers

[`UefiBaseType.h`](MdePkg/Include/Uefi/UefiBaseType.h) need
to be aware of the PE32+ machine type to use for our
architecture. Unless you're lucky like me, you'll have
to assign your own. 

[`UefiSpec.h`](MdePkg/Include/Uefi/UefiSpec.h) defines
the bootloader name for removable media
(`EFI_REMOVABLE_MEDIA_FILE_NAME`), which is architecture-specific. I
chose `BOOTPPC.EFI`.

[`BaseLib.h`](MdePkg/Include/Library/BaseLib.h) defines a setjmp/longjmp-like
context for non-local goto. This is used for image dispatch, for
example. BASE_LIBRARY_JUMP_BUFFER you're going to have to
either crib from somewhere else, or use your ABI documentation.

And now...
```
pbg4:~/src/edk2/ build
Build environment: Linux-3.16.0-4-powerpc-ppc-with-debian-8.8
Build start time: 21:30:10, Jul.08 2017

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

Processing meta-data .......... done!
Building ... /home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.inf [PPC]
 -Wno-address -fno-asynchronous-unwind-tables -fno-stack-protector -mno-multiple -mno-pointers-to-nested-functions -mno-strict-align -msoft-float -g -fshort-wchar -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -DSTRING_ARRAY_NAME=HelloStrings -O0 -c -o /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/./Hello.obj -I/home/andreiw/src/edk2/PPCPkg/Applications/Hello -I/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG -I/home/andreiw/src/edk2/MdePkg -I/home/andreiw/src/edk2/MdePkg/Include -I/home/andreiw/src/edk2/MdePkg/Include/PPC -I/home/andreiw/src/edk2/MdeModulePkg -I/home/andreiw/src/edk2/MdeModulePkg/Include /home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.c
"gcc"   -g -Os -fshort-wchar -fno-builtin -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -fno-common -fno-short-enums -save-temps -fverbose-asm -fsigned-char -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-builtin -Wno-address -fno-asynchronous-unwind-tables -fno-stack-protector -mno-multiple -mno-pointers-to-nested-functions -mno-strict-align -msoft-float -g -fshort-wchar -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -DSTRING_ARRAY_NAME=HelloStrings -O0 -c -o /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/./AutoGen.obj -I/home/andreiw/src/edk2/PPCPkg/Applications/Hello -I/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG -I/home/andreiw/src/edk2/MdePkg -I/home/andreiw/src/edk2/MdePkg/Include -I/home/andreiw/src/edk2/MdePkg/Include/PPC -I/home/andreiw/src/edk2/MdeModulePkg -I/home/andreiw/src/edk2/MdeModulePkg/Include /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/AutoGen.c
"gcc-ar" cr /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/Hello.lib  @/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/object_files.lst
"gcc" -o /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll -pie -nostdlib -Wl,--gc-sections -u _ModuleEntryPoint -Wl,-e,_ModuleEntryPoint -Wl,-Map,/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.map -z common-page-size=0x20 -Wl,--start-group,@/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/OUTPUT/static_library_files.lst,--end-group   -g -Os -fshort-wchar -fno-builtin -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -fno-common -fno-short-enums -save-temps -fverbose-asm -fsigned-char -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-builtin -Wno-address -fno-asynchronous-unwind-tables -fno-stack-protector -mno-multiple -mno-pointers-to-nested-functions -mno-strict-align -msoft-float -g -fshort-wchar -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -include AutoGen.h -DSTRING_ARRAY_NAME=HelloStrings -O0 -Wl,--script=/home/andreiw/src/edk2/BaseTools/Scripts/GccBase.lds -Wl,--defsym=PECOFF_HEADER_SIZE=0x220
"objcopy"  /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll
cp -f /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.debug
objcopy --strip-unneeded -R .eh_frame /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll
objcopy --add-gnu-debuglink=/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.debug /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll
cp -f /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.debug /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/Hello.debug
"GenFw" -e UEFI_APPLICATION -o /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.efi /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll
GenFw: ERROR 3000: Unsupported
  ELF EI_DATA not ELFDATA2LSB
GenFw: ERROR 3000: Invalid
  Unable to convert /home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.dll from ELF to PE/COFF.
make: *** [/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.efi] Error 2
GNUmakefile:321: recipe for target '/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.efi' failed


build.py...
 : error 7000: Failed to execute command
 make tbuild [/home/andreiw/src/edk2/Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello]


build.py...
 : error F002: Failed to build module
 /home/andreiw/src/edk2/PPCPkg/Applications/Hello/Hello.inf [PPC, GCC49, DEBUG]

- Failed -
Build end time: 21:30:38, Jul.08 2017
Build total time: 00:00:28
```

...we're ready to move to the next part, where we teach the `BaseTools` about PowerPC PE32+ file.
