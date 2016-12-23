/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "HLLib.h"
#include "MemoryStream.h"

using namespace HLLib;
using namespace HLLib::Streams;

CMemoryStream::CMemoryStream(hlVoid *lpData, hlULongLong uiBufferSize) : bOpened(hlFalse), uiMode(HL_MODE_INVALID), lpData(lpData), uiBufferSize(uiBufferSize), uiPointer(0), uiLength(0)
{

}

CMemoryStream::~CMemoryStream()
{

}

HLStreamType CMemoryStream::GetType() const
{
	return HL_STREAM_MEMORY;
}

const hlVoid *CMemoryStream::GetBuffer() const
{
	return this->lpData;
}

hlULongLong CMemoryStream::GetBufferSize() const
{
	return this->uiBufferSize;
}

const hlChar *CMemoryStream::GetFileName() const
{
	return "";
}

hlBool CMemoryStream::GetOpened() const
{
	return this->bOpened;
}

hlUInt CMemoryStream::GetMode() const
{
	return this->uiMode;
}

hlBool CMemoryStream::Open(hlUInt uiMode)
{
	if(this->uiBufferSize != 0 && this->lpData == 0)
	{
		LastError.SetErrorMessage("Memory stream is null.");
		return hlFalse;
	}

	if((uiMode & (HL_MODE_READ | HL_MODE_WRITE)) == 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);
		return hlFalse;
	}

	this->uiPointer = 0;
	this->uiLength = (uiMode & HL_MODE_READ) ? this->uiBufferSize : 0;

	this->bOpened = hlTrue;
	this->uiMode = uiMode;

	return hlTrue;
}

hlVoid CMemoryStream::Close()
{
	this->bOpened = hlFalse;
	this->uiMode = HL_MODE_INVALID;
	this->uiPointer = 0;
	this->uiLength = 0;
}

hlULongLong CMemoryStream::GetStreamSize() const
{
	return this->uiLength;
}

hlULongLong CMemoryStream::GetStreamPointer() const
{
	return this->uiPointer;
}

hlULongLong CMemoryStream::Seek(hlLongLong iOffset, HLSeekMode eSeekMode)
{
	if(!this->bOpened)
	{
		return 0;
	}

	switch(eSeekMode)
	{
		case HL_SEEK_BEGINNING:
			this->uiPointer = 0;
			break;
		case HL_SEEK_CURRENT:

			break;
		case HL_SEEK_END:
			this->uiPointer = this->uiLength;
			break;
	}

	hlLongLong iPointer = static_cast<hlLongLong>(this->uiPointer) + iOffset;

	if(iPointer < 0)
	{
		iPointer = 0;
	}
	else if(iPointer > static_cast<hlLongLong>(this->uiLength))
	{
		iPointer = static_cast<hlLongLong>(this->uiLength);
	}

	this->uiPointer = static_cast<hlULongLong>(iPointer);

	return this->uiPointer;
}

hlBool CMemoryStream::Read(hlChar &cChar)
{
	if(!this->bOpened)
	{
		return hlFalse;
	}

	if((this->uiMode & HL_MODE_READ) == 0)
	{
		LastError.SetErrorMessage("Stream not in read mode.");
		return hlFalse;
	}

	if(this->uiPointer == this->uiLength)
	{
		return hlFalse;
	}
	else
	{
		cChar = *((hlChar *)this->lpData + this->uiPointer++);

		return hlTrue;
	}
}

hlUInt CMemoryStream::Read(hlVoid *lpData, hlUInt uiBytes)
{
	if(!this->bOpened)
	{
		return 0;
	}

	if((this->uiMode & HL_MODE_READ) == 0)
	{
		LastError.SetErrorMessage("Stream not in read mode.");
		return 0;
	}

	if(this->uiPointer == this->uiLength)
	{
		return 0;
	}
	else if(this->uiPointer + static_cast<hlULongLong>(uiBytes) > this->uiLength) // This right?
	{
		uiBytes = static_cast<hlUInt>(this->uiLength - this->uiPointer);

		memcpy(lpData, (hlByte *)this->lpData + this->uiPointer, uiBytes);

		this->uiPointer = this->uiLength;

		return uiBytes;
	}
	else
	{
		memcpy(lpData, (hlByte *)this->lpData + this->uiPointer, uiBytes);

		this->uiPointer += static_cast<hlULongLong>(uiBytes);

		return uiBytes;
	}
}

hlBool CMemoryStream::Write(hlChar cChar)
{
	if(!this->bOpened)
	{
		return hlFalse;
	}

	if((this->uiMode & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessage("Stream not in write mode.");
		return hlFalse;
	}

	if(this->uiPointer == this->uiBufferSize)
	{
		return hlFalse;
	}
	else
	{
		*((hlChar *)this->lpData + this->uiPointer++) = cChar;

		if(this->uiPointer > this->uiLength)
		{
			this->uiLength = this->uiPointer;
		}

		return hlTrue;
	}
}

hlUInt CMemoryStream::Write(const hlVoid *lpData, hlUInt uiBytes)
{
	if(!this->bOpened)
	{
		return 0;
	}

	if((this->uiMode & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessage("Stream not in write mode.");
		return 0;
	}

	if(this->uiPointer == this->uiBufferSize)
	{
		return 0;
	}
	else if(this->uiPointer + static_cast<hlULongLong>(uiBytes) > this->uiBufferSize)
	{
		uiBytes = static_cast<hlUInt>(this->uiBufferSize - this->uiPointer);

		memcpy((hlByte *)this->lpData + this->uiPointer, lpData, uiBytes);

		this->uiPointer = this->uiBufferSize;

		if(this->uiPointer > this->uiLength)
		{
			this->uiLength = this->uiPointer;
		}

		return uiBytes;
	}
	else
	{
		memcpy((hlByte *)this->lpData + this->uiPointer, lpData, uiBytes);

		this->uiPointer += static_cast<hlULongLong>(uiBytes);

		if(this->uiPointer > this->uiLength)
		{
			this->uiLength = this->uiPointer;
		}

		return uiBytes;
	}
}
