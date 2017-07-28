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

#ifndef __OFW_LIB_H__
#define __OFW_LIB_H__

#define OFW_CALL_MAX_ARGS_RETS 10

typedef struct {
  CHAR8 *Service;
  UINTN NumArgs;
  UINTN NumRet;
  VOID *Args[OFW_CALL_MAX_ARGS_RETS];
} OFW_CALL_ARGS;

static inline VOID *
OFW_Call(void (*OFWEntry)(void *),
         char *Service,
         int NumArgs,
         int NumRet,
         ...)
{
  OFW_CALL_ARGS Args;
  VA_LIST List;
  int Index;

  Args.Service = Service;
  Args.NumArgs = NumArgs;
  Args.NumRet = NumRet;

  VA_START(List, NumRet);
  for (Index = 0; Index < NumArgs; ++Index) {
    Args.Args[Index] = VA_ARG(List, VOID *);
  }
  VA_END(List);
  for (Index = 0; Index < NumRet; ++Index) {
    Args.Args[Index + NumArgs] = 0;
  }

  OFWEntry(&Args);
  return Args.Args[NumArgs];
}


#endif /* __OFW_LIB_H__ */
