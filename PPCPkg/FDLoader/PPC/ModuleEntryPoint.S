//
// Copyright (c) 2017, Andrei Warkentin <andrey.warkentin@gmail.com>
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//

#include <AsmMacroLib.h>
#include <Library/PcdLib.h>

#define FV_OFFSET (FixedPcdGet32(PcdFvBaseAddress) - \
                   FixedPcdGet32(PcdFdBaseAddress))

ASM_FUNC(_ModuleEntryPoint)
        /*
         * With credit to the Teensy ELF tutorial.
         *
         * Who needs linkers anyway?
         */
start:  .byte 0x7F, 'E', 'L', 'F', 1, 2, 1, 0  /* e_ident */
        .byte 0, 0, 0, 0, 0, 0, 0, 0
        .word 2                                /* e_type */
        .word 20                               /* e_machine */
        .long 1                                /* e_version */
        .long FixedPcdGet32(PcdFdBaseAddress) + code - start /* e_entry */
        .long phdr - start                     /* e_phoff */
        .long 0                                /* e_shoff */
        .long 0                                /* e_flags */
        .word phdr - start                     /* e_ehsize */
        .word code - phdr                      /* e_phentsize */
        .word 1                                /* e_phnum */
        .word 0                                /* e_shentsize */
        .word 0                                /* e_shnum */
        .word 0                                /* e_shstrndx */
phdr:   .long 1                                /* e_ptype */
        .long 0                                /* p_offset */
        .long FixedPcdGet32(PcdFdBaseAddress)  /* p_vaddr */
        .long FixedPcdGet32(PcdFdBaseAddress)  /* p_paddr */
        .long FixedPcdGet32(PcdFdSize)         /* p_filesz */
        .long FixedPcdGet32(PcdFdSize)         /* p_memsz */
        .long 7                                /* p_flags */
        .long 0x1000                           /* p_align */
code:   
        /*
         * This fussy nonsense jumps through an entry point
         * stored at 0b + 0x4000.
         */
1:      bl   2f
2:      mflr r13
        addi r13, r13, (FV_OFFSET) - (2b - start)
        lwz  r13, 0(r13)
        mtlr r13
        blr
