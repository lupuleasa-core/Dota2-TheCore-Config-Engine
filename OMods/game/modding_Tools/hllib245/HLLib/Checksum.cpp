/*
 * HLLib
 * Copyright (C) 2006-2013 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 *
 * Adler32() and CRC32() based on zlib 1.2.3:
 *
 * zlib.h -- interface of the 'zlib' general purpose compression library
 * version 1.2.3, July 18th, 2005
 *
 * Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Jean-loup Gailly jloup@gzip.org
 * Mark Adler madler@alumni.caltech.edu
 */

#include "Checksum.h"

#define BASE 65521UL	// Largest prime smaller than 65536.
#define NMAX 5552		// Largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1

#define DOALDER1(lpBuffer,i)  {uiLow += (lpBuffer)[i]; uiHigh += uiLow;}
#define DOALDER2(lpBuffer,i)  DOALDER1(lpBuffer,i); DOALDER1(lpBuffer,i+1);
#define DOALDER4(lpBuffer,i)  DOALDER2(lpBuffer,i); DOALDER2(lpBuffer,i+2);
#define DOALDER8(lpBuffer,i)  DOALDER4(lpBuffer,i); DOALDER4(lpBuffer,i+4);
#define DOALDER16(lpBuffer)   DOALDER8(lpBuffer,0); DOALDER8(lpBuffer,8);

hlULong HLLib::Adler32(const hlByte *lpBuffer, hlUInt uiBufferSize, hlULong uiAdler32)
{
	hlULong uiLow, uiHigh;

	if(lpBuffer == 0)
	{
		return 1UL;
	}

	// Split Adler-32 into component sums.
	uiLow = uiAdler32 & 0xffff;
	uiHigh = (uiAdler32 >> 16) & 0xffff;

	// In case user likes doing a byte at a time, keep it fast.
	if(uiBufferSize == 1)
	{
		uiLow += *lpBuffer;
		if (uiLow >= BASE)
		{
			uiLow -= BASE;
		}
		uiHigh += uiLow;
		if (uiHigh >= BASE)
		{
			uiHigh -= BASE;
		}
		return uiLow | (uiHigh << 16);
	}

	// In case short lengths are provided, keep it somewhat fast.
	if(uiBufferSize < 16)
	{
		while(uiBufferSize--)
		{
			uiLow += *lpBuffer++;
			uiHigh += uiLow;
		}
		if(uiLow >= BASE)
		{
			uiLow -= BASE;
		}
		uiHigh %= BASE;
		return uiLow | (uiHigh << 16);
	}

	// Do length NMAX blocks - requires just one modulo operation.
	while(uiBufferSize >= NMAX)
	{
		uiBufferSize -= NMAX;
		hlUInt uiN = NMAX / 16;
		do
		{
			DOALDER16(lpBuffer);
			lpBuffer += 16;
		} while(--uiN);
		uiLow %= BASE;
		uiHigh %= BASE;
	}

	// Do remaining bytes (less than NMAX, still just one modulo).
	if(uiBufferSize)
	{
		while(uiBufferSize >= 16)
		{
			uiBufferSize -= 16;
			DOALDER16(lpBuffer);
			lpBuffer += 16;
		}
		while(uiBufferSize--)
		{
			uiLow += *lpBuffer++;
			uiHigh += uiLow;
		}
		uiLow %= BASE;
		uiHigh %= BASE;
	}

	// Return recombined sums.
	return uiLow | (uiHigh << 16);
}

const hlULong lpCRCTable[256] =
{
    0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL,
    0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL,
    0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL,
    0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
    0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL,
    0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
    0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL,
    0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
    0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL,
    0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL,
    0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL,
    0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
    0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL,
    0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL,
    0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL,
    0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
    0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL,
    0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
    0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL,
    0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
    0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL,
    0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL,
    0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL,
    0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
    0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL,
    0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL,
    0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL,
    0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
    0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL,
    0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
    0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL,
    0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
    0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL,
    0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL,
    0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL,
    0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
    0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL,
    0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL,
    0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL,
    0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
    0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL,
    0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
    0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL,
    0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
    0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL,
    0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL,
    0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL,
    0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
    0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL,
    0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL,
    0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL,
    0x2d02ef8dUL
};

#define DOCRC1 uiCRC = lpCRCTable[((hlInt)uiCRC ^ (*lpBuffer++)) & 0xff] ^ (uiCRC >> 8)
#define DOCRC8 DOCRC1; DOCRC1; DOCRC1; DOCRC1; DOCRC1; DOCRC1; DOCRC1; DOCRC1

hlULong HLLib::CRC32(const hlByte *lpBuffer, hlUInt uiBufferSize, hlULong uiCRC)
{
    uiCRC = uiCRC ^ 0xffffffffUL;

    while(uiBufferSize >= 8)
	{
        DOCRC8;
        uiBufferSize -= 8;
    }

    if(uiBufferSize)
	{
		do
		{
			DOCRC1;
		} while(--uiBufferSize);
	}

    return uiCRC ^ 0xffffffffUL;
}

inline hlULong LeftRoate(hlULong value, hlUInt bits)
{
	return (value << bits) | (value >> (32 - bits));
}

inline hlULong SwapEndian(hlULong value)
{
	value = ((value << 8) & 0xFF00FF00UL) | ((value >> 8) & 0x00FF00FFUL);
	return (value << 16) | (value >> 16);
}

const hlULong lpMD5Table[4][16] =
{
	{
		0xD76AA478UL, 0xE8C7B756UL, 0x242070DBUL, 0xC1BDCEEEUL,
		0xF57C0FAFUL, 0x4787C62AUL, 0xA8304613UL, 0xFD469501UL,
		0x698098D8UL, 0x8B44F7AFUL, 0xFFFF5BB1UL, 0x895CD7BEUL,
		0x6B901122UL, 0xFD987193UL, 0xA679438EUL, 0x49B40821UL,
	},
	{
		0xF61E2562UL, 0xC040B340UL, 0x265E5A51UL, 0xE9B6C7AAUL,
		0xD62F105DUL, 0x02441453UL, 0xD8A1E681UL, 0xE7D3FBC8UL,
		0x21E1CDE6UL, 0xC33707D6UL, 0xF4D50D87UL, 0x455A14EDUL,
		0xA9E3E905UL, 0xFCEFA3F8UL, 0x676F02D9UL, 0x8D2A4C8AUL,
	},
	{
		0xFFFA3942UL, 0x8771F681UL, 0x6D9D6122UL, 0xFDE5380CUL,
		0xA4BEEA44UL, 0x4BDECFA9UL, 0xF6BB4B60UL, 0xBEBFBC70UL,
		0x289B7EC6UL, 0xEAA127FAUL, 0xD4EF3085UL, 0x04881D05UL,
		0xD9D4D039UL, 0xE6DB99E5UL, 0x1FA27CF8UL, 0xC4AC5665UL,
	},
	{
		0xF4292244UL, 0x432AFF97UL, 0xAB9423A7UL, 0xFC93A039UL,
		0x655B59C3UL, 0x8F0CCC92UL, 0xFFEFF47DUL, 0x85845DD1UL,
		0x6FA87E4FUL, 0xFE2CE6E0UL, 0xA3014314UL, 0x4E0811A1UL,
		0xF7537E82UL, 0xBD3AF235UL, 0x2AD7D2BBUL, 0xEB86D391UL,
	},
};

const hlUInt lpMD5ShiftAmounts[4][16] =
{
	{ 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, },
	{ 5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20, },
	{ 4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23, },
	{ 6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21, },
};

const hlByte lpMD5Padding[64] =
{
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

hlVoid HLLib::MD5_Initialize(MD5Context& context)
{
	context.lpState[0] = 0x67452301UL;
	context.lpState[1] = 0xEFCDAB89UL;
	context.lpState[2] = 0x98BADCFEUL;
	context.lpState[3] = 0x10325476UL;
	context.uiLength = 0;
}

hlVoid HLLib::MD5_Update(MD5Context& context, const hlByte *lpBuffer, hlUInt uiBufferSize)
{
	hlULong uiBlockLength = context.uiLength % sizeof(context.lpBlock);
	while(uiBlockLength + uiBufferSize >= sizeof(context.lpBlock))
	{
		hlULong uiCopyLength = std::min(static_cast<hlULong>(uiBufferSize), static_cast<hlULong>(sizeof(context.lpBlock) - uiBlockLength));
		memcpy(reinterpret_cast<hlByte*>(context.lpBlock) + uiBlockLength, lpBuffer, uiCopyLength);
		context.uiLength += uiCopyLength;

		lpBuffer += uiCopyLength;
		uiBufferSize -= uiCopyLength;

		{
			hlULong a = context.lpState[0];
			hlULong b = context.lpState[1];
			hlULong c = context.lpState[2];
			hlULong d = context.lpState[3];

			// Round 1.
			for(hlULong i = 0; i < 16; ++i)
			{
				hlULong x = d ^ (b & (c ^ d));

				hlULong t = d;
				d = c;
				c = b;
				b += LeftRoate(a + x + lpMD5Table[0][i] + context.lpBlock[i], lpMD5ShiftAmounts[0][i]);
				a = t;
			}

			// Round 2.
			for(hlULong i = 0; i < 16; ++i)
			{
				hlULong x = (d & b) | (c & (~d));

				hlULong t = d;
				d = c;
				c = b;
				b += LeftRoate(a + x + lpMD5Table[1][i] + context.lpBlock[(5 * i + 1) % 16], lpMD5ShiftAmounts[1][i]);
				a = t;
			}

			// Round 3.
			for(hlULong i = 0; i < 16; ++i)
			{
				hlULong x = b ^ c ^ d;

				hlULong t = d;
				d = c;
				c = b;
				b += LeftRoate(a + x + lpMD5Table[2][i] + context.lpBlock[(3 * i + 5) % 16], lpMD5ShiftAmounts[2][i]);
				a = t;
			}

			// Round 4.
			for(hlULong i = 0; i < 16; ++i)
			{
				hlULong x = c ^ (b | (~d));

				hlULong t = d;
				d = c;
				c = b;
				b += LeftRoate(a + x + lpMD5Table[3][i] + context.lpBlock[(7 * i) % 16], lpMD5ShiftAmounts[3][i]);
				a = t;
			}

			context.lpState[0] += a;
			context.lpState[1] += b;
			context.lpState[2] += c;
			context.lpState[3] += d;
		}

		uiBlockLength = 0;
	}

	memcpy(reinterpret_cast<hlByte*>(context.lpBlock) + uiBlockLength, lpBuffer, uiBufferSize);
	context.uiLength += uiBufferSize;
}

hlVoid HLLib::MD5_Finalize(MD5Context& context, hlByte (&lpDigest)[16])
{
	hlULongLong uiLengthInBits = 8ULL * static_cast<hlULongLong>(context.uiLength);

	hlULong uiBlockLength = context.uiLength % sizeof(context.lpBlock);
	if(uiBlockLength < sizeof(context.lpBlock) - sizeof(hlULongLong))
	{
		MD5_Update(context, lpMD5Padding, sizeof(context.lpBlock) - sizeof(uiLengthInBits) - uiBlockLength);
	}
	else
	{
		MD5_Update(context, lpMD5Padding, 2 * sizeof(context.lpBlock) - sizeof(uiLengthInBits) - uiBlockLength);
	}

	MD5_Update(context, reinterpret_cast<hlByte*>(&uiLengthInBits), sizeof(uiLengthInBits));

	for(hlULong i = 0; i < sizeof(context.lpState) / sizeof(context.lpState[0]); ++i)
	{
		reinterpret_cast<hlULong*>(lpDigest)[i] = context.lpState[i];
	}
}

hlVoid HLLib::SHA1_Initialize(SHA1Context& context)
{
	context.lpState[0] = 0x67452301UL;
	context.lpState[1] = 0xEFCDAB89UL;
	context.lpState[2] = 0x98BADCFEUL;
	context.lpState[3] = 0x10325476UL;
	context.lpState[4] = 0xC3D2E1F0UL;
	context.uiLength = 0;
}

hlVoid HLLib::SHA1_Update(SHA1Context& context, const hlByte *lpBuffer, hlUInt uiBufferSize)
{
	hlULong uiBlockLength = context.uiLength % sizeof(context.lpBlock);
	while(uiBlockLength + uiBufferSize >= sizeof(context.lpBlock))
	{
		hlULong uiCopyLength = std::min(static_cast<hlULong>(uiBufferSize), static_cast<hlULong>(sizeof(context.lpBlock) - uiBlockLength));
		memcpy(reinterpret_cast<hlByte*>(context.lpBlock) + uiBlockLength, lpBuffer, uiCopyLength);
		context.uiLength += uiCopyLength;

		lpBuffer += uiCopyLength;
		uiBufferSize -= uiCopyLength;

		{
			hlULong a = context.lpState[0];
			hlULong b = context.lpState[1];
			hlULong c = context.lpState[2];
			hlULong d = context.lpState[3];
			hlULong e = context.lpState[4];

			hlULong i;
			hlULong lpExtendedBlock[80];

			// Input needs to be big-endian.
			for(i = 0; i < 16; ++i)
			{				
				lpExtendedBlock[i] = SwapEndian(context.lpBlock[i]);
			}

			// Extend the 16 dwords to 80.
			for(; i < 80; ++i)
			{
				lpExtendedBlock[i] = LeftRoate(lpExtendedBlock[i - 3] ^ lpExtendedBlock[i - 8] ^ lpExtendedBlock[i - 14] ^ lpExtendedBlock[i - 16], 1);
			}
			
			// Round 1.
			for(i = 0; i < 20; ++i)
			{
				hlULong x = d ^ (b & (c ^ d));

				hlULong t = LeftRoate(a, 5) + x + e + 0x5A827999UL + lpExtendedBlock[i];
				e = d;
				d = c;
				c = LeftRoate(b, 30);
				b = a;
				a = t;
			}

			// Round 2.
			for(; i < 40; ++i)
			{
				hlULong x = b ^ c ^ d;

				hlULong t = LeftRoate(a, 5) + x + e + 0x6ED9EBA1UL + lpExtendedBlock[i];
				e = d;
				d = c;
				c = LeftRoate(b, 30);
				b = a;
				a = t;
			}

			// Round 3.
			for(; i < 60; ++i)
			{
				hlULong x = (b & c) | ((b | c) & d);

				hlULong t = LeftRoate(a, 5) + x + e + 0x8F1BBCDCUL + lpExtendedBlock[i];
				e = d;
				d = c;
				c = LeftRoate(b, 30);
				b = a;
				a = t;
			}

			// Round 4.
			for(; i < 80; ++i)
			{
				hlULong x = b ^ c ^ d;

				hlULong t = LeftRoate(a, 5) + x + e + 0xCA62C1D6UL + lpExtendedBlock[i];
				e = d;
				d = c;
				c = LeftRoate(b, 30);
				b = a;
				a = t;
			}

			context.lpState[0] += a;
			context.lpState[1] += b;
			context.lpState[2] += c;
			context.lpState[3] += d;
			context.lpState[4] += e;
		}

		uiBlockLength = 0;
	}

	memcpy(reinterpret_cast<hlByte*>(context.lpBlock) + uiBlockLength, lpBuffer, uiBufferSize);
	context.uiLength += uiBufferSize;
}

hlVoid HLLib::SHA1_Finalize(SHA1Context& context, hlByte (&lpDigest)[20])
{
	hlULongLong uiLengthInBits = 8ULL * static_cast<hlULongLong>(context.uiLength);

	hlULong uiBlockLength = context.uiLength % sizeof(context.lpBlock);
	if(uiBlockLength < sizeof(context.lpBlock) - sizeof(hlULongLong))
	{
		SHA1_Update(context, lpMD5Padding, sizeof(context.lpBlock) - sizeof(uiLengthInBits) - uiBlockLength);
	}
	else
	{
		SHA1_Update(context, lpMD5Padding, 2 * sizeof(context.lpBlock) - sizeof(uiLengthInBits) - uiBlockLength);
	}

	// Length needs to be big-endian.
	uiLengthInBits = (uiLengthInBits << 32) | (uiLengthInBits >> 32);
	uiLengthInBits = static_cast<hlULongLong>(SwapEndian(static_cast<hlULong>(uiLengthInBits & 0xFFFFFFFFULL))) | static_cast<hlULongLong>(SwapEndian(static_cast<hlULong>(uiLengthInBits >> 32))) << 32;
	SHA1_Update(context, reinterpret_cast<hlByte*>(&uiLengthInBits), sizeof(uiLengthInBits));

	for(hlULong i = 0; i < sizeof(context.lpState) / sizeof(context.lpState[0]); ++i)
	{
		// Output needs to be big-endian.
		reinterpret_cast<hlULong*>(lpDigest)[i] = SwapEndian(context.lpState[i]);
	}
}