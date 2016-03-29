/**
 *  cmostime.c
 *  get system time from CMOS
 */

#include "helper.h"
#include "cmostime.h"

int read_cmos_time(int field)
{
	int ones, tens;
	int onesmask = 0x0000000f;
	int tensmask = 0x000000f0;
	int rawvalue = get_cmos_time(field);

	ones = rawvalue & onesmask;
	tens = ((rawvalue & tensmask) >> 4) * 10;

	return (tens + ones);
}

