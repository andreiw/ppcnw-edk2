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

#include <Uefi.h>
#include <Pcr.h>
#include <Library/OFWLib.h>

typedef struct {
  CHAR8 *Service;
  UINTN NumArgs;
  UINTN NumRet;
  VOID *Args[OFW_CALL_MAX_ARGS_RETS];
} OFW_CALL_ARGS;

VOID
OFWIplInitialize(VOID)
{
  OFW_PHANDLE Chosen;
  OFW_IHANDLE Stdout;
  OFW_IHANDLE Screen;

  OFWCall("finddevice", 1, 1, "/chosen", &Chosen);
  PcrGet()->OFPChosen = (UINTN) Chosen;

  Screen = (OFW_IHANDLE) -1;
  OFWCall("interpret", 1, 2,
          "\" _screen-ihandle\" $find if execute else 0 then",
          NULL, &Screen);
  if (Screen == (OFW_IHANDLE) 0 || Screen == (OFW_IHANDLE) -1) {
    OFWCall("open", 1, 1, "screen", NULL);
  }

  OFWGetProp(Chosen, "stdout", &Stdout, sizeof(Stdout));
  PcrGet()->OFIStdout = (UINTN) Stdout;
}

OFW_RETVAL
OFWCall(IN     char *Service,
        IN     int NumArgs,
        IN     int NumRet,
        IN OUT ...)
{
  INTN (*OFWEntry)(void *);
  OFW_CALL_ARGS Args;
  VA_LIST List;
  UINTN Index;
  INTN RetVal;

  OFWEntry = (VOID *) PcrGet()->OFEntry;
  Args.Service = Service;
  Args.NumArgs = NumArgs;
  Args.NumRet = NumRet;

  VA_START(List, NumRet);
  for (Index = 0; Index < NumArgs; Index++) {
    Args.Args[Index] = VA_ARG(List, VOID *);
  }

  for (Index = 0; Index < NumRet; Index++) {
    Args.Args[Index + NumArgs] = 0;
  }

  RetVal = OFWEntry(&Args);
  if (RetVal == 0) {
    if (NumRet > 0) {
      for (Index = 0; Index < NumRet; Index++) {
        VOID **RetPointer = VA_ARG(List, VOID **);

        if (RetPointer != NULL) {
          *RetPointer = Args.Args[Index + NumArgs];
        }
      }
    }
  }

  VA_END(List);

  return RetVal;
}


