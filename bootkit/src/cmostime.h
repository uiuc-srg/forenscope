/**
 *  cmostime.h
 *  get system time from CMOS
 */

#define CMOS_TIME_INPORT 	0x70
#define CMOS_TIME_OUTPORT 	0x71

#define CMOS_DISABLE_INT 	0x80
#define CMOS_SECONDS		0x00
#define CMOS_MINUTES		0x02
#define CMOS_HOURS			0x04
#define CMOS_DATE			0x07
#define CMOS_MONTH			0x08
#define CMOS_YEAR			0x09

