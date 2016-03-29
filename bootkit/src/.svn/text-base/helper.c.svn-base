#include "helper.h"
#include "grub_addr.h"
#include "grub/pci.h"

#include "md5.h"

#define NUMRANGES 	100	
#define NUMCORRECT 	(1024*1024*2)
//#define NUMCORRECT 	4096
#define NUMTHRESHOLD	(1024*1024*32)
unsigned range_starts[NUMRANGES];
unsigned range_ends[NUMRANGES];

char debug_msg[20] = "shivaram";
int (*s_grub_open) (char *filename) = 		(void*) kk_grub_open;
int (*s_grub_read) (char *buf, int len) = 	(void*) kk_grub_read;
int (*s_fat_read) (char *buf, int len) = 	(void*) kk_fat_read;
void (*s_print_fsys_type) (void) = 		(void*) kk_print_fsys_type;
int (*s_blocklist_func) (char*, int) = 		(void*) kk_blocklist_func;
int (*s_set_device) (char*) = 			(void*) kk_set_device;
void (*s_setup_part) (char*) = 			(void*) kk_setup_part;
void (*s_open_device) (void) = 			(void*) kk_open_device;
int *s_disk_read_hook = 			(void*) kk_disk_read_hook;
int *s_disk_read_func = 			(void*) kk_disk_read_func;

void do_dmesg(void)
{
	printf("Printing kernel ringbuffer...\n");
	printf("%s\n", kk___log_buf - LINUX_PAGE_OFFSET);
	while(1);
}

void do_qemu(void)
{
}

void do_qemuidle(void)
{
}

void do_memwrite_noflush(int* testptr, unsigned int testcnt, unsigned int length)
{
	printf("Writing memory... 0x%x to 0x%x\n", testptr, testptr + testcnt);

	while(testcnt--)
	{
		if(testcnt % 0x100000 == 0)
			printf(".");
		*testptr++ = TESTPATTERN;
#ifndef QEMU
		asm("clflush (%0)" : : "r"(testptr - 1));
#endif
	}

	printf("Done writing... rebooting...\n");
#ifdef QEMU
	/* Cause QEMU to reboot */
#else
	/* Triple fault reboot */
	asm("ljmp 0xf0f0f0f0");
#endif
	while(1);
}

void do_memwrite(int* testptr, unsigned int testcnt)
{
	printf("Writing memory... 0x%x to 0x%x\n", testptr, testptr + testcnt);

	while(testcnt--)
	{
		if(testcnt % 0x100000 == 0)
			printf(".");
		*testptr++ = TESTPATTERN;
#ifndef QEMU
		asm("clflush (%0)" : : "r"(testptr - 1));
#endif
	}

	printf("Done writing... rebooting...\n");
#ifdef QEMU
	/* Cause QEMU to reboot */
#else
	/* Triple fault reboot */
	asm("ljmp 0xf0f0f0f0");
#endif
	while(1);
}

int (*rawwrite)(int drive, int sector, char *buf) = (void*) kk_rawwrite;
int (*rawread)(int drive, int sector, int byte_offset, int byte_len, char *buf) = (void*) kk_rawread;

unsigned int get_time(void)
{
	unsigned int start_hr = read_cmos_time(CMOS_HOURS), start_min = read_cmos_time(CMOS_MINUTES), start_sec = read_cmos_time(CMOS_SECONDS);
	return start_hr*3600 + start_min*60 + start_sec;
}

void do_memup(void)
{
	int* s_current_drive = (void*)kk_current_drive;
	int dumpsector = 2;
	void *nextsector = (void*) MEM_START; 
	char sectorbuf[512];
	int* errnum = kk_errnum;	
	int ret = 1;
        while((int)nextsector < (int)MEM_LIMIT)
	{	
		ret = rawread(*s_current_drive, dumpsector, 0, 512, (char *) sectorbuf);
		if(ret == 0)
		{
			printf("Cowardly refusing to read from the dump err=%d\n", *errnum);
			while(1);
		}
		memcpy(nextsector, (void *)sectorbuf, 512);
		nextsector = (void *)((unsigned int)nextsector + 512);
		dumpsector++;
		printf("Restored to %d and %x%x\n",dumpsector, (int)sectorbuf[1]);
		}
	printf("Done restoring to %d\n", *s_current_drive);
	printf("first 10 bytes of last 512 bytes are %x%x \n", ((int*)sectorbuf)[0],((int*)sectorbuf)[1]);
	printf("ended with nextsector as %x\n", (unsigned)nextsector);
}
	
void do_memdump(void)
{
	int* s_current_drive = (void*)kk_current_drive;
	int dumpsector = 2, mb = 0;
	void *nextsector = (void*) MEM_START; //start at start of extended memory
	char sectorbuf[512];

	printf("System time: %d:%d:%d GMT\n", read_cmos_time(CMOS_HOURS), read_cmos_time(CMOS_MINUTES), read_cmos_time(CMOS_SECONDS));
	printf("System date: %d/%d/%d\n", read_cmos_time(CMOS_MONTH), read_cmos_time(CMOS_DATE), read_cmos_time(CMOS_YEAR));

	unsigned int start_time = get_time();
	unsigned int end_time =  start_time;

	unsigned int print = 1;
	
	while((int)nextsector < (int)MEM_LIMIT)
	{	
		memcpy((void *)sectorbuf, nextsector, 512);
		if (print == 1) {
			printf("first 10 bytes are %x%x \n", ((int*)sectorbuf)[0],
				((int*)sectorbuf)[1]);
			print = 0;
		}
		rawwrite(*s_current_drive, dumpsector, (void*) sectorbuf);
		if(dumpsector % 2048 == 0) // Once per MB
		{
			end_time = get_time();
		        printf("SS: %d \t %d buf=%x\n",*s_current_drive, dumpsector, (void*)sectorbuf);
			printf("memory addr at %x dumped to disk sector %d time:%d mb:%d\n", (unsigned)nextsector, dumpsector, end_time - start_time, ++mb);
		}
		nextsector = (void *)((int)nextsector + 512);
		dumpsector++;
	}

	printf("Done dumping to %d\n", *s_current_drive);
	printf("first 10 bytes of last 512 bytes are %x%x \n", ((int*)sectorbuf)[0],
			((int*)sectorbuf)[1]);
	printf("ended with nextsector as %x\n", (unsigned)nextsector);

	while(1);
}

void do_printtime(void)
{
	printf("System time: %d:%d:%d GMT\n", read_cmos_time(CMOS_HOURS), read_cmos_time(CMOS_MINUTES), read_cmos_time(CMOS_SECONDS));
	printf("System date: %d/%d/%d\n", read_cmos_time(CMOS_MONTH), read_cmos_time(CMOS_DATE), read_cmos_time(CMOS_YEAR));
	while(1);
}

void do_pcifill(void)
{
	bootjacker_fillpci();
}

void do_pcilist(void)
{
	do_pcifill();
  	grub_printf("%s", pci_list_str);
	while(1);
}

void do_memcmp(int* testptr, int testcnt, int length)
{
	int badcount = 0;
	int i;
	unsigned maxgood = 0;
	unsigned numcorrect = 0;
	unsigned range_index = 0;
	int *lastcorrect = 0;

	printf("Checking memory... 0x%x to 0x%x\n", testptr, testptr + testcnt);

	for (i = 0; i < NUMRANGES; i++) {
		range_starts[i] = 0;
		range_ends[i] = 0;
	}

	while(testcnt--)
	{
		if(*testptr++ != TESTPATTERN)
		{
			badcount++;

			if (numcorrect > maxgood) maxgood = numcorrect;
			if (numcorrect >= NUMCORRECT) {
				range_ends[range_index] = (int) (testptr-1);
				range_index++;
				if (range_index == NUMRANGES) {
					printf("Number of valid ranges exceed %d. Halting\n", NUMRANGES);
					//while(1);
				}
			}

			numcorrect = 0;

			//	  errcnt --;
			//	  if(errcnt > 0)
			//            printf("Memory error at %x val=%x\n", testptr - 1, *(testptr - 1));
		} else {
			// Match
			// We need NUMRANGES passes before we confirm a range_starts
			numcorrect++;
			if (numcorrect >= 1)
				lastcorrect = testptr - 1;
			if (numcorrect == NUMCORRECT)
				range_starts[range_index] = (int) lastcorrect;
		}
	}

	if (numcorrect > maxgood) maxgood = numcorrect;

	if (range_starts[range_index] && (*(testptr-1) == TESTPATTERN))
	{
		range_ends[range_index] = (int) testptr;
		range_index++;
	}

	printf("Total number of corrupted words: %d (0x%x bytes)\n", badcount, badcount*4);
	if (badcount == (length / 4)) printf("Oh, and yes... that's all the memory words in the test range.\n");
	printf("Largest preserved chunk was    : 0x%x (%d) bytes\n", (maxgood+1)*4, (maxgood+1)*4);
	printf("Uncorrupted ranges are (%d):\n", range_index);

	for (i = 0; i < NUMRANGES; i++) {
		if((range_ends[i] - range_starts[i]) > NUMTHRESHOLD)
			printf("0x%x - 0x%x (%d M)\n", range_starts[i], range_ends[i],
					(range_ends[i] - range_starts[i])/(1024*1024));
	}

	while(1);
}

int strcmp(const char *cs, const char *ct)
{
	signed char __res;

	while (1) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}
	return __res;
}

int memcmp_helper(const void *cs, const void *ct, size_t count, int showerrs)
{
	const unsigned char *su1, *su2;
	int crap;
	crap = 0;
	int res = 0;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
		{
			crap ++;
			if(showerrs-- > 0)
				printf("Mismatch(%d) at: %x  orig %x != %x\n", showerrs, su2, *su1, *su2);
		}
	return crap;
}

void *memcpy(void *dest, const void *src, size_t count)
{
	char *tmp = dest;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}

void *memset(void *s, int c, size_t n)
{
    char *tmp = s;

    while (n--)
        *tmp++ = (char)c;

    return s;
}

#define MALLOC_SIZE (65536 + 10240 + 4096)
char malloc_buf[MALLOC_SIZE];
unsigned malloc_start = (unsigned) malloc_buf;

void* malloc(size_t size)
{
    printf("starting to malloc %d bytes\n", size);
	void* ret;
	if(((unsigned int)malloc_start + size) <= ((unsigned int)malloc_buf + MALLOC_SIZE))
	{
		ret = (void*) malloc_start;
		malloc_start += size;
	} else {
		printf("Bootjacker Malloc: out of memory\n");
		while(1);
	}
    printf("malloc has %d bytes left\n", ((unsigned)malloc_buf + MALLOC_SIZE) - malloc_start);

	return ret;
}

void free(void* p)
{
	printf("Bootjacker free: %x\n", (unsigned int)p);
}

int get_next_sector(void)
{
	int ret, total = 0;
	//int* current_drive = (void*)kk_current_drive;
	char dummy[BYTES_AT_A_TIME];
	static int last_sector = 0, cur_sector = 0;

	auto void disk_read_blocklist_func (int sector, int offset, int length);

	auto void disk_read_blocklist_func (int sector, int offset, int length)
	{ cur_sector = sector; }

	*s_disk_read_hook = (int*) disk_read_blocklist_func;
	while((ret = s_grub_read(dummy, BYTES_AT_A_TIME)) && (cur_sector == last_sector))
	{
		dummy[0] = 0;
		total += BYTES_AT_A_TIME;	
#if 0
		if(i++ % 10000 == 0)
			printf("Read %d bytes, %d total\n dummy=%s", BYTES_AT_A_TIME, total, dummy);
#endif
	}

	last_sector = cur_sector;

	if(!ret)
		return ret;

	return cur_sector;
}

void do_fatdump(void)
{
	//int* fsys_type = (void*)kk_fsys_type;
	//int* filepos = (void*)kk_filepos;
	//int* filemax = (void*)kk_filemax;
	//int* buf_drive = (void*)kk_buf_drive;
	int* s_current_drive = (void*)kk_current_drive;
	void *nextsector = (void*) CMP_OFFSET; //start at start of extended memory
	int ret = 0;
	unsigned int writecnt = 0;

	printf("Dumping memory image to FAT drive\n");
	
	s_print_fsys_type();

	if(!s_grub_open(MEMDUMP_FILE))
	{
		printf("Cannot open memory dump file %s... Did you set the root drive in Grub?\n", MEMDUMP_FILE);
	} else {
		while((ret = get_next_sector())	 &&
			(unsigned int)nextsector < (unsigned int)MEM_LIMIT)	
		{
			rawwrite(*s_current_drive, ret, nextsector);
			if(writecnt++ % 2048 == 0)
				printf("%d M..", writecnt/2048);
			
			//printf("File next sector is %d size=%d\n", ret, size += 512);
			nextsector = (void *)((int)nextsector + 512);
		}
	}
	printf("\n\n\n***********<end>**************\n\n\n");
	*s_disk_read_hook = 0;
	while(1);
}

#if QLZ_STREAMING_BUFFER == 0
    #error Define some streaming buffer space please
#endif
void do_memcompressdump(void)
{
    char *memtarget = (char *)MEM_START;
    char *memlimit = (char *)MEM_LIMIT;
    //char *memlimit = (char *)0x00437000; //quick md5 test
    char localbuf[QLZ_LOCAL_BUFFER_SIZE];
    char temp_copy_buf[QLZ_LOCAL_BUFFER_SIZE];
    char sectorbuf[QLZ_SECTOR_SIZE];
    int dumpsector;
    int comp_out;
    int local_bytes = 0;
    int sector_bytes = 0;
    int copy_bytes;

    char firstbytes[16];
    char lastbytes[16];

    char output_char_buf[17];

    //stats
    int total_out = 0;
    int total_in = 0;

    unsigned int mb = 0;
    unsigned int start_time = get_time();
    unsigned int end_time =  start_time;
    unsigned int writecnt = 0;

    //MD5//
    MD5Context md5ctx;
    md5_init(&md5ctx);
    
    // write to file vars 
    int* s_current_drive = (void*)kk_current_drive;

    //set up QLZ's scratch space. must be cleared.
    char *qlz_scratch;
    qlz_scratch = (char *)malloc(QLZ_SCRATCH_COMPRESS);
    memset(qlz_scratch, 0, QLZ_SCRATCH_COMPRESS);

    printf("using %d bytes of scratch space\n", QLZ_SCRATCH_COMPRESS);

    if(!s_grub_open(MEMDUMP_FILE))
	{
		printf("Cannot open memory dump file %s... Did you set the root drive in Grub?\n", MEMDUMP_FILE);
        while(1);
	}

    if(!(dumpsector = get_next_sector()))
    {
        printf("reached end of file.\n");
        while(1);
    }

    while((unsigned)memtarget < (unsigned)memlimit)
    {

        //printf("starting to compress at %x\n", memtarget);
       
        //MD5//
        //do md5 checksum on pre-compress input
        //will this take more memory than we have? probably.
        md5_update(&md5ctx, memtarget, QLZ_PACKET_SIZE);


        comp_out = qlz_compress(memtarget, localbuf+local_bytes, QLZ_PACKET_SIZE, qlz_scratch);
        memtarget += QLZ_PACKET_SIZE;
        total_in += QLZ_PACKET_SIZE;

        local_bytes += comp_out;
        
        if(total_out == 0)
        {
            memcpy(firstbytes, localbuf, 16);
        }
        
        while(local_bytes)
        {
            if(local_bytes <= QLZ_SECTOR_SIZE - sector_bytes)
            {
                copy_bytes = local_bytes;
            }
            else
            {
                copy_bytes = QLZ_SECTOR_SIZE - sector_bytes;
            }
            memcpy(sectorbuf+sector_bytes, localbuf, copy_bytes);
            sector_bytes += copy_bytes;
            if(sector_bytes == QLZ_SECTOR_SIZE)
            {
                //push to disk
                rawwrite(*s_current_drive, dumpsector, sectorbuf);
		writecnt++;
		if(writecnt % 2048 == 0)
		{
			end_time = get_time();
			printf("Compressed dumped to disk sector time:%d mb:%d\n", end_time - start_time, ++mb);
		}
                sector_bytes = 0;
                //printf("compressed data written to sector %d\n", dumpsector);
                total_out += QLZ_SECTOR_SIZE;
                if(!(dumpsector = get_next_sector()))
                {
                    printf("reached end of file. try a bigger file next time\n");
                    while(1);
                }
            }
            if(local_bytes > copy_bytes)
            {
                //shift and reset localbuf
                //dont have memmove, plus it uses memory we don't want to use
                //memmove(localbuf, localbuf+copy_bytes, local_bytes - copy_bytes);
                memcpy(temp_copy_buf, localbuf, QLZ_LOCAL_BUFFER_SIZE);
                memcpy(localbuf, temp_copy_buf+copy_bytes, local_bytes-copy_bytes);

            }
            local_bytes -= copy_bytes; //we can safely reset local_bytes after fixing the localbuf
        }

    }
    //finish buffer
    if(sector_bytes)
    {
            //push to disk
            rawwrite(*s_current_drive, dumpsector, sectorbuf);
	end_time = get_time();
	printf("Compressed dumped to disk sector time:%d mb:%d\n", end_time - start_time, ++mb);
            sector_bytes = 0;
            printf("last (possibly partial) sector of compressed data written to sector %d\n", dumpsector);
            memcpy(lastbytes, sectorbuf, 16);
    }

    printf("total bytes read in:%d, total compressed out:%d\n", total_in, total_out);
    firstbytes[15] = 0;
    lastbytes[15] = 0;
    printf("first bytes compressed:%s, last sector bytes:%s\n", firstbytes, lastbytes);


    //MD5//
    md5_final(&md5ctx, output_char_buf);
    output_char_buf[16] = 0;

    printf("md5 hash: %x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x\n", (unsigned char)output_char_buf[0], (unsigned char)output_char_buf[1], (unsigned char)output_char_buf[2], (unsigned char)output_char_buf[3],
                                                           (unsigned char)output_char_buf[4], (unsigned char)output_char_buf[5], (unsigned char)output_char_buf[6], (unsigned char)output_char_buf[7],
                                                           (unsigned char)output_char_buf[8], (unsigned char)output_char_buf[9], (unsigned char)output_char_buf[10], (unsigned char)output_char_buf[11],
                                                           (unsigned char)output_char_buf[12], (unsigned char)output_char_buf[13], (unsigned char)output_char_buf[14], (unsigned char)output_char_buf[15]
                                                           );

	*s_disk_read_hook = 0;

    while(1); //end
}

char *
convert_to_ascii (char *buf, int c,...)
{
  unsigned long num = *((&c) + 1), mult = 10;
  char *ptr = buf;

#ifndef STAGE1_5
  if (c == 'x' || c == 'X')
    mult = 16;

  if ((num & 0x80000000uL) && c == 'd')
    {
      num = (~num) + 1;
      *(ptr++) = '-';
      buf++;
    }
#endif

  do
    {
      int dig = num % mult;
      *(ptr++) = ((dig > 9) ? dig + 'a' - 10 : '0' + dig);
    }
  while (num /= mult);

  /* reorder to correct direction!! */
  {
    char *ptr1 = ptr - 1;
    char *ptr2 = buf;
    while (ptr1 > ptr2)
      {
	int tmp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = tmp;
	ptr1--;
	ptr2++;
      }
  }

  return ptr;
}

int
sprintf (char *buffer, const char *format, ...)
{
  /* XXX hohmuth
     ugly hack -- should unify with printf() */
  int *dataptr = (int *) &format;
  char c, *ptr, str[16];
  char *bp = buffer;

  dataptr++;

  while ((c = *format++) != 0)
    {
      if (c != '%')
	*bp++ = c; /* putchar(c); */
      else
	switch (c = *(format++))
	  {
	  case 'd': case 'u': case 'x':
	    *convert_to_ascii (str, c, *((unsigned long *) dataptr++)) = 0;

	    ptr = str;

	    while (*ptr)
	      *bp++ = *(ptr++); /* putchar(*(ptr++)); */
	    break;

	  case 'c': *bp++ = (*(dataptr++))&0xff;
	    /* putchar((*(dataptr++))&0xff); */
	    break;

	  case 's':
	    ptr = (char *) (*(dataptr++));

	    while ((c = *ptr++) != 0)
	      *bp++ = c; /* putchar(c); */
	    break;
	  }
    }

  *bp = 0;
  return bp - buffer;
}

extern unsigned syscall_start_addr, syscall_end_addr;
void do_syscall(void)
{
  unsigned int syscall_cnt = kk_syscall_table_size/4;
  unsigned int* syscall_table = kk_sys_call_table - LINUX_PAGE_OFFSET;
  unsigned int* real_syscalltable = (unsigned*) syscall_start_addr;
  unsigned int mismatches = 0;
  printf("Checking syscall table at(%d):%x real_table=%x\n", syscall_cnt, (int)syscall_table, (int)real_syscalltable);

//  printf("%x %x %x %x\n", syscall_table[0], syscall_table[1], syscall_table[2], syscall_table[3]);
//  printf("%x %x %x %x\n", real_syscalltable[0], real_syscalltable[1], real_syscalltable[2], real_syscalltable[3]);

  do {
    if(syscall_table[syscall_cnt] != real_syscalltable[syscall_cnt])
    {
      printf("Syscall %d %x != real %x %s\n", syscall_cnt, syscall_table[syscall_cnt], real_syscalltable[syscall_cnt], syscall_table[syscall_cnt] > LAST_ADDR ? "[!T]" : "[T]");
      mismatches++;
    }
  } while(syscall_cnt-- > 0);

  printf("Done checking syscall table, %d mismatches\n", mismatches);

  while(1);
}

void do_syscallwrite(void)
{
  unsigned int syscall_cnt = kk_syscall_table_size/4;
  unsigned int* syscall_table = kk_sys_call_table - LINUX_PAGE_OFFSET;
  unsigned int* real_syscalltable = (unsigned*) syscall_start_addr;
  printf("Checking syscall table at(%d):%x real_table=%x\n", syscall_cnt, (int)syscall_table, (int)real_syscalltable);
  if(real_syscalltable == 0)
  {
     printf("real_syscalltable == 0 ... halting\n");
  }

  do {
    syscall_table[syscall_cnt] = real_syscalltable[syscall_cnt];
  } while(syscall_cnt-- > 0);

  printf("Done writing syscall table, %d bytes\n", kk_syscall_table_size);
}

