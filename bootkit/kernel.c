/* kernel.c - the C part of the kernel */
/* Copyright (C) 1999  Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "multiboot.h"
#include "src/helper.h"
#include "module/linuxaddr.h"
#include "../include/linux/autoconf.h"

/* Macros.  */

/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

/* Some screen stuff.  */
/* The number of columns.  */
#define COLUMNS			80
/* The number of lines.  */
#define LINES			24
/* The attribute of an character.  */
#define ATTRIBUTE		7
/* The video memory address.  */
#define VIDEO			0xB8000

/* Variables.  */
/* Save the X position.  */
static int xpos;
/* Save the Y position.  */
static int ypos;
/* Point to the video memory.  */
static volatile unsigned char *video;

/* Forward declarations.  */
void cmain (unsigned long magic, unsigned long addr);
void build_page_tables(void);
static void cls (void);
static void itoa (char *buf, int base, int d);
static void putchar (int c);
void printf (const char *format, ...);
multiboot_info_t *mbi;
volatile unsigned mod_start_addr;
volatile unsigned mod_end_addr;
char* cmdline;
int strcmp(const char *cs, const char *ct);

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR.  */
unsigned syscall_start_addr, syscall_end_addr;
void
cmain (unsigned long magic, unsigned long addr)
{
	int i;
  int fixtext_found = 0;
  int syscall_found = 0;
  int sysmap_found = 0;
  unsigned fix_start_addr, fix_end_addr;
  unsigned base = CMP_START;
  unsigned length = 0;
  unsigned int testcnt = 0, *testptr = CMP_OFFSET;

  /* Clear the screen.  */
  cls ();

  /* Am I booted by a Multiboot-compliant boot loader?  */
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
      printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
      return;
    }

  /* Set MBI to the address of the Multiboot information structure.  */
  mbi = (multiboot_info_t *) addr;

  /* Print out the flags.  */
  printf ("flags = 0x%x\n", (unsigned) mbi->flags);

  /* Are mem_* valid?  */
  if (CHECK_FLAG (mbi->flags, 0))
    printf ("mem_lower = %uKB, mem_upper = %uKB\n",
	    (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

  /* Is boot_device valid?  */
  if (CHECK_FLAG (mbi->flags, 1))
      printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

  
  if (CHECK_FLAG (mbi->flags, 6))
  {
      memory_map_t *mmap;
      
      for (mmap = (memory_map_t *) mbi->mmap_addr;
           (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
           mmap = (memory_map_t *) ((unsigned long) mmap
                                    + mmap->size + sizeof (mmap->size)))
      {
          base =  mmap->base_addr_high;
          base = (base << 16) | mmap->base_addr_low;
          length = mmap->length_high;
          length = (length << 16) |  mmap->length_low;
          length -= 0x100000;

          if(length > DUMPMAX)
		length = DUMPMAX;

          if (base == 0x100000) {
              testcnt = (length / 4);
              break;
          }
      }
  }


  /* Are mods_* valid?  */
  if (CHECK_FLAG (mbi->flags, 3))
    {
      module_t *mod;
      
      printf ("mods_count = %d, mods_addr = 0x%x\n",
	      (int) mbi->mods_count, (int) mbi->mods_addr);
      for (i = 0, mod = (module_t *) mbi->mods_addr;
	   i < mbi->mods_count;
	   i++, mod++)
      {
	      printf (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
			      (unsigned) mod->mod_start,
			      (unsigned) mod->mod_end,
			      (char *) mod->string);

	      if(!strcmp((char*) mod->string, "/boot/bootjacker-System.map"))
	      {
	        printf("System.map found!\n");
	        mod_start_addr = (unsigned) mod->mod_start;
	        mod_end_addr = (unsigned) mod->mod_end;
		sysmap_found = 1;
	      }

	      if(!strcmp((char*) mod->string, "/boot/bootjacker-fixtext"))
	      {
	        printf("fixtext found!\n");
	        fix_start_addr = (unsigned) mod->mod_start;
	        fix_end_addr = (unsigned) mod->mod_end;
		fixtext_found = 1;
	      }

	      if(!strcmp((char*) mod->string, "/syscall.bin"))
	      {
	        printf("syscall.bin found!\n");
	        syscall_start_addr = (unsigned) mod->mod_start;
	        syscall_end_addr = (unsigned) mod->mod_end;
		if(syscall_start_addr == 0)
			while(1);
		syscall_found = 1;
	      }
      }
    }

  /* Is the command line passed?  */
  if (CHECK_FLAG (mbi->flags, 2))
  {
    printf ("cmdline = %s\n", (char *) mbi->cmdline);
    cmdline = (char*) mbi->cmdline;
    if(!strcmp(cmdline,"/boot/bootjacker-kernel dmesg"))
			do_dmesg();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel memdump"))
			do_memdump();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel memup"))
			do_memup();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel memcompressdump"))
			do_memcompressdump();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel printtime"))
			do_printtime();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel fatdump"))
			do_fatdump();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel pcilist"))
			do_pcilist();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel memwrite-noflush"))
			do_memwrite_noflush(testptr, testcnt, length);
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel memwrite"))
			do_memwrite(testptr, testcnt);
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel memcmp"))
			do_memcmp(testptr, testcnt, length);
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel vfs"))
			do_vfs();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel syscall"))
			do_syscall();
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel syscallwrite")) {
			do_syscallwrite();
			do_qemu();
    }
    else if(!strcmp(cmdline,"/boot/bootjacker-kernel qemu")) {
			do_pcifill();
			do_qemu();
    } else if(!strcmp(cmdline,"/boot/bootjacker-kernel idle"))
			do_qemuidle();
		else
		{
			printf("Unknown command line: %s. Halting.\n", cmdline);
			while(1);
		}	
  }

  if(!sysmap_found || !fixtext_found)
  {
//    printf("System.map or fixtext not found... halting.\n");
//    while(1);
  }

  /* Bits 4 and 5 are mutually exclusive!  */
  if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
    {
      printf ("Both bits 4 and 5 are set.\n");
      return;
    }

  /* Is the symbol table of a.out valid?  */
  if (CHECK_FLAG (mbi->flags, 4))
    {
      aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);
      
      printf ("aout_symbol_table: tabsize = 0x%0x, "
	      "strsize = 0x%x, addr = 0x%x\n",
	      (unsigned) aout_sym->tabsize,
	      (unsigned) aout_sym->strsize,
	      (unsigned) aout_sym->addr);
    }

  /* Is the section header table of ELF valid?  */
  if (CHECK_FLAG (mbi->flags, 5))
    {
      elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

      printf ("elf_sec: num = %u, size = 0x%x,"
	      " addr = 0x%x, shndx = 0x%x\n",
	      (unsigned) elf_sec->num, (unsigned) elf_sec->size,
	      (unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
    }

  /* Are mmap_* valid?  */
  if (CHECK_FLAG (mbi->flags, 6))
    {
      memory_map_t *mmap;
      
      printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
	      (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
      for (mmap = (memory_map_t *) mbi->mmap_addr;
	   (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
	   mmap = (memory_map_t *) ((unsigned long) mmap
				    + mmap->size + sizeof (mmap->size)))
	printf (" size = 0x%x, base_addr = 0x%x%x,"
		" length = 0x%x%x, type = 0x%x\n",
		(unsigned) mmap->size,
		(unsigned) mmap->base_addr_high,
		(unsigned) mmap->base_addr_low,
		(unsigned) mmap->length_high,
		(unsigned) mmap->length_low,
		(unsigned) mmap->type);
    }

  /* Check if valid Linux is loaded */
  int kmagic = *((int*)(kk_linux_banner - LINUX_PAGE_OFFSET));
  int goodmagic = 0x756e694c; // Linux
  if(kmagic != goodmagic)
  {
    printf("Linux kernel magic not present. BADMAGIC=%x GOODMAGIC=%x\n", kmagic, goodmagic);
    printf("Please boot Linux first.\n");
    while(1);
  }

#ifdef FIX_TEXT  
  count = memcmp(fix_start_addr, LINUX_PHYS_OFFSET, fix_end_addr - fix_start_addr);
  if(count)
  {
    printf("Number of clobbered bytes = %d\n", count);

    show_bad_bits(fix_start_addr, LINUX_PHYS_OFFSET, fix_end_addr - fix_start_addr, 10);
    printf("Number of clobbered bytes = %d size=%d    \n", count, fix_end_addr - fix_start_addr);
    if(count > 4000)
    {
      // Linux modifies 3119 bytes of its own FP code
      printf("Too many bit errors... halting...    \n");
      while(1);
    }
    /* Fix clobbered text */
    printf("Fixing clobbered text from %x to %x\n", LINUX_TEXT_START, LINUX_TEXT_START + fix_end_addr - fix_start_addr);
    memcpy(LINUX_TEXT_START - LINUX_PAGE_OFFSET, fix_start_addr, fix_end_addr - fix_start_addr);
  }
#endif

  build_page_tables();
}    

/* Clear the screen and initialize VIDEO, XPOS and YPOS.  */
void
cls (void)
{
  int i;

  video = (unsigned char *) VIDEO;
  
  for (i = 0; i < COLUMNS * LINES * 2; i++)
    *(video + i) = 0;

  xpos = 0;
  ypos = 0;
}

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal.  */
static void
itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /* If %d is specified and D is minus, put `-' in the head.  */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;

  /* Divide UD by DIVISOR until UD == 0.  */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);

  /* Terminate BUF.  */
  *p = 0;
  
  /* Reverse BUF.  */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}

/* Put the character C on the screen.  */
static void
putchar (int c)
{
  if (c == '\n' || c == '\r')
    {
    newline:
      xpos = 0;
      ypos++;
      if (ypos >= LINES)
	ypos = 0;
      return;
    }

  *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
  *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

  xpos++;
  if (xpos >= COLUMNS)
    goto newline;
}

/* Format a string and print it on the screen, just like the libc
   function printf.  */
void
printf (const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
	putchar (c);
      else
	{
	  char *p;
	  
	  c = *format++;
	  switch (c)
	    {
	    case 'd':
	    case 'u':
	    case 'x':
	      itoa (buf, c, *((int *) arg++));
	      p = buf;
	      goto string;
	      break;

	    case 's':
	      p = *arg++;
	      if (! p)
		p = "(null)";

	    string:
	      while (*p)
		putchar (*p++);
	      break;

	    default:
	      putchar (*((int *) arg++));
	      break;
	    }
	}
    }
}

unsigned pde_start;
extern unsigned bj_page_offset;

void build_page_tables(void)
{
	/* 4k page table base */
	/* 512mb of page tables mapped at 0 and PAGE_OFFSET */
	/* 128 2nd level tables, 4 kb each *2 = 1mb total */ 
	/* 2mb total allocated */
	const unsigned memsize = 512 * 1024 * 1024;
#define PDE_SIZE (1024*1024*4)
	unsigned pa, i, tmp;
	unsigned int *pageptr = (unsigned int*) (((kk_page_buf & 0xfffff000) - bj_page_offset + 4096));
	unsigned int *pteptr  = (unsigned int*) (((unsigned)pageptr) + 4096);
	pde_start = (unsigned) pageptr;
	printf("Making page tables for %d bytes at %x\n", memsize, pde_start);

	for(pa = 0; pa < memsize; pa += PDE_SIZE)
	{
		tmp = pa >> (20 + 2);
//		printf("Wrote %x (PDE) to %x (PTE)\n", tmp, (unsigned)pteptr);
		pageptr[tmp] = (unsigned)pteptr | 0x7; /* R/W */

		// Fill in PDE
		for(i = 0; i < 1024; i++)
		{
			*pteptr++ = (pa & 0xfffff000)  + (i * 4096) + 0x7;
//			printf("Mapping (pa) %x -> (va) %x\n", pa + i*4096, pa + i*4096);
		}
	}

	for(pa = bj_page_offset; pa < (memsize + bj_page_offset); pa += PDE_SIZE)
	{
		tmp = pa >> (20 + 2);
		pageptr[tmp] = (unsigned)pteptr | 0x7; /* R/W */

		// Fill in PDE
		for(i = 0; i < 1024; i++)
		{
			*pteptr++ = ((pa - bj_page_offset) & 0xfffff000)  + (i * 4096) + 0x7;
		}
	}
//	while(1);
}
