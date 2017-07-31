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

.text

#/**
#  Saves the current CPU context that can be restored with a call to LongJump() and returns 0.#
#
#  Saves the current CPU context in the buffer specified by JumpBuffer and returns 0.  The initial
#  call to SetJump() must always return 0.  Subsequent calls to LongJump() cause a non-zero
#  value to be returned by SetJump().
#
#  If JumpBuffer is NULL, then ASSERT().
#  For IPF CPUs, if JumpBuffer is not aligned on a 16-byte boundary, then ASSERT().
#
#  @param  JumpBuffer    A pointer to CPU context buffer.
#
#**/
#
#UINTN
#EFIAPI
#SetJump (
#  IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer  // r3
#  );
#
ASM_FUNC(SetJump)
        stw     %r1,  0(%r3)
        stwu    %r2,  4(%r3)
        stwu    %r13, 4(%r3)
        stwu    %r14, 4(%r3)
        stwu    %r15, 4(%r3)
        stwu    %r16, 4(%r3)
        stwu    %r17, 4(%r3)
        stwu    %r18, 4(%r3)
        stwu    %r19, 4(%r3)
        stwu    %r20, 4(%r3)
        stwu    %r21, 4(%r3)
        stwu    %r22, 4(%r3)
        stwu    %r23, 4(%r3)
        stwu    %r24, 4(%r3)
        stwu    %r25, 4(%r3)
        stwu    %r26, 4(%r3)
        stwu    %r27, 4(%r3)
        stwu    %r28, 4(%r3)
        stwu    %r29, 4(%r3)
        stwu    %r30, 4(%r3)
        stwu    %r31, 4(%r3)
        mflr    %r0
        stwu    %r0,  4(%r3)
        mfcr    %r0
        stwu    %r0,  4(%r3)
        li      %r3,  0
        blr

#/**
#  Restores the CPU context that was saved with SetJump().#
#
#  Restores the CPU context from the buffer specified by JumpBuffer.
#  This function never returns to the caller.
#  Instead is resumes execution based on the state of JumpBuffer.
#
#  @param  JumpBuffer    A pointer to CPU context buffer.
#  @param  Value         The value to return when the SetJump() context is restored.
#
#**/
#VOID
#EFIAPI
#InternalLongJump (
#  IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer,  // r3
#  IN      UINTN                     Value         // r4
#  );
#
ASM_FUNC(InternalLongJump)
        lwz      %r1,  0(%r3)
        lwzu     %r2,  4(%r3)
        lwzu     %r13, 4(%r3)
        lwzu     %r14, 4(%r3)
        lwzu     %r15, 4(%r3)
        lwzu     %r16, 4(%r3)
        lwzu     %r17, 4(%r3)
        lwzu     %r18, 4(%r3)
        lwzu     %r19, 4(%r3)
        lwzu     %r20, 4(%r3)
        lwzu     %r21, 4(%r3)
        lwzu     %r22, 4(%r3)
        lwzu     %r23, 4(%r3)
        lwzu     %r24, 4(%r3)
        lwzu     %r25, 4(%r3)
        lwzu     %r26, 4(%r3)
        lwzu     %r27, 4(%r3)
        lwzu     %r28, 4(%r3)
        lwzu     %r29, 4(%r3)
        lwzu     %r30, 4(%r3)
        lwzu     %r31, 4(%r3)
        lwzu     %r0,  4(%r3)
        mtlr    %r0
        lwzu     %r0,  4(%r3)
        mtcr    %r0
        mr      %r3,  %r4
        blr
