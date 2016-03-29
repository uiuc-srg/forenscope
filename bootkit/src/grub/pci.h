/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2008  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	GRUB_CPU_PCI_H
#define	GRUB_CPU_PCI_H	1

//#include <grub/types.h>
#include "io.h"

typedef unsigned grub_uint32_t;
typedef grub_uint32_t grub_pci_address_t;
typedef grub_uint32_t grub_pci_id_t;
typedef int (*grub_pci_iteratefunc_t) (int bus, int device, int func,
                                       grub_pci_id_t pciid);
typedef grub_uint32_t grub_err_t;
#define GRUB_ERR_NONE 0

int grub_printf (const char *fmt, ...);
#define grub_printf printf

grub_pci_address_t
grub_pci_make_address (int bus, int device, int function, int reg);
void grub_pci_iterate (grub_pci_iteratefunc_t hook);
int bootjacker_lspci (void);

#define GRUB_PCI_ADDR_REG	0xcf8
#define GRUB_PCI_DATA_REG	0xcfc

static inline grub_uint32_t
grub_pci_read (grub_pci_address_t addr)
{
  grub_outl (addr, GRUB_PCI_ADDR_REG);
  return grub_inl (GRUB_PCI_DATA_REG);
}

#endif /* GRUB_CPU_PCI_H */