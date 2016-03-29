/////////////////////////////////////////////////////////////////////////
// $Id: pit.h,v 1.15 2006/05/27 15:54:48 sshwarts Exp $
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

#ifndef _BX_PIT_H
#define _BX_PIT_H

#include "config.h"

#if (BX_USE_NEW_PIT==0)

#if BX_USE_PIT_SMF
#  define BX_PIT_SMF  static
#  define BX_PIT_THIS bx_pit.
#else
#  define BX_PIT_SMF
#  define BX_PIT_THIS this->
#endif

#ifdef OUT
#  undef OUT
#endif


typedef struct {
  Bit8u      mode;
  Bit8u      latch_mode;
  Bit16u     input_latch_value;
  bx_bool    input_latch_toggle;
  Bit16u     output_latch_value;
  bx_bool    output_latch_toggle;
  bx_bool    output_latch_full;
  Bit16u     counter_max;
  Bit16u     counter;
  bx_bool    bcd_mode;
  bx_bool    active;
  bx_bool    GATE;     // GATE input  pin
  bx_bool    OUT;      // OUT  output pin
} bx_pit_t;

class bx_pit_c : public logfunctions {
public:
  bx_pit_c();
  virtual ~bx_pit_c();
  BX_PIT_SMF int init(void);
  BX_PIT_SMF void reset( unsigned type);
  BX_PIT_SMF bx_bool periodic( Bit32u   usec_delta );
#if BX_SUPPORT_SAVE_RESTORE
  BX_PIT_SMF void register_state(void);
#endif

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PIT_SMF
  Bit32u   read( Bit32u   addr, unsigned int len );
  void write( Bit32u   addr, Bit32u   Value, unsigned int len );
#endif

  struct s_type {
    bx_pit_t timer[3];
    Bit8u   speaker_data_on;
    bx_bool refresh_clock_div2;
    int  timer_handle[3];
  } s;

  BX_PIT_SMF void  write_count_reg( Bit8u   value, unsigned timerid );
  BX_PIT_SMF Bit8u read_counter( unsigned timerid );
  BX_PIT_SMF void  latch( unsigned timerid );
  BX_PIT_SMF void  set_GATE(unsigned pit_id, unsigned value);
  BX_PIT_SMF void  start(unsigned timerid);
};

extern bx_pit_c bx_pit;

#endif  // #if (BX_USE_NEW_PIT==0)
#endif  // #ifndef _BX_PIT_H
