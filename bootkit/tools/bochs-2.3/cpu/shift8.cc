/////////////////////////////////////////////////////////////////////////
// $Id: shift8.cc,v 1.24 2006/03/26 18:58:01 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


void BX_CPU_C::ROL_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }

  if ( (count & 0x07) == 0 ) {
    if ( count & 0x18 ) {
      unsigned bit0 = op1_8 & 1;
      set_CF(bit0);
      set_OF(bit0 ^ (op1_8 >> 7));
    }
    return;
  }
  count &= 0x07; // use only lowest 3 bits

  result_8 = (op1_8 << count) | (op1_8 >> (8 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  /* set eflags:
   * ROL count affects the following flags: C, O
   */
  bx_bool temp_CF = (result_8 & 0x01);

  set_CF(temp_CF);
  set_OF(temp_CF ^ (result_8 >> 7));
}

void BX_CPU_C::ROR_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }

  if ( (count & 0x07) == 0 ) {
    if ( count & 0x18 ) {
      unsigned bit6 = (op1_8 >> 6) & 1;
      unsigned bit7 = (op1_8 >> 7);
      set_CF(bit7);
      set_OF(bit7 ^ bit6);
    }
    return;
  }
  count &= 0x07; /* use only bottom 3 bits */

  result_8 = (op1_8 >> count) | (op1_8 << (8 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  /* set eflags:
   * ROR count affects the following flags: C, O
   */
  bx_bool result_b7 = (result_8 & 0x80) != 0;
  bx_bool result_b6 = (result_8 & 0x40) != 0;

  set_CF(result_b7);
  set_OF(result_b7 ^ result_b6);
}

void BX_CPU_C::RCL_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count = (count & 0x1f) % 9;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }
 
  if (! count) return;

  if (count==1) {
    result_8 = (op1_8 << 1) | getB_CF();
  }
  else {
    result_8 = (op1_8 << count) | (getB_CF() << (count - 1)) |
             (op1_8 >> (9 - count));
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  /* set eflags:
   * RCL count affects the following flags: C, O
   */
  bx_bool temp_CF = (op1_8 >> (8 - count)) & 0x01;

  set_CF(temp_CF);
  set_OF(temp_CF ^ (result_8 >> 7));
}

void BX_CPU_C::RCR_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count = (count & 0x1f) % 9;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }

  if (! count) return;

  result_8 = (op1_8 >> count) | (getB_CF() << (8 - count)) |
             (op1_8 << (9 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  /* set eflags:
   * RCR count affects the following flags: C, O
   */

  set_CF((op1_8 >> (count - 1)) & 0x01);
  set_OF((((result_8 << 1) ^ result_8) & 0x80) > 0);
}

void BX_CPU_C::SHL_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }

  if (!count) return;

  result_8 = (op1_8 << count);

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  SET_FLAGS_OSZAPC_8(op1_8, count, result_8, BX_INSTR_SHL8);
}


void BX_CPU_C::SHR_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }

  if (!count) return;

  result_8 = (op1_8 >> count);

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  SET_FLAGS_OSZAPC_8(op1_8, count, result_8, BX_INSTR_SHR8);
}

void BX_CPU_C::SAR_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1() == 0xc0)
    count = i->Ib();
  else if (i->b1() == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  }

  if (!count) return;

  if (count < 8) {
    if (op1_8 & 0x80) {
      result_8 = (op1_8 >> count) | (0xff << (8 - count));
    }
    else {
      result_8 = (op1_8 >> count);
    }
  }
  else {
    if (op1_8 & 0x80) {
      result_8 = 0xff;
    }
    else {
      result_8 = 0;
    }
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_RMW_virtual_byte(result_8);
  }

  SET_FLAGS_OSZAPC_8(op1_8, count, result_8, BX_INSTR_SAR8);
}
