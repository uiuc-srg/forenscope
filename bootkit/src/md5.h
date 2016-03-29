/**
 *  Quick header for md5.c
 */

#ifndef _BOOTJACKER_MD5_H
#define _BOOTJACKER_MD5_H

/*
** typedefs for convenience
*/
typedef unsigned long mULONG;
typedef unsigned char mUCHAR;

typedef struct {
        mULONG hash[4];
        mULONG bits[2];
        mUCHAR data[64];
} MD5Context;


void md5_init( MD5Context *ctx );
void md5_update( MD5Context *ctx, const mUCHAR *buf, mULONG buflen );
void md5_final( MD5Context *ctx, mUCHAR digest[ 16 ] );


#endif /* _BOOTJACKER_MD5_H */

