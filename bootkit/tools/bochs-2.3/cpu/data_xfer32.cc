/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer32.cc,v 1.39 2006/06/26 20:28:00 sshwarts Exp $
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

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RAX EAX
#endif

void BX_CPU_C::XCHG_ERXEAX(bxInstruction_c *i)
{
  Bit32u temp32 = EAX;
  RAX = BX_READ_32BIT_REG(i->opcodeReg());
  BX_WRITE_32BIT_REGZ(i->opcodeReg(), temp32);
}

void BX_CPU_C::MOV_ERXId(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->opcodeReg(), i->Id());
}

void BX_CPU_C::MOV_EEdGd(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), RMAddr(i), &BX_READ_32BIT_REG(i->nnn()));
}

void BX_CPU_C::MOV_EGdGd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->nnn());
  BX_WRITE_32BIT_REGZ(i->rm(), op2_32);
}

void BX_CPU_C::MOV_GdEGd(bxInstruction_c *i)
{
  // 2nd modRM operand Ex, is known to be a general register Gd.
  Bit32u op2_32 = BX_READ_32BIT_REG(i->rm());
  BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
}

void BX_CPU_C::MOV_GdEEd(bxInstruction_c *i)
{
  // 2nd modRM operand Ex, is known to be a memory operand, Ed.
  read_virtual_dword(i->seg(), RMAddr(i), &BX_READ_32BIT_REG(i->nnn()));
  BX_CLEAR_64BIT_HIGH(i->nnn());
}

void BX_CPU_C::LEA_GdM(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_INFO(("LEA_GdM: op2 is a register"));
    UndefinedOpcode(i);
  }

  /* write effective address of op2 in op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), RMAddr(i));
}

void BX_CPU_C::MOV_EAXOd(bxInstruction_c *i)
{
  read_virtual_dword(i->seg(), i->Id(), &EAX);
  BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RAX);
}

void BX_CPU_C::MOV_OdEAX(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), i->Id(), &EAX);
}

void BX_CPU_C::MOV_EdId(bxInstruction_c *i)
{
  Bit32u op2_32 = i->Id();

  /* now write sum back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), op2_32);
  }
  else {
    write_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }
}

#if BX_CPU_LEVEL >= 3
void BX_CPU_C::MOVZX_GdEb(bxInstruction_c *i)
{
  Bit8u  op2_8;

  if (i->modC0()) {
    op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  }

  /* zero extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit32u) op2_8);
}

void BX_CPU_C::MOVZX_GdEw(bxInstruction_c *i)
{
  Bit16u op2_16;

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  /* zero extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit32u) op2_16);
}

void BX_CPU_C::MOVSX_GdEb(bxInstruction_c *i)
{
  Bit8u op2_8;

  if (i->modC0()) {
    op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  }

  /* sign extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GdEw(bxInstruction_c *i)
{
  Bit16u op2_16;

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  /* sign extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit16s) op2_16);
}
#endif

void BX_CPU_C::XCHG_EdGd(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    BX_WRITE_32BIT_REGZ(i->rm(), op2_32);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    write_RMW_virtual_dword(op2_32);
  }

  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
}

void BX_CPU_C::CMOV_GdEd(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  // Note: CMOV accesses a memory source operand (read), regardless
  //       of whether condition is true or not.  Thus, exceptions may
  //       occur even if the MOV does not take place.

  bx_bool condition = 0;
  Bit32u op2_32;

  switch (i->b1()) {
    // CMOV opcodes:
    case 0x140: condition = get_OF(); break;
    case 0x141: condition = !get_OF(); break;
    case 0x142: condition = get_CF(); break;
    case 0x143: condition = !get_CF(); break;
    case 0x144: condition = get_ZF(); break;
    case 0x145: condition = !get_ZF(); break;
    case 0x146: condition = get_CF() || get_ZF(); break;
    case 0x147: condition = !get_CF() && !get_ZF(); break;
    case 0x148: condition = get_SF(); break;
    case 0x149: condition = !get_SF(); break;
    case 0x14A: condition = get_PF(); break;
    case 0x14B: condition = !get_PF(); break;
    case 0x14C: condition = getB_SF() != getB_OF(); break;
    case 0x14D: condition = getB_SF() == getB_OF(); break;
    case 0x14E: condition = get_ZF() || (getB_SF() != getB_OF()); break;
    case 0x14F: condition = !get_ZF() && (getB_SF() == getB_OF()); break;
    default:
      BX_PANIC(("CMOV_GdEd: default case"));
  }

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  if (condition) {
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  }
#else
  BX_INFO(("CMOV_GdEd: -enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}
