#/** @file
# PowerPC 32-bit BE
#
# Copyright (c) 2017, Andrei Warkentin <andrey.warkentin@gmail.com>
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = PPCPkg
  PLATFORM_GUID                  = 5CFBD99E-3C43-4E7F-8054-9CDEAFF7711E
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/PPC
  SUPPORTED_ARCHITECTURES        = PPC
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[BuildOptions]
  RELEASE_*_*_CC_FLAGS  = -DMDEPKG_NDEBUG

[LibraryClasses.common]

[LibraryClasses.common.DXE_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.SEC]

[PcdsFixedAtBuild.common]

[Components.common]
  PPCPkg/Applications/Hello/Hello.inf
