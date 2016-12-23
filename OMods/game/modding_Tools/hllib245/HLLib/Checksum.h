/*
 * HLLib
 * Copyright (C) 2006-2013 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your Option) any later
 * version.
 */

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "stdafx.h"
#include "Error.h"

namespace HLLib
{
	hlULong Adler32(const hlByte *lpBuffer, hlUInt uiBufferSize, hlULong uiAdler32 = 0);
	hlULong CRC32(const hlByte *lpBuffer, hlUInt uiBufferSize, hlULong uiCRC = 0);

	struct MD5Context
	{
		hlULong lpState[4];
		hlULong lpBlock[16];
		hlULong uiLength;
	};

	hlVoid MD5_Initialize(MD5Context& context);
	hlVoid MD5_Update(MD5Context& context, const hlByte *lpBuffer, hlUInt uiBufferSize);
	hlVoid MD5_Finalize(MD5Context& context, hlByte (&lpDigest)[16]);

	struct SHA1Context
	{
		hlULong lpState[5];
		hlULong lpBlock[16];
		hlULong uiLength;
	};

	hlVoid SHA1_Initialize(SHA1Context& context);
	hlVoid SHA1_Update(SHA1Context& context, const hlByte *lpBuffer, hlUInt uiBufferSize);
	hlVoid SHA1_Finalize(SHA1Context& context, hlByte (&lpDigest)[20]);

	class Checksum
	{
	public:
		virtual ~Checksum()
		{
		}

		virtual hlULong GetDigestSize() const = 0;
		virtual void Initialize() = 0;
		virtual void Update(const hlByte *lpBuffer, hlUInt uiBufferSize) = 0;
		virtual bool Finalize(const hlByte *lpHash) = 0;
	};

	class CRC32Checksum : public Checksum
	{
	public:
		CRC32Checksum()
		{
			Initialize();
		}

		virtual hlULong GetDigestSize() const
		{
			return sizeof(this->uiChecksum);
		}

		virtual void Initialize()
		{
			this->uiChecksum = 0;
		}

		virtual void Update(const hlByte *lpBuffer, hlUInt uiBufferSize)
		{
			this->uiChecksum = CRC32(lpBuffer, uiBufferSize, this->uiChecksum);
		}

		virtual bool Finalize(const hlByte *lpHash)
		{
			return *reinterpret_cast<const hlULong*>(lpHash) == this->uiChecksum;
		}

	private:
		hlULong uiChecksum;
	};

	class MD5Checksum : public Checksum
	{
	public:
		MD5Checksum()
		{
			Initialize();
		}

		virtual hlULong GetDigestSize() const
		{
			return sizeof(this->context.lpState);
		}

		virtual void Initialize()
		{
			MD5_Initialize(this->context);
		}

		virtual void Update(const hlByte *lpBuffer, hlUInt uiBufferSize)
		{
			MD5_Update(this->context, lpBuffer, uiBufferSize);
		}

		virtual bool Finalize(const hlByte *lpHash)
		{
			hlByte lpDigest[16];
			MD5_Finalize(this->context, lpDigest);
			return memcmp(lpHash, lpDigest, sizeof(lpDigest)) == 0;
		}

	private:
		MD5Context context;
	};

	class SHA1Checksum : public Checksum
	{
	public:
		SHA1Checksum()
		{
			Initialize();
		}

		virtual hlULong GetDigestSize() const
		{
			return sizeof(this->context.lpState);
		}

		virtual void Initialize()
		{
			SHA1_Initialize(this->context);
		}

		virtual void Update(const hlByte *lpBuffer, hlUInt uiBufferSize)
		{
			SHA1_Update(this->context, lpBuffer, uiBufferSize);
		}

		virtual bool Finalize(const hlByte *lpHash)
		{
			hlByte lpDigest[20];
			SHA1_Finalize(this->context, lpDigest);
			return memcmp(lpHash, lpDigest, sizeof(lpDigest)) == 0;
		}

	private:
		SHA1Context context;
	};
}

#endif
