/** @file
  Processor or Compiler specific defines and types for PowerPC.

  Copyright (c) 2017, Andrei Warkentin <andrey.warkentin@gmail.com>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PROCESSOR_BIND_H__
#define __PROCESSOR_BIND_H__

//
// Only support GCC.
//
#ifndef __GNUC__
#error Unsupported compiler
#endif

//
// Define the processor type so other code can make processor based choices
//
#define MDE_CPU_PPC

//
// Use ANSI C 2000 stdint.h integer width declarations
//
#include "stdint.h"
typedef uint8_t   BOOLEAN;
typedef int8_t    INT8;
typedef uint8_t   UINT8;
typedef int16_t   INT16;
typedef uint16_t  UINT16;
typedef int32_t   INT32;
typedef uint32_t  UINT32;
typedef int64_t   INT64;
typedef uint64_t  UINT64;
typedef char      CHAR8;
typedef uint16_t  CHAR16;

typedef UINT32  UINTN;
typedef INT32   INTN;


//
// Processor specific defines
//
#define MAX_BIT     0x80000000
#define MAX_2_BITS  0xC0000000

//
// Maximum legal PPC address
//
#define MAX_ADDRESS   0xFFFFFFFF

//
// Stack alignment.
//
#define CPU_STACK_ALIGNMENT 16

//
// Modifier to ensure that all protocol member functions and EFI intrinsics
// use the correct C calling convention. All protocol member functions and
// EFI intrinsics are required to modify their member functions with EFIAPI.
//

#define EFIAPI

//
// The Microsoft* C compiler can removed references to unreferenced data items
//  if the /OPT:REF linker option is used. We defined a macro as this is a 
//  a non standard extension
//
#define GLOBAL_REMOVE_IF_UNREFERENCED

#define ASM_GLOBAL .global

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__
#endif

#define ASM_LABEL(a) _CONCATENATE(__USER_LABEL_PREFIX__, a)
#define ASM_FUNC(name)                          \
  .section .text;                               \
  .align 2;                                     \
  .type name,@function;                         \
  ASM_GLOBAL ASM_LABEL(name);                   \
ASM_LABEL(name):

/**
  Return the pointer to the first instruction of a function given a function pointer.
  On PPC these two pointer values are the same, so the implementation of this macro
  is very simple.

  @param  FunctionPointer   A pointer to a function.

  @return The pointer to the first instruction of a function given a function pointer.

**/
#define FUNCTION_ENTRY_POINT(FunctionPointer) (VOID *)(UINTN)(FunctionPointer)

#endif /* __PROCESSOR_BIND_H__ */
