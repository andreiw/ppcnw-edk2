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

#ifndef __PCR_H__
#define __PCR_H__

#ifndef __ASSEMBLY__
typedef struct _PCR {
  UINT32 OFEntry;
  UINT32 OFPChosen;
  UINT32 OFPMemory;
  UINT32 OFIStdout;
} PCR;

STATIC inline PCR *
PcrGet(void)
{
	register PCR *pcr asm("r2");
	return pcr;
}
#else
#define PCR_OFEntry   0
#define PCR_OFPChosen 4
#define PCR_OFPMemory 8
#define PCR_OFIStdout 12
#define PCR_Size      0x1000
#define PCR_R(name, reg) (PCR_ ## name)(reg)
#define PCR(name) PCR_R(name, r2)

#endif // __ASSEMBLY__
#endif // __PCR_H__

