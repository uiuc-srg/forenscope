/*********************************************************************\

MODULE NAME:    md5.c

AUTHOR:         Bob Trower 03/11/05

PROJECT:        Crypt Data Packaging

COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001, 2005

NOTES:          This is free software. This source code may be used as 
                you wish, subject to the LGPL license.  See the
                LICENCE section below.

                Canonical source should be at:
                    http://toogles.sourceforge.net

                This is a big header for a little utility, but it is
                designed to be self-contained, so this is pretty much
                the complete package in one file.


DESCRIPTION:
                This little utility implements the md5
                Message-Digest Algorithm described in RFC1321
                (http://www.faqs.org/rfcs/rfc1321.html).


DESIGN GOALS:   Specifically:
                Code is a stand-alone utility to perform md5 hashing.
                It should be genuinely useful when the need arises and
                it meets a need that is likely to occur for some
                users. Code acts as sample code to show the author's
                design and coding style.

                Generally:
                This program is designed to survive: Everything you
                need is in a single source file. It compiles cleanly
                using a vanilla ANSI C compiler. It does its job
                correctly with a minimum of fuss. The code is not
                overly clever, not overly simplistic and not overly
                verbose. Access is 'cut and paste' from a web page.
                Terms of use are reasonable.

VALIDATION:     Non-trivial code is never without errors.  This file
                likely has some problems, since it has only been
                tested by the author.  It is expected with most source
                code that there is a period of 'burn-in' when problems
                are identified and corrected.  That being said, it is
                possible to have 'reasonably correct' code by
                following a regime of unit test that covers the most
                likely cases and regression testing prior to release.
                This has been done with this code and it has a good
                probability of performing as expected.

                Unit Test Cases:

                Note that this program has a 'self test' function
                that tests the inputs given in the RFC document.

                The following are additional tests on the program
                itself and using test vectors from files.

                case 0:empty file:
                    CASE0.DAT
                    Hash Verified, errorlevel is: 0

                case 1:One input character (a):
                    CASE1.DAT
                    Hash Verified, errorlevel is: 0

                case 2:Three input characters (abc):
                    CASE2.DAT
                    Hash Verified, errorlevel is: 0

                case 3:'message digest':
                    CASE3.DAT
                    Hash Verified, errorlevel is: 0

                case 4:The alphabet:
                    CASE4.DAT
                    Hash Verified, errorlevel is: 0

                case 5:AlphaNumeric Characters:
                    CASE5.DAT
                    Hash Verified, errorlevel is: 0

                case 6:Lots of numbers:
                    CASE6.DAT
                    Hash Verified, errorlevel is: 0

                case 7: Large files:
                    Tested 67 MB file (OOo_1.1.4_Win32Intel_install.zip).
                    Correct hash as tested against another md5 program.

                case 8: Very Large Files:
                    Tested ISO images for Mandrake Linux
                    Correct hash.
                    Approximately 25-40MB/sec on 1GHz machine.
                    Times were the same to copy the files to the nul
                    device, so it is likely that performance is simply
                    I/O bound for this application.

                case 9 Stress:
                    All files in a working directory hashed. Spot
                    checked to ensure correct hashes and ensure
                    that multiple runs do not cause problems
                    such as exhausting file handles, tmp storage, etc.

                -------------

                Syntax, operation and failure:
                    All options/switches tested.  Performs as
                    expected.

                case 10:
                    No Args -- Shows Usage Screen
                    md5
                    Return Code 1 (Invalid Syntax)

                case 11:
                    One Arg -- Defaults to hashing as string
                    md5 "message digest"
                    Return Code 0 (Success)
                    Hash is correct.

                case 12:
                    One Arg Help (-?) -- Shows detailed Usage Screen.
                    Return Code 0 (Success -- help request is valid).

                case 13:
                    One Arg Help (-h) -- Shows detailed Usage Screen.
                    Return Code 0 (Success -- help request is valid).

                case 14:
                    One Arg -'f' (valid) -- Uses stdin/stdout (filter)
                    md5 -f < CASE3.DAT
                    Return Code 0 (Success)
                    CASE3.DAT used -- hash matches correctly.

                case 15:
                    Two Args (invalid filename) -- shows system error.
                    md5 -f :Is:Not:A\Valid/FileName
                    Return Code 2 (File Error)

                case 16:
                    hash non-existent file -- shows system error.
                    md5 -f NotLikelyAFileName.xyz
                    Return Code 2 (File Error)

                case 17:
                    Invalid disk -- shows system error.
                    md5 -f y:NoDiskHere
                    Return Code 2 (File Error)

                case 18:
                    Too many args -- shows system error.
                    md5 x y
                    Return Code 1 (Invalid Syntax)

                case 19:
                    Missing Args -- shows system error.
                    md5 -s
                    md5:004:Missing input -- nothing to hash.
                    Return Code 4 (Missing input)

                case 20:
                    Verify Option Missing Args -- shows error.
                    md5 -v FileNameButNoHash
                    md5:004:Missing input -- nothing to hash.
                    Return Code 4 (Missing input)

                case 21:
                    Verify Option Too Many Args -- shows error.
                    md5 -v x y z
                    md5:006:Syntax: Too many arguments.
                    Return Code 1 (Invalid Syntax)

                case 22:
                    Verify -- Syntax Ok, verify fails.
                    md5 -v CASE3.DAT bogushashvalue
                    bogushashvalue << Expected Hash Value
                    f96b697d7cb7938d525a2f31aaf161d0 << Actual Hash Value
                    Hash not verified. No match.
                    md5:007:Test Failure.

                    Return Code 7 (Test Failure)

                case 23:
                    Verify -- Verify passes.
                    md5 -v CASE3.DAT f96b697d7cb7938d525a2f31aaf161d0
                    f96b697d7cb7938d525a2f31aaf161d0 << Expected Hash Value
                    f96b697d7cb7938d525a2f31aaf161d0 << Actual Hash Value
                    Hash Verified

                    Return Code 0 (Success)

                case 24:
                    Self Test
                    md5 -t
                    d41d8cd98f00b204e9800998ecf8427e << Expected Hash Value
                    d41d8cd98f00b204e9800998ecf8427e << Actual Hash Value
                    0cc175b9c0f1b6a831c399e269772661 << Expected Hash Value
                    0cc175b9c0f1b6a831c399e269772661 << Actual Hash Value
                    900150983cd24fb0d6963f7d28e17f72 << Expected Hash Value
                    900150983cd24fb0d6963f7d28e17f72 << Actual Hash Value
                    f96b697d7cb7938d525a2f31aaf161d0 << Expected Hash Value
                    f96b697d7cb7938d525a2f31aaf161d0 << Actual Hash Value
                    c3fcd3d76192e4007dfb496cca67e13b << Expected Hash Value
                    c3fcd3d76192e4007dfb496cca67e13b << Actual Hash Value
                    d174ab98d277d9f5a5611c2c9f419d9f << Expected Hash Value
                    d174ab98d277d9f5a5611c2c9f419d9f << Actual Hash Value
                    57edf4a22be3c955ac49da2e2107b67a << Expected Hash Value
                    57edf4a22be3c955ac49da2e2107b67a << Actual Hash Value
                    Writing CASE0.DAT
                    d41d8cd98f00b204e9800998ecf8427e << Expected Hash Value
                    d41d8cd98f00b204e9800998ecf8427e << Actual Hash Value
                    Writing CASE1.DAT
                    0cc175b9c0f1b6a831c399e269772661 << Expected Hash Value
                    0cc175b9c0f1b6a831c399e269772661 << Actual Hash Value
                    Writing CASE2.DAT
                    900150983cd24fb0d6963f7d28e17f72 << Expected Hash Value
                    900150983cd24fb0d6963f7d28e17f72 << Actual Hash Value
                    Writing CASE3.DAT
                    f96b697d7cb7938d525a2f31aaf161d0 << Expected Hash Value
                    f96b697d7cb7938d525a2f31aaf161d0 << Actual Hash Value
                    Writing CASE4.DAT
                    c3fcd3d76192e4007dfb496cca67e13b << Expected Hash Value
                    c3fcd3d76192e4007dfb496cca67e13b << Actual Hash Value
                    Writing CASE5.DAT
                    d174ab98d277d9f5a5611c2c9f419d9f << Expected Hash Value
                    d174ab98d277d9f5a5611c2c9f419d9f << Actual Hash Value
                    Writing CASE6.DAT
                    57edf4a22be3c955ac49da2e2107b67a << Expected Hash Value
                    57edf4a22be3c955ac49da2e2107b67a << Actual Hash Value
                    Writing CASE7.DAT
                    Writing CASE8.DAT
                    Self Test Passed

                    Return Code 0 (Success)

                case 25:
                    Test convert test hash to lowercase for comparison.
                    md5 -v CASE3.DAT F96B697D7CB7938D525A2F31AAF161D0
                    f96b697d7cb7938d525a2f31aaf161d0 << Expected Hash Value
                    f96b697d7cb7938d525a2f31aaf161d0 << Actual Hash Value
                    Hash Verified

                    Return Code 0 (Success)

                -------------

                Compile/Regression test:
                    gcc compiled binary under Cygwin:
                        gcc md5.c -omd5.exe
                    Microsoft Visual Studio under Windows 2000
                        cl md5.c
                    Microsoft Version 6.0 C under Windows 2000
                        cl md5.c

DEPENDENCIES:   None

LICENCE:        NOTE: This license should be liberal enough for most
                purposes while still offering some protection to the
                code. If you find the license a problem, get in touch
                through http://www.trantor.ca and we will see what we
                can do. In particular, those who have been kind enough
                to make the original sources and other materials
                available can expect to use the code with more liberal
                permissions.

                Copyright (c) 2005 Bob Trower and Trantor Standard
                Systems Inc.

                This library is free software; you can redistribute it
                and/or modify it under the terms of the GNU Lesser
                General Public License as published by the Free
                Software Foundation; either version 2.1 of the
                License, or (at your option) any later version.

                This library is distributed in the hope that it will
                be useful, but WITHOUT ANY WARRANTY; without even the
                implied warranty of MERCHANTABILITY or FITNESS FOR A
                PARTICULAR PURPOSE.  See the GNU Lesser General Public
                License for more details.

                You should have received a copy of the GNU Lesser
                General Public License along with this library; if
                not, write to the Free Software Foundation, Inc., 59
                Temple Place, Suite 330, Boston, MA  02111-1307  USA

CREDITS:        Algorithm and (I think) sample code for the RFC was
                done by Ron Rivest.

                Colin Plumb wrote a public domain version in 1993.
                Some of this code derives from code that derives from
                that.

                From a header in one of the sources used to create
                this file:

                parts of this file are :

                Written March 1993 by Branko Lankester Modified June
                1993 by Colin Plumb for altered md5.c. Modified
                October 1995 by Erik Troan for RPM

                Although not used, code supplied by Langfine Ltd. was
                consulted and used for some of the validation.

                Although it was not used, code by John Walker was
                consulted. John is a source of inspiration. His
                actions speak for themselves. You can find lots of
                other neat stuff at his website:
                http://www.fourmilab.ch/

VERSION HISTORY:
                Bob Trower 03/11/05 -- Create Version 0.00.00B

\******************************************************************* */

/** made new header file md5.h (not downloaded) */
#include "md5.h"
#include "helper.h"

/* Basic MD5 functions */

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) (y ^ (z & (x ^ y)))
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define TRANSFORM(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
**
** md5_transform
**
** The MD5 "basic transformation". Updates the hash
** based on the data block passed.
**
*/
static void md5_transform( mULONG hash[ 4 ], const mULONG data[ 16 ] )
{
    mULONG a = hash[0], b = hash[1], c = hash[2], d = hash[3];

    /* Round 1 */
    TRANSFORM( F1, a, b, c, d, data[ 0] + 0xd76aa478,  7);
    TRANSFORM( F1, d, a, b, c, data[ 1] + 0xe8c7b756, 12);
    TRANSFORM( F1, c, d, a, b, data[ 2] + 0x242070db, 17);
    TRANSFORM( F1, b, c, d, a, data[ 3] + 0xc1bdceee, 22);
    TRANSFORM( F1, a, b, c, d, data[ 4] + 0xf57c0faf,  7);
    TRANSFORM( F1, d, a, b, c, data[ 5] + 0x4787c62a, 12);
    TRANSFORM( F1, c, d, a, b, data[ 6] + 0xa8304613, 17);
    TRANSFORM( F1, b, c, d, a, data[ 7] + 0xfd469501, 22);
    TRANSFORM( F1, a, b, c, d, data[ 8] + 0x698098d8,  7);
    TRANSFORM( F1, d, a, b, c, data[ 9] + 0x8b44f7af, 12);
    TRANSFORM( F1, c, d, a, b, data[10] + 0xffff5bb1, 17);
    TRANSFORM( F1, b, c, d, a, data[11] + 0x895cd7be, 22);
    TRANSFORM( F1, a, b, c, d, data[12] + 0x6b901122,  7);
    TRANSFORM( F1, d, a, b, c, data[13] + 0xfd987193, 12);
    TRANSFORM( F1, c, d, a, b, data[14] + 0xa679438e, 17);
    TRANSFORM( F1, b, c, d, a, data[15] + 0x49b40821, 22);

    /* Round 2 */
    TRANSFORM( F2, a, b, c, d, data[ 1] + 0xf61e2562,  5);
    TRANSFORM( F2, d, a, b, c, data[ 6] + 0xc040b340,  9);
    TRANSFORM( F2, c, d, a, b, data[11] + 0x265e5a51, 14);
    TRANSFORM( F2, b, c, d, a, data[ 0] + 0xe9b6c7aa, 20);
    TRANSFORM( F2, a, b, c, d, data[ 5] + 0xd62f105d,  5);
    TRANSFORM( F2, d, a, b, c, data[10] + 0x02441453,  9);
    TRANSFORM( F2, c, d, a, b, data[15] + 0xd8a1e681, 14);
    TRANSFORM( F2, b, c, d, a, data[ 4] + 0xe7d3fbc8, 20);
    TRANSFORM( F2, a, b, c, d, data[ 9] + 0x21e1cde6,  5);
    TRANSFORM( F2, d, a, b, c, data[14] + 0xc33707d6,  9);
    TRANSFORM( F2, c, d, a, b, data[ 3] + 0xf4d50d87, 14);
    TRANSFORM( F2, b, c, d, a, data[ 8] + 0x455a14ed, 20);
    TRANSFORM( F2, a, b, c, d, data[13] + 0xa9e3e905,  5);
    TRANSFORM( F2, d, a, b, c, data[ 2] + 0xfcefa3f8,  9);
    TRANSFORM( F2, c, d, a, b, data[ 7] + 0x676f02d9, 14);
    TRANSFORM( F2, b, c, d, a, data[12] + 0x8d2a4c8a, 20);

    /* Round 3 */
    TRANSFORM( F3, a, b, c, d, data[ 5] + 0xfffa3942,  4);
    TRANSFORM( F3, d, a, b, c, data[ 8] + 0x8771f681, 11);
    TRANSFORM( F3, c, d, a, b, data[11] + 0x6d9d6122, 16);
    TRANSFORM( F3, b, c, d, a, data[14] + 0xfde5380c, 23);
    TRANSFORM( F3, a, b, c, d, data[ 1] + 0xa4beea44,  4);
    TRANSFORM( F3, d, a, b, c, data[ 4] + 0x4bdecfa9, 11);
    TRANSFORM( F3, c, d, a, b, data[ 7] + 0xf6bb4b60, 16);
    TRANSFORM( F3, b, c, d, a, data[10] + 0xbebfbc70, 23);
    TRANSFORM( F3, a, b, c, d, data[13] + 0x289b7ec6,  4);
    TRANSFORM( F3, d, a, b, c, data[ 0] + 0xeaa127fa, 11);
    TRANSFORM( F3, c, d, a, b, data[ 3] + 0xd4ef3085, 16);
    TRANSFORM( F3, b, c, d, a, data[ 6] + 0x04881d05, 23);
    TRANSFORM( F3, a, b, c, d, data[ 9] + 0xd9d4d039,  4);
    TRANSFORM( F3, d, a, b, c, data[12] + 0xe6db99e5, 11);
    TRANSFORM( F3, c, d, a, b, data[15] + 0x1fa27cf8, 16);
    TRANSFORM( F3, b, c, d, a, data[ 2] + 0xc4ac5665, 23);

    /* Round 4 */
    TRANSFORM( F4, a, b, c, d, data[ 0] + 0xf4292244,  6);
    TRANSFORM( F4, d, a, b, c, data[ 7] + 0x432aff97, 10);
    TRANSFORM( F4, c, d, a, b, data[14] + 0xab9423a7, 15);
    TRANSFORM( F4, b, c, d, a, data[ 5] + 0xfc93a039, 21);
    TRANSFORM( F4, a, b, c, d, data[12] + 0x655b59c3,  6);
    TRANSFORM( F4, d, a, b, c, data[ 3] + 0x8f0ccc92, 10);
    TRANSFORM( F4, c, d, a, b, data[10] + 0xffeff47d, 15);
    TRANSFORM( F4, b, c, d, a, data[ 1] + 0x85845dd1, 21);
    TRANSFORM( F4, a, b, c, d, data[ 8] + 0x6fa87e4f,  6);
    TRANSFORM( F4, d, a, b, c, data[15] + 0xfe2ce6e0, 10);
    TRANSFORM( F4, c, d, a, b, data[ 6] + 0xa3014314, 15);
    TRANSFORM( F4, b, c, d, a, data[13] + 0x4e0811a1, 21);
    TRANSFORM( F4, a, b, c, d, data[ 4] + 0xf7537e82,  6);
    TRANSFORM( F4, d, a, b, c, data[11] + 0xbd3af235, 10);
    TRANSFORM( F4, c, d, a, b, data[ 2] + 0x2ad7d2bb, 15);
    TRANSFORM( F4, b, c, d, a, data[ 9] + 0xeb86d391, 21);

    hash[ 0 ] += a;
    hash[ 1 ] += b;
    hash[ 2 ] += c;
    hash[ 3 ] += d;
}

/*
** md5_init
**
** Initialise md5 context structure
**
*/
void md5_init( MD5Context *ctx )
{
    ctx->hash[ 0 ] = 0x67452301;
    ctx->hash[ 1 ] = 0xefcdab89;
    ctx->hash[ 2 ] = 0x98badcfe;
    ctx->hash[ 3 ] = 0x10325476;

    ctx->bits[ 0 ] = 0;
    ctx->bits[ 1 ] = 0;
}

/*
** md5_update
**
** Update context with the next buffer from the stream of data.
** Call with each block of data to update the md5 hash.
**
*/
void md5_update( MD5Context *ctx, const mUCHAR *buf, mULONG buflen )
{
    mULONG idx;

    /* Update bitcount */

    idx = ctx->bits[ 0 ];
    ctx->bits[ 0 ] = idx + (buflen << 3);
    if( ctx->bits[ 0 ] < idx ) {
        ctx->bits[ 1 ]++;         /* Carry from low to high */
    }
    ctx->bits[ 1 ] += buflen >> 29;

    idx = (idx >> 3) & 0x3f;    /* Bytes already in ctx->data */

    /* Handle any leading odd-sized chunks */

    if( idx != 0 ) {
        mUCHAR *p = (mUCHAR *) ctx->data + idx;

        idx = 64 - idx;
        if( buflen < idx ) {
            memcpy( p, buf, (size_t) buflen );
        }
        else {
            memcpy( p, buf, (size_t) idx );
            md5_transform( ctx->hash, (mULONG *) ctx->data );
            buf += idx;
            buflen -= idx;
        }
    }
    if( buflen >= idx ) {
        while( buflen >= 64 ) {
            memcpy( ctx->data, buf, 64 );
            md5_transform( ctx->hash, (mULONG *) ctx->data );
            buf += 64;
            buflen -= 64;
        }
        memcpy( ctx->data, buf, (size_t) buflen );
    }
}

/*
** md5_final
**
** Finalize creation of md5 hash and copy to digest buffer.
**
*/
void md5_final( MD5Context *ctx, mUCHAR digest[ 16 ] )
{
    mULONG count;
    mUCHAR *pad;

    count = (ctx->bits[ 0 ] >> 3) & 0x3F; /* Number of bytes mod 64 */
    pad = ctx->data + count;
    *pad++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if( count < 8 ) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset( pad, 0, (size_t) count );
        md5_transform( ctx->hash, (mULONG *) ctx->data );

        /* Now fill the next block with 56 bytes */
        memset( ctx->data, 0, 56 );
    } else {
        /* Pad block to 56 bytes */
        memset( pad, 0, (size_t) (count - 8) );
    }

    /* Append length in bits and transform */
    ((mULONG *) ctx->data)[ 14 ] = ctx->bits[ 0 ];
    ((mULONG *) ctx->data)[ 15 ] = ctx->bits[ 1 ];

    md5_transform( ctx->hash, (mULONG *) ctx->data );
    memcpy( digest, ctx->hash, 16 );
}

/*
** md5_digest_string
**
** Supply the digest and a buffer for the string.
** This routine will populate the buffer and
** return the value as a C string.
**
*/
/*
char *md5_digest_string( mUCHAR d[ 16 ], char digest_string[ 33 ] )
{
    int i;

    digest_string[ 32 ] = 0;
    for( i = 0; i < 16; i++ ) {
        sprintf( &(digest_string[ i * 2 ]), "%2.2x", d[ i ] );
    }

    return( digest_string );
}
*/

/*
** returnable errors
**
** Error codes returned to the caller
** and to the operating system.
**
*/
#define MD5_SYNTAX_ERROR        1
#define MD5_FILE_ERROR          2
#define MD5_FILE_IO_ERROR       3
#define MD5_MISSING_INPUT       4
#define MD5_ERROR_OUT_CLOSE     5
#define MD5_SYNTAX_TOOMANYARGS  6
#define MD5_TEST_FAILURE        7

/*
** md5_message
**
** Gather text messages in one place.
**
*/
char *md5_message( int errcode )
{
    #define MD5_MAX_MESSAGES 8
    char *msgs[ MD5_MAX_MESSAGES ] = {
            "md5:000:Invalid Message Code.",
            "md5:001:Syntax Error -- check help for usage.",
            "md5:002:File Error Opening/Creating Files.",
            "md5:003:File I/O Error -- Note: file cleanup not done.",
            "md5:004:Missing input -- nothing to hash.",
            "md5:005:Error on output file close.",
            "md5:006:Syntax: Too many arguments.",
            "md5:007:Test Failure."
    };
    char *msg = msgs[ 0 ];

    if( errcode > 0 && errcode < MD5_MAX_MESSAGES ) {
        msg = msgs[ errcode ];
    }

    return( msg );
}

/*
**
** md5_file
**
** Compute hash on an open file handle.
**
*/
#define MD5_BUFSIZE 4096
/*
int md5_file( FILE *infile, mUCHAR digest[ 16 ] )
{
    int retcode;
    MD5Context ctx;
    mUCHAR buf[ MD5_BUFSIZE ];
    mULONG bytes_read;

    md5_init( &ctx );

    while( (bytes_read = fread (buf, sizeof (mUCHAR), MD5_BUFSIZE, infile)) > 0 ) {
        md5_update( &ctx, buf, bytes_read );
    }
    if( ferror( infile ) ) {
        retcode = MD5_FILE_IO_ERROR;
    }
    else {
        md5_final( &ctx, digest );
        retcode = 0;
    }

    return( retcode );
}
*/
/*
**
** md5_filename
**
** Compute the hash on a file.
**
*/
/*
int md5_filename( const char *infilename, mUCHAR digest[ 16 ] )
{
    FILE *infile;
    int retcode = MD5_FILE_ERROR;

    if( !infilename ) {
        infile = stdin;
    }
    else {
        infile = fopen( infilename, "rb" );
    }
    if( !infile ) {
        printf( "Error: FileName='%s' -- %s\n", infilename, strerror( errno ) );
    }
    else {
        retcode = md5_file( infile, digest );
        if( infile != stdin ) {
            if( fclose( infile ) != 0 ) {
                char *ErrM = md5_message( MD5_ERROR_OUT_CLOSE );
                printf( "Error: %s -- %s\n", ErrM, strerror( errno ) );
                retcode = MD5_FILE_IO_ERROR;
            }
        }
    }

    return( retcode );
}
*/
/*
**
** md5_verify_filename
**
** Verify that a given file has a given hash.
**
*/
/*
int md5_verify_filename( char *infilename, char *hash )
{
    int retcode = 0;
    mUCHAR digest[ 16 ];
    char digest_string[ 33 ];
    char *hd;

    md5_filename( infilename, digest );
    hd = md5_digest_string( digest, digest_string );
    printf( "%s << Expected Hash Value\n%s << Actual Hash Value\n", hash, hd );
    if( strcmp( hash, hd ) ){
        retcode = MD5_TEST_FAILURE;
    }

    return( retcode );
}
*/
/*
**
** md5_buffer
**
** Compute the md5 hash on a buffer.
**
*/
void md5_buffer( const mUCHAR *buf, int buflen, mUCHAR digest[ 16 ] )
{
    MD5Context ctx;

    md5_init( &ctx );
    md5_update( &ctx, buf, buflen );
    md5_final( &ctx, digest );
}

/*
**
** Standard test vectors and expected hashes.
** Data below is used in the self-test function.
**
** The standard test vectors are augmented by
** a couple more cases to ease regression test.
**
*/
#define NUM_VECTORS 7
#define NUM_CASES   9
char *test_vector[] = {
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890",

    "CASE7.DAT should be replaced with your own 10MB to 100MB file\n",
    "CASE8.DAT should be replaced with your own 100MB+  file\n"
};

char *test_hash[] = {
    "d41d8cd98f00b204e9800998ecf8427e",
    "0cc175b9c0f1b6a831c399e269772661",
    "900150983cd24fb0d6963f7d28e17f72",
    "f96b697d7cb7938d525a2f31aaf161d0",
    "c3fcd3d76192e4007dfb496cca67e13b",
    "d174ab98d277d9f5a5611c2c9f419d9f",
    "57edf4a22be3c955ac49da2e2107b67a",

    "04842407f40f4fc21f159bf89a7bf63e",
    "472c5207d61df9454c1e732930d1c496"
};

/*
**
** GenAndTestFileCases()
**
** Generates CASE data files for regression test
** and runs md5_verify_filename check.
**
*/
/*
int GenAndTestFileCases()
{
    int i;
    char outfilename[16];
    FILE *outfile;
    int retcode = 0;

    for( i = 0; i < NUM_CASES && retcode == 0; i++ ) {
        sprintf( outfilename, "CASE%d.DAT", i );
        printf( "Writing %s\n", outfilename );
        outfile = fopen( outfilename, "wb" );
        if( !outfile ) {
            printf( "Error: FileName='%s' -- %s\n", outfilename, strerror( errno ) );
        }
        else {
            if( fprintf( outfile, "%s", test_vector[ i ] ) < 0 ) {
                printf( "Error: FileName='%s' -- %s\n", outfilename, strerror( errno ) );
                retcode = MD5_FILE_IO_ERROR;
            }
            if( fclose( outfile ) != 0 ) {
                char *ErrM = md5_message( MD5_ERROR_OUT_CLOSE );
                printf( "Error: %s -- %s\n", ErrM, strerror( errno ) );
                retcode = MD5_FILE_IO_ERROR;
            }
            else {
                if( i < NUM_VECTORS ) {
                    retcode = md5_verify_filename( outfilename, test_hash[ i ] );
                }
            }
        }
    }

    return( retcode );
}
*/
/*
**
** md5_self_test
**
** Tests the core hashing functions against the
** test suite given in the RFC.
**
*/
/*
int md5_self_test( void )
{
    int i, retcode = 0;
    mUCHAR digest[ 16 ];
    char digest_string[ 33 ];
    char *tv;
    char *hd;

    for( i = 0; i < NUM_VECTORS && retcode == 0; i++ ) {
        tv = test_vector[ i ];
        md5_buffer( (mUCHAR *) tv, strlen( tv ), digest );
        hd = md5_digest_string( digest, digest_string );
        printf( "%s << Expected Hash Value\n%s << Actual Hash Value\n", test_hash[ i ], hd );
        if( strcmp( hd, test_hash[ i ] ) ) {
            retcode = MD5_TEST_FAILURE;
        }
    }
    if( retcode == 0 ) {
        retcode = GenAndTestFileCases();
    }

    return( retcode );
}
*/
/*
**
** LCaseHex
**
** Forces Hex string to lowercase.
**
** Make sure that we do apples to apples
** comparison of input hash to calculated hash.
**
*/
static char *LCaseHex( char *h )
{
    char *p;

    for( p = h; *p; p++ ) {
        *p = (char) (*p | 0x20);
    }

    return( h );
}

/*
** showuse
**
** display usage information, help, version info
*/
/*
void showuse( int morehelp )
{
    {
        printf( "\n" );
        printf( "  md5      (create md5 hash)           Bob Trower 03/11/05 \n" );
        printf( "           (C) Copr Bob Trower 1986-2005.    Version 0.00B \n" );
        printf( "  Usage:   md5 [-option] <Input> [testhash]\n" );
        printf( "  Purpose: This program is a simple utility that\n" );
        printf( "           implements the md5 hashing algorithm (RFC1321).\n" );
    }
    if( !morehelp ) {
        printf( "           Use -h option for additional help.\n" );
    }
    else {
        printf( "  Options: -f  Input is filename. -s  Input is string.\n" );
        printf( "           -h  This help text.    -?  This help text.\n" );
        printf( "           -t  Self Test.         -v  Verify file hash.\n" );
        printf( "  Note:    -s  Is the default. It hashes the input as a\n" );
        printf( "               string\n" );
        printf( "  Returns: 0 = success.  Non-zero is an error code.\n" );
        printf( "  ErrCode: 1 = Bad Syntax         2 = File Open error\n" );
        printf( "           3 = File I/O error     4 = Missing input\n" );
        printf( "  Example: md5 -s Some_String         <- hash that string.\n" );
        printf( "           md5 -f SomeFileName        <- hash that file.\n" );
        printf( "           md5 -t                     <- Perform Self Test.\n" );
        printf( "           md5 -v filename, testhash  <- Verify file hash.\n" );
        printf( "  Source:  Source code and latest releases can be found at:\n" );
        printf( "           http://toogles.sourceforge.net\n" );
        printf( "  Release: 0.00.00, Fri Mar 11 03:30:00 2005, ANSI-SOURCE C\n" );
    }
}
*/
#define THIS_OPT(ac, av) (ac > 1 ? av[ 1 ][ 0 ] == '-' ? av[ 1 ][ 1 ] : 0 : 0)

/*
** main
**
** parse and validate arguments and call md5 routines or help
**
*/
/*
int main( int argc, char **argv )
{
    int opt = 0;
    int retcode = 0;
    char *input = NULL;
    mUCHAR digest[ 16 ];
    char digest_string[ 33 ];

    while( THIS_OPT( argc, argv ) ) {
        switch( THIS_OPT(argc, argv) ) {
            case 'f':
                    opt = 'f';
                    break;
            case '?':
            case 'h':
                    opt = 'h';
                    break;
            case 's':
                    opt = 's';
                    break;
            case 't':
                    opt = 't';
                    break;
            case 'v':
                    opt = 'v';
                    break;
             default:
                    opt = 's';
                    break;
        }
        argv++;
        argc--;
    }
    if( opt == 'v' && argc > 3  || (opt !='v' && argc > 2) ) {
        printf( "%s\n", md5_message( MD5_SYNTAX_TOOMANYARGS ) );
        opt = 0;
    }
    else {
        if( opt == 0 && argc == 2 ) {
            opt = 's';
        }
    }
    switch( opt ) {
        case 'f':
            input = argc > 1 ? argv[ 1 ] : NULL;
            retcode = md5_filename( input, digest );
            break;
        case 's':
            input = argc > 1 ? argv[ 1 ] : NULL;
            if( input ) {
                md5_buffer( (mUCHAR *) input, strlen( input ), digest );
                retcode = 0;
            }
            else {
                retcode = MD5_MISSING_INPUT;
            }
            break;
        case 't':
            retcode = md5_self_test();
            if( retcode == 0 ) {
                printf( "Self Test Passed\n" );
            }
            else {
                printf( "Self Test Failed\n" );
            }
            break;
        case 'v':
            if( argc < 3 ) {
                retcode = MD5_MISSING_INPUT;
            }
            else {
                retcode = md5_verify_filename( argv[ 1 ], LCaseHex( argv[ 2 ] ) );
                if( retcode == 0 ) {
                    printf( "Hash Verified\n" );
                }
                else {
                    printf( "Hash not verified. No match.\n" );
                }
            }                                                                   
            break;
        case 0:
            retcode = MD5_SYNTAX_ERROR;
        case 'h':
            showuse( opt );
            break;

    }
    if( retcode ) {
        printf( "%s\n", md5_message( retcode ) );
    }
    else {
        if( opt == 'f' || opt == 's' ) {
            printf( "%s\n", md5_digest_string( digest, digest_string ) );
        }
    }

    return( retcode );
}
*/
