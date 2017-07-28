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

#ifndef __ASM_MACRO_LIB_H__
#define __ASM_MACRO_LIB_H__

#define r0  %r0
#define r1  %r1
#define r2  %r2
#define r3  %r3
#define r4  %r4
#define r5  %r5
#define r6  %r6
#define r7  %r7
#define r8  %r8
#define r9  %r9
#define r10 %r10
#define r11 %r11
#define r12 %r12
#define r13 %r13
#define r14 %r14
#define r15 %r15
#define r16 %r16
#define r17 %r17
#define r18 %r18
#define r19 %r19
#define r20 %r20
#define r21 %r21
#define r22 %r22
#define r23 %r23
#define r24 %r24
#define r25 %r25
#define r26 %r26
#define r27 %r27
#define r28 %r28
#define r29 %r29
#define r30 %r30
#define r31 %r31

#define LOAD_IMM(r, e) \
  lis r,(e)@h;         \
  ori r,r,(e)@l;

#endif // __ASM_MACRO_LIB_H__

