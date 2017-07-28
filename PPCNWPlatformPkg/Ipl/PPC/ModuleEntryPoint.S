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

.text
        .p2align 4
stack:
        .fill 4096
stack_top:

ASM_FUNC(_ModuleEntryPoint)
        bl 1f
1:      mflr r1
        addi r1, r1, (stack_top - 1b)
        addi r1,r1,-16
        bl CEntryPoint
        b .