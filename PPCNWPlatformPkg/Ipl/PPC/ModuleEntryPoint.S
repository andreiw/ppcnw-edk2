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
#include <Pcr.h>

.text
        .p2align 4
stack:
        .fill 4096
stack_top:

pcr:
        .fill 4096

//
// Enter here with r5 = OF CIF entry.
//
ASM_FUNC(_ModuleEntryPoint)
        //
        // Setup stack.
        //
        bl      1f
1:      mflr    r3
        addi    r1, r3, (stack_top - 1b)
        addi    r1,r1,-16
        //
        // Setup PCR and save OF CIF.
        //
        addi    r2, r3, (pcr - 1b)
        stw     r5, PCR(OFEntry)
        //
        // Go C: r5 = OF CIF.
        //
        bl      CEntryPoint
        b       .