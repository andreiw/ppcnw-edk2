/*
 * Copyright (c) 2017, Andrei Warkentin <andrey.warkentin@gmail.com>
 *
 * This program and the accompanying materials
 * are licensed and made available under the terms and conditions of the BSD License
 * which accompanies this distribution.  The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*/

SECTIONS {
  .text : {
    *(.text .text.*)
  }

  /DISCARD/ : {
    *(.note.GNU-stack)
    *(.gnu_debuglink)
    *(.interp)
    *(.dynsym)
    *(.dynstr)
    *(.dynamic)
    *(.hash)
    *(.comment)
    *(.shstrtab)
  }
}

