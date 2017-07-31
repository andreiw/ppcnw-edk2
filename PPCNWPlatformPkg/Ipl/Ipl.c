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
#include <Library/OFWLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Pcr.h>

VOID
Log (
     IN CONST CHAR16 *FormatString,
     ...
     )
{
  VA_LIST Marker;
  UINTN   Count;
  CHAR8 Buffer[100];

  VA_START (Marker, FormatString);
  Count = AsciiVSPrintUnicodeFormat (Buffer, sizeof(Buffer),
                                     FormatString, Marker);
  VA_END (Marker);

  SerialPortWrite ((UINT8 *) Buffer, Count);
}

VOID
CEntryPoint (VOID)
{
  OFWIplInitialize();
  SerialPortInitialize();

  Log (L"\n%s UEFI firmware (built at %a on %a)\n",
       (CHAR16*) PcdGetPtr (PcdFirmwareVersionString),
       __TIME__, __DATE__);

  DEBUG((EFI_D_INFO, "FD is at 0x%x-0x%x\n",
         PcdGet32(PcdFdBaseAddress),
         PcdGet32(PcdFdBaseAddress) + PcdGet32(PcdFdSize)));
  DEBUG((EFI_D_INFO, "FV is at 0x%x-0x%x\n",
         PcdGet32(PcdFvBaseAddress),
         PcdGet32(PcdFvBaseAddress) + PcdGet32(PcdFvSize)));

  while (1);
}
