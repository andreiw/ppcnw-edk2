# Converting ELF to EFI

Generally speaking, this is the part where:
- we figure out our compiler/linker flags are no bloody good.
- we modify GenFw to convert the ELF binaries to PE32+.

But because our target is a big-endian, things are about to get really weird.
See, the fine folks are Intel never considered that anyone would ever
want to run Tiano on a big-endian machine, nor even that someone
would cross-compile on a big-endian machine. UEFI is always LE,
so this means, for example, that a BE AArch64 kernel cannot easily consume
UEFI RT services...

Fun. It doesn't help that PE32+ is defined to be little-endian, as well.

What are our choices?
- Go through every tool (`GenFw`, but also FV generation and etc)
  and make it endian-safe (`htole16` and friends). This means
  making any of matching Tiano components (e.g. PE32 loader
  and so on) endian-safe as well. This would allow to move
  development to a little-endian host, eventually. This is
  the morally right approach that would make hiring engineering
  managers comfy ;-).
- Ignore endianness. This means that things will only work
  if the build host and target host have matching endianness.
  This also means that the generated files will be wildly
  incompatible with their LE counterparts. On the plus side,
  we won't have to fix up code that assumes LE (as long
  as it doesn't rely on interpreting words as strings),
  and that means a smaller delta with upstream. After all,
  it's highly doubtful anyone upstream would care about
  BE.

[`BasePeCoff.c`](BaseTools/Source/C/Common/BasePeCoff.c) needs to be aware of the target architecture in `PeCoffLoaderCheckImageType`.

[`PeImage.h`](BaseTools/Source/C/Include/IndustryStandard/PeImage.h) needs the PE32 architecture identifier defines.

The meat of the work is in
[`Elf32Convert.c`](BaseTools/Source/C/GenFw/Elf32Convert.c), because
the target architecture is 32-bit. There's a [matching 64-bit
file](BaseTools/Source/C/GenFw/Elf32Convert.c). Aside from validating
the input ELF file, this creates PE32+ base relocations based on
relocation information in the ELF file. Usually, the ELF files are
built as position-independent executables or shared objects. In my
case, after beating my head and still getting ELF files generated
with relocation types that don't map well (e.g. R_PPC_ADDR16_HA) to COFF base relocs,
I think that compiling with `-fpic` is the magic bullet. The
generated code is verbose, but needs no relocations to process.
If this doesn't work out, I'll try with `-mrelocatable`, which
creates a `.fixup` section that would be pretty easy to process
inside `GenFw`. In the mean time, add enough logic to detect the
presence of `.rela` relocations, which should cause errors since
we don't expect any to exist.

So now running `build` should wind us up with a strange `Build/PPC/DEBUG_GCC49/PPC/PPCPkg/Applications/Hello/Hello/DEBUG/Hello.efi` file, which is sort-of like PE32, but with everything in the wrong endianness.