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
#include "GCFStream.h"

using namespace HLLib;
using namespace HLLib::Streams;

CGCFStream::CGCFStream(const CGCFFile &GCFFile, hlUInt uiFileID) : bOpened(hlFalse), uiMode(HL_MODE_INVALID), GCFFile(GCFFile), uiFileID(uiFileID), pView(0), uiPointer(0), uiLength(0)
{

}

CGCFStream::~CGCFStream()
{
	this->Close();
}

HLStreamType CGCFStream::GetType() const
{
	return HL_STREAM_GCF;
}

const CGCFFile &CGCFStream::GetPackage() const
{
	return this->GCFFile;
}

const hlChar *CGCFStream::GetFileName() const
{
	return this->GCFFile.lpDirectoryNames + this->GCFFile.lpDirectoryEntries[this->uiFileID].uiNameOffset;
}

hlBool CGCFStream::GetOpened() const
{
	return this->bOpened;
}

hlUInt CGCFStream::GetMode() const
{
	return this->uiMode;
}

hlBool CGCFStream::Open(hlUInt uiMode)
{
	this->Close();

	if(!this->GCFFile.GetOpened())
	{
		LastError.SetErrorMessage("GCF file not opened.");
		return hlFalse;
	}

	if((uiMode & (HL_MODE_READ | HL_MODE_WRITE)) == 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);
		return hlFalse;
	}

	if((uiMode & HL_MODE_READ) != 0 && (this->GCFFile.pMapping->GetMode() & HL_MODE_READ) == 0)
	{
		LastError.SetErrorMessage("GCF file does not have read permissions.");
		return hlFalse;
	}

	if((uiMode & HL_MODE_WRITE) != 0 && (this->GCFFile.pMapping->GetMode() & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessage("GCF file does not have write permissions.");
		return hlFalse;
	}

	this->uiPointer = 0;
	this->uiLength = (uiMode & HL_MODE_READ) ? this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize : 0;

	this->bOpened = hlTrue;
	this->uiMode = uiMode;

	this->uiBlockEntryIndex = this->GCFFile.lpDirectoryMapEntries[this->uiFileID].uiFirstBlockIndex;
	this->uiBlockEntryOffset = 0;
	this->uiDataBlockIndex = this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFirstDataBlockIndex;
	this->uiDataBlockOffset = 0;

	return hlTrue;
}

hlVoid CGCFStream::Close()
{
	this->bOpened = hlFalse;
	this->uiMode = HL_MODE_INVALID;

	this->GCFFile.pMapping->Unmap(this->pView);

	this->uiPointer = 0;
	this->uiLength = 0;
}

hlULongLong CGCFStream::GetStreamSize() const
{
	return this->uiLength;
}

hlULongLong CGCFStream::GetStreamPointer() const
{
	return this->uiPointer;
}

hlULongLong CGCFStream::Seek(hlLongLong iOffset, HLSeekMode eSeekMode)
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

hlBool CGCFStream::Read(hlChar &cChar)
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

		hlULongLong uiViewPointer = this->uiPointer - (this->uiBlockEntryOffset + this->uiDataBlockOffset);
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

hlUInt CGCFStream::Read(hlVoid *lpData, hlUInt uiBytes)
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

			hlULongLong uiViewPointer = this->uiPointer - (this->uiBlockEntryOffset + this->uiDataBlockOffset);
			hlULongLong uiViewBytes = this->pView->GetLength() - uiViewPointer;

			if(uiViewBytes >= static_cast<hlULongLong>(uiBytes))
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

hlBool CGCFStream::Write(hlChar cChar)
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

	if(this->uiPointer < this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize)
	{
		if(!this->Map(this->uiPointer))
		{
			return 0;
		}

		hlULongLong uiViewPointer = this->uiPointer - (this->uiBlockEntryOffset + this->uiDataBlockOffset);
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

hlUInt CGCFStream::Write(const hlVoid *lpData, hlUInt uiBytes)
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

	if(this->uiPointer == this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize)
	{
		return 0;
	}
	else
	{
		hlULongLong uiOffset = 0;
		while(uiBytes && this->uiPointer < this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize)
		{
			if(!this->Map(this->uiPointer))
			{
				break;
			}

			hlULongLong uiViewPointer = this->uiPointer - (this->uiBlockEntryOffset + this->uiDataBlockOffset);
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

hlBool CGCFStream::Map(hlULongLong uiPointer)
{
	if(uiPointer < this->uiBlockEntryOffset + this->uiDataBlockOffset)
	{
		this->uiBlockEntryIndex = this->GCFFile.lpDirectoryMapEntries[this->uiFileID].uiFirstBlockIndex;
		this->uiBlockEntryOffset = 0;
		this->uiDataBlockIndex = this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFirstDataBlockIndex;
		this->uiDataBlockOffset = 0;
	}

	hlULongLong uiLength = this->uiDataBlockOffset + this->GCFFile.pDataBlockHeader->uiBlockSize > this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize ? this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize - this->uiDataBlockOffset : this->GCFFile.pDataBlockHeader->uiBlockSize;
	//hlUInt uiDataBlockTerminator = this->pDataBlockHeader->uiBlockCount >= 0x0000ffff ? 0xffffffff : 0x0000ffff;
	hlUInt uiDataBlockTerminator = this->GCFFile.pFragmentationMapHeader->uiTerminator == 0 ? 0x0000ffff : 0xffffffff;

	while((uiPointer >= this->uiBlockEntryOffset + this->uiDataBlockOffset + uiLength) && (this->uiBlockEntryIndex != this->GCFFile.pDataBlockHeader->uiBlockCount))
	{
		// Loop through each data block fragment.
		while((uiPointer >= this->uiBlockEntryOffset + this->uiDataBlockOffset + uiLength) && (this->uiDataBlockIndex < uiDataBlockTerminator && this->uiDataBlockOffset < this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize))
		{
			// Get the next data block fragment.
			this->uiDataBlockIndex = this->GCFFile.lpFragmentationMap[this->uiDataBlockIndex].uiNextDataBlockIndex;
			this->uiDataBlockOffset += static_cast<hlULongLong>(this->GCFFile.pDataBlockHeader->uiBlockSize);

			uiLength = this->uiDataBlockOffset + this->GCFFile.pDataBlockHeader->uiBlockSize > this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize ? static_cast<hlULongLong>(this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize) - this->uiDataBlockOffset : static_cast<hlULongLong>(this->GCFFile.pDataBlockHeader->uiBlockSize);
		}

		if(this->uiDataBlockOffset >= static_cast<hlULongLong>(this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize))
		{
			// Get the next data block.
			this->uiBlockEntryOffset += static_cast<hlULongLong>(this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize);
			this->uiBlockEntryIndex = this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiNextBlockEntryIndex;

			this->uiDataBlockOffset = 0;
			if(this->uiBlockEntryIndex != this->GCFFile.pDataBlockHeader->uiBlockCount)
			{
				this->uiDataBlockIndex = this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFirstDataBlockIndex;
			}

			uiLength = this->uiDataBlockOffset + this->GCFFile.pDataBlockHeader->uiBlockSize > this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize ? static_cast<hlULongLong>(this->GCFFile.lpBlockEntries[this->uiBlockEntryIndex].uiFileDataSize) - this->uiDataBlockOffset : static_cast<hlULongLong>(this->GCFFile.pDataBlockHeader->uiBlockSize);
		}
	}

	if(this->uiBlockEntryIndex == this->GCFFile.pDataBlockHeader->uiBlockCount || this->uiDataBlockIndex >= uiDataBlockTerminator)
	{
		if(this->uiBlockEntryOffset + this->uiDataBlockOffset < static_cast<hlULongLong>(this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize))
		{
#ifdef _WIN32
			LastError.SetErrorMessageFormated("Unexpected end of GCF stream (%I64u B of %u B).  Has the GCF file been completely acquired?", this->uiBlockEntryOffset + this->uiDataBlockOffset, this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize);
#else
			LastError.SetErrorMessageFormated("Unexpected end of GCF stream (%llu B of %u B).  Has the GCF file been completely acquired?", this->uiBlockEntryOffset + this->uiDataBlockOffset, this->GCFFile.lpDirectoryEntries[this->uiFileID].uiItemSize);
#endif
		}

		this->GCFFile.pMapping->Unmap(this->pView);
		return hlFalse;
	}

	if(this->pView)
	{
		if(this->pView->GetAllocationOffset() == this->GCFFile.pDataBlockHeader->uiFirstBlockOffset + this->uiDataBlockIndex * this->GCFFile.pDataBlockHeader->uiBlockSize)
		{
			return hlTrue;
		}
	}

	return this->GCFFile.pMapping->Map(this->pView, static_cast<hlULongLong>(this->GCFFile.pDataBlockHeader->uiFirstBlockOffset) + static_cast<hlULongLong>(this->uiDataBlockIndex) * static_cast<hlULongLong>(this->GCFFile.pDataBlockHeader->uiBlockSize), uiLength);
}
