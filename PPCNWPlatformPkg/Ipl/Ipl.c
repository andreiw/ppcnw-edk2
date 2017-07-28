/** @file
  The UEFI Ipl for NewWorld OpenFirmware-based systems.

  Andrei Warkentin <andrey.warkentin@gmail.com>

  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Uefi.h>
#include <Library/OFWLib.h>

/**
  The C Entry Point for the IPL.

  @param[in] a1    Unknown.
  @param[in] a2    Unknown.
  @param[in] prom  OpenFirmware CIF.

**/
VOID
CEntryPoint (
             VOID *a1,
             VOID *a2,
             VOID *prom
             )
{
  OFW_Call(prom, "interpret", 1, 1, ".\" Hello from UEFI IPL on a PowerPC NewWorld!\" cr");
  OFW_Call(prom, "exit", 0, 0);
  while (1);
}
