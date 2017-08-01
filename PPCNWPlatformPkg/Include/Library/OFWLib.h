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

typedef VOID *OFW_IHANDLE;
typedef VOID *OFW_PHANDLE;
typedef INTN OFW_RETVAL;

VOID
OFWIplInitialize(VOID);

OFW_RETVAL
OFWCall(char *Service,
        int NumArgs,
        int NumRet,
        ...);

STATIC inline BOOLEAN
OFWGetProp(IN  OFW_PHANDLE Node,
           IN  CHAR8 *Name,
           OUT VOID *Buf,
           IN  UINT32 Length)
{
  OFW_RETVAL CallRet;
  OFW_RETVAL CIFRet = OFWCall("getprop", 4, 1, Node,
                              Name, Buf, Length, &CallRet);
  if (CIFRet != 0) {
    return FALSE;
  }

  if (CallRet == -1) {
    return FALSE;
  }

  return TRUE;
}

STATIC inline BOOLEAN
OFWGetPropLen(IN  OFW_PHANDLE Node,
              IN  CHAR8 *Name,
              OUT UINT32 *Length)
{
  OFW_RETVAL CallRet;
  OFW_RETVAL CIFRet = OFWCall("getproplen", 2, 1, Node,
                              Name, &CallRet);
  if (CIFRet != 0) {
    return FALSE;
  }

  if (CallRet == -1) {
    return FALSE;
  }

  *Length = CallRet;
  return TRUE;
}

#endif /* __OFW_LIB_H__ */
