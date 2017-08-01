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

#include <Library/PcdLib.h>
#include <AsmMacroLib.h>
#include <Pcr.h>

.text
        .p2align 4

#define STACK_FRAME_MIN 16

local_vars:
/* + 0: IplReservedBottom */
        .long FixedPcdGet32(PcdFdBaseAddress)
/* + 4: stack base */
        .long FixedPcdGet32(PcdFdBaseAddress) + \
              FixedPcdGet32(PcdFdSize)
/* + 8: PCR base */
        .long FixedPcdGet32(PcdFdBaseAddress) + \
              FixedPcdGet32(PcdFdSize) + \
              FixedPcdGet32(PcdCPUCorePrimaryStackSize)
/* + 12: IplFreeMemoryBotom */
        .long FixedPcdGet32(PcdFdBaseAddress) + \
              FixedPcdGet32(PcdFdSize) + \
              FixedPcdGet32(PcdCPUCorePrimaryStackSize) + \
              PCR_Size
/* +16: IplMemoryTop */
        .long FixedPcdGet32(PcdFdBaseAddress) + \
              FixedPcdGet32(PcdFdSize) + \
              FixedPcdGet32(PcdFdLoaderExtraSize)
//
// Enter here with r5 = OF CIF entry.
//
ASM_FUNC(_ModuleEntryPoint)
        mr      r31, r5
        bl      1f
1:      mflr    r30
        addi    r29, r30, (local_vars - 1b)
        lwz     r3, 0(r29) /* IplReservedBottom */
        lwz     r4, 4(r29) /* stack base */
        lwz     r2, 8(r29) /* PCR base */
        subi    r1, r2, STACK_FRAME_MIN /* Stack top */
        lwz     r5, 12(r29) /* IplFreeMemoryBottom */
        lwz     r6, 16(r29) /* IplMemoryTop */
        //
        // Save OF CIF in PCR.
        //
        stw     r31, PCR(OFEntry)
        //
        // Go C: r3 = IplReservedBottom
        //       r4 = stack base
        //       r5 = IplFreeMemoryBottom
        //       r6 = IplMemoryTop
        //
        bl      CEntryPoint
        b       .