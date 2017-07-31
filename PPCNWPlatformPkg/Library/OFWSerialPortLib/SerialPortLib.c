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
#include <Pcr.h>

RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  return RETURN_SUCCESS;
}

/**
  Write data to serial device.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Write data failed.
  @retval !0               Actual number of bytes writed to serial device.

**/
UINTN
EFIAPI
SerialPortWrite (
                 IN UINT8 *Buffer,
                 IN UINTN NumberOfBytes
                 )
{
  INTN BytesWritten;
  OFW_RETVAL RetVal;

  RetVal = OFWCall("write", 3, 1,
                   PcrGet()->OFIStdout, Buffer,
                   NumberOfBytes, &BytesWritten);
  if (RetVal != 0) {
    return 0;
  }

  if (BytesWritten == -1) {
    return 0;
  }

  return (UINTN) BytesWritten;
}

/**
  Read data from serial device and save the datas in buffer.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Read data failed.
  @retval !0               Aactual number of bytes read from serial device.

**/
UINTN
EFIAPI
SerialPortRead (
                OUT UINT8 *Buffer,
                IN  UINTN NumberOfBytes
                )
{
  return 0;
}

/**
  Check to see if any data is avaiable to be read from the debug device.

  @retval TRUE       At least one byte of data is avaiable to be read
  @retval FALSE      No data is avaiable to be read

**/
BOOLEAN
EFIAPI
SerialPortPoll (
                VOID
                )
{
  return FALSE;
}

