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
#include "MappingStream.h"

using namespace HLLib;
using namespace HLLib::Streams;

CMappingStream::CMappingStream(Mapping::CMapping &Mapping, hlULongLong uiMappingOffset, hlULongLong uiMappingSize, hlULongLong uiViewSize) : bOpened(hlFalse), uiMode(HL_MODE_INVALID), Mapping(Mapping), uiMappingOffset(uiMappingOffset), uiMappingSize(uiMappingSize), uiViewSize(uiViewSize), pView(0), uiPointer(0), uiLength(0)
{
	if(this->uiViewSize == 0)
	{
		switch(this->Mapping.GetType())
		{
		case HL_MAPPING_FILE:
			if(this->Mapping.GetMode() & HL_MODE_QUICK_FILEMAPPING)
			{
		case HL_MAPPING_MEMORY:
				this->uiViewSize = this->uiMappingSize;
				break;
			}
		default:
			this->uiViewSize = HL_DEFAULT_VIEW_SIZE;
			break;
		}
	}
}

CMappingStream::~CMappingStream()
{
	this->Close();
}

HLStreamType CMappingStream::GetType() const
{
	return HL_STREAM_MAPPING;
}

const Mapping::CMapping &CMappingStream::GetMapping() const
{
	return this->Mapping;
}

const hlChar *CMappingStream::GetFileName() const
{
	return "";
}

hlBool CMappingStream::GetOpened() const
{
	return this->bOpened;
}

hlUInt CMappingStream::GetMode() const
{
	return this->uiMode;
}

hlBool CMappingStream::Open(hlUInt uiMode)
{
	this->Close();

	if((uiMode & (HL_MODE_READ | HL_MODE_WRITE)) == 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);
		return hlFalse;
	}

	if((uiMode & HL_MODE_READ) != 0 && (this->Mapping.GetMode() & HL_MODE_READ) == 0)
	{
		LastError.SetErrorMessage("Mapping does not have read permissions.");
		return hlFalse;
	}

	if((uiMode & HL_MODE_WRITE) != 0 && (this->Mapping.GetMode() & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessage("Mapping does not have write permissions.");
		return hlFalse;
	}

	this->uiPointer = 0;
	this->uiLength = (uiMode & HL_MODE_READ) ? this->uiMappingSize : 0;

	this->bOpened = hlTrue;
	this->uiMode = uiMode;

	return hlTrue;
}

hlVoid CMappingStream::Close()
{
	this->bOpened = hlFalse;
	this->uiMode = HL_MODE_INVALID;

	this->Mapping.Unmap(this->pView);

	this->uiPointer = 0;
	this->uiLength = 0;
}

hlULongLong CMappingStream::GetStreamSize() const
{
	return this->uiLength;
}

hlULongLong CMappingStream::GetStreamPointer() const
{
	return this->uiPointer;
}

hlULongLong CMappingStream::Seek(hlLongLong iOffset, HLSeekMode eSeekMode)
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

hlBool CMappingStream::Read(hlChar &cChar)
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

	if(this->uiPointer < this->uiLength)
	{
		if(!this->Map(this->uiPointer))
		{
			return 0;
		}

		hlULongLong uiViewPointer = this->uiPointer - (this->pView->GetAllocationOffset() + this->pView->GetOffset() - this->uiMappingOffset);
		hlULongLong uiViewBytes = this->pView->GetLength() - uiViewPointer;

		if(uiViewBytes >= 1)
		{
			cChar = *(static_cast<const hlChar *>(this->pView->GetView()) + uiViewPointer);
			this->uiPointer++;
			return 1;
		}
	}

	return 0;
}

hlUInt CMappingStream::Read(hlVoid *lpData, hlUInt uiBytes)
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
	else
	{
		hlULongLong uiOffset = 0;
		while(uiBytes && this->uiPointer < this->uiLength)
		{
			if(!this->Map(this->uiPointer))
			{
				break;
			}

			hlULongLong uiViewPointer = this->uiPointer - (this->pView->GetAllocationOffset() + this->pView->GetOffset() - this->uiMappingOffset);
			hlULongLong uiViewBytes = this->pView->GetLength() - uiViewPointer;

			if(uiViewBytes >= uiBytes)
			{
				memcpy(static_cast<hlByte *>(lpData) + uiOffset, static_cast<const hlByte *>(this->pView->GetView()) + uiViewPointer, uiBytes);
				this->uiPointer += static_cast<hlULongLong>(uiBytes);
				uiOffset += uiBytes;
				break;
			}
			else
			{
				memcpy(static_cast<hlByte *>(lpData) + uiOffset, static_cast<const hlByte *>(this->pView->GetView()) + uiViewPointer, static_cast<size_t>(uiViewBytes));
				this->uiPointer += uiViewBytes;
				uiOffset += uiViewBytes;
				uiBytes -= static_cast<hlUInt>(uiViewBytes);
			}
		}

		return static_cast<hlUInt>(uiOffset);
	}
}

hlBool CMappingStream::Write(hlChar cChar)
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

	if(this->uiPointer < this->uiMappingSize)
	{
		if(!this->Map(this->uiPointer))
		{
			return 0;
		}

		hlULongLong uiViewPointer = this->uiPointer - (this->pView->GetAllocationOffset() + this->pView->GetOffset() - this->uiMappingOffset);
		hlULongLong uiViewBytes = this->pView->GetLength() - uiViewPointer;

		if(uiViewBytes >= 1)
		{
			*(static_cast<hlChar *>(const_cast<hlVoid *>(this->pView->GetView())) + uiViewPointer) = cChar;
			this->uiPointer++;

			if(this->uiPointer > this->uiLength)
			{
				this->uiLength = this->uiPointer;
			}

			return 1;
		}
	}

	return 0;
}

hlUInt CMappingStream::Write(const hlVoid *lpData, hlUInt uiBytes)
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

	if(this->uiPointer == this->uiMappingSize)
	{
		return 0;
	}
	else
	{
		hlULongLong uiOffset = 0;
		while(uiBytes && this->uiPointer < this->uiMappingSize)
		{
			if(!this->Map(this->uiPointer))
			{
				break;
			}

			hlULongLong uiViewPointer = this->uiPointer - (this->pView->GetAllocationOffset() + this->pView->GetOffset() - this->uiMappingOffset);
			hlULongLong uiViewBytes = this->pView->GetLength() - uiViewPointer;

			if(uiViewBytes >= uiBytes)
			{
				memcpy(static_cast<hlByte *>(const_cast<hlVoid *>(this->pView->GetView())) + uiViewPointer, static_cast<const hlByte *>(lpData) + uiOffset, uiBytes);
				this->uiPointer += static_cast<hlULongLong>(uiBytes);
				uiOffset += uiBytes;
				break;
			}
			else
			{
				memcpy(static_cast<hlByte *>(const_cast<hlVoid *>(this->pView->GetView())) + uiViewPointer, static_cast<const hlByte *>(lpData) + uiOffset, static_cast<size_t>(uiViewBytes));
				this->uiPointer += uiViewBytes;
				uiOffset += uiViewBytes;
				uiBytes -= static_cast<hlUInt>(uiViewBytes);
			}
		}

		if(this->uiPointer > this->uiLength)
		{
			this->uiLength = this->uiPointer;
		}

		return static_cast<hlUInt>(uiOffset);
	}
}

hlBool CMappingStream::Map(hlULongLong uiPointer)
{
	uiPointer = (uiPointer / this->uiViewSize) * this->uiViewSize;

	if(this->pView)
	{
		if(this->pView->GetAllocationOffset() - this->uiMappingOffset == uiPointer)
		{
			return hlTrue;
		}
	}

	hlULongLong uiLength = uiPointer + this->uiViewSize > this->uiMappingSize ? this->uiMappingSize - uiPointer : this->uiViewSize;

	return this->Mapping.Map(this->pView, this->uiMappingOffset + uiPointer, uiLength);
}
