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
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/OFWLib.h>
#include <Library/HobLib.h>
#include <Library/PrePiLib.h>
#include <Library/PrePiHobListPointerLib.h>
#include <Pcr.h>

static EFI_STATUS
ScanMemory (IN VOID *TemporaryBuffer)
{
  UINT32 *End;
  UINT32 Base;
  UINT32 Length;
  UINT32 *Value; 
  UINT32 MemRegLength;

  if (!OFWGetPropLen((VOID *) PcrGet()->OFPMemory,
                     "reg", &MemRegLength)) {
    return EFI_UNSUPPORTED;
  }

  if (!OFWGetProp((VOID *) PcrGet()->OFPMemory, "reg",
                  TemporaryBuffer, MemRegLength)) {
    return EFI_DEVICE_ERROR;
  }

  Value = TemporaryBuffer;
  End = (VOID *) (((UINTN) Value) + MemRegLength);
  Base = 0;
  Length = 0;

  while (Value < End) {
    if (Length == 0) {
      Base = *Value;
      Length = *(Value + 1);
    } else if ((Base + Length) != *Value) {
      DEBUG((EFI_D_WARN,
             "Ignoring non-contiguous range 0x%x-0x%x\n",
             *Value, *(Value + 1)));
    } else {
      Length += *(Value + 1);
    }

    Value += 2;
  }

  PatchPcdSet32(PcdSystemMemoryBase, Base);
  PatchPcdSet32(PcdSystemMemorySize, Length);
  return EFI_SUCCESS;
}

VOID
CEntryPoint (
             IN UINTN IplReservedBottom,
             IN UINTN StackBase,
             IN UINTN IplFreeMemoryBottom,
             IN UINTN IplMemoryTop
             )
{
  EFI_STATUS Status;
  EFI_HOB_HANDOFF_INFO_TABLE *HobList;

  OFWIplInitialize();
  SerialPortInitialize();

  DEBUG((EFI_D_INFO, "\n%s UEFI firmware (built %a %a)\n",
         (CHAR16*) PcdGetPtr(PcdFirmwareVersionString),
         __TIME__, __DATE__));

  DEBUG((EFI_D_INFO, "Ipl total @ 0x%x-0x%x\n",
         IplReservedBottom, IplMemoryTop));
  DEBUG((EFI_D_INFO, "Ipl used  @ 0x%x-0x%x\n",
         IplReservedBottom, IplFreeMemoryBottom));
  DEBUG((EFI_D_INFO, "Stack     @ 0x%x-0x%x\n",
         StackBase, StackBase +
         PcdGet32 (PcdCPUCorePrimaryStackSize)));
  DEBUG((EFI_D_INFO, "Ipl free  @ 0x%x-0x%x\n",
         IplFreeMemoryBottom, IplMemoryTop));
  DEBUG((EFI_D_INFO, "FV        @ 0x%x-0x%x\n",
         PcdGet32(PcdFvBaseAddress),
         PcdGet32(PcdFvBaseAddress) + PcdGet32(PcdFvSize)));

  Status = ScanMemory((VOID *) IplFreeMemoryBottom);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "No system memory: %r", Status));
    goto err;
  }

  DEBUG((EFI_D_INFO, "RAM       @ 0x%x-0x%x\n",
         PcdGet32(PcdSystemMemoryBase),
         PcdGet32(PcdSystemMemoryBase) +
         PcdGet32(PcdSystemMemorySize)));

  /*
   * Claim PHIT memory entire system RAM range:
   * this lets us use BuildMemoryAllocationHob
   * to cover reserved allocations.
   */
  HobList = HobConstructor((VOID *) 0,
                           PcdGet32(PcdSystemMemoryBase) +
                           PcdGet32(PcdSystemMemorySize),
                           (VOID *) IplFreeMemoryBottom,
                           (VOID *) IplMemoryTop);
  PrePeiSetHobList(HobList);

  /*
   * [IplReservedBottom, IplFreeMemoryBottom) is
   * RAM reserved for FD, stack and PCR.
   */
  BuildMemoryAllocationHob(IplReservedBottom,
                            IplFreeMemoryBottom -
                           IplReservedBottom,
                           EfiBootServicesData);

  /*
   * Describe system RAM and reserved OFW ranges.
   */

  /*
   * DXE stack (which is our stack as well before we transfer control).
   */
  BuildStackHob(StackBase, PcdGet32(PcdCPUCorePrimaryStackSize));

  BuildCpuHob(PcdGet8(PcdPrePiCpuMemorySize),
              PcdGet8(PcdPrePiCpuIoSize));
  SetBootMode(BOOT_WITH_FULL_CONFIGURATION);
  BuildFvHob(PcdGet32 (PcdFvBaseAddress), PcdGet32(PcdFvSize));

err:
  DEBUG((EFI_D_ERROR, "IPL failed\n"));
  while (1);
}
