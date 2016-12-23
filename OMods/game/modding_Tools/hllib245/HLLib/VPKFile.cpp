/*
 * HLLib
 * Copyright (C) 2006-2013 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "HLLib.h"
#include "VPKFile.h"
#include "Mappings.h"
#include "Streams.h"
#include "Checksum.h"

using namespace HLLib;

#define HL_VPK_SIGNATURE 0x55aa1234
#define HL_VPK_NO_ARCHIVE 0x7fff

#define HL_VPK_CHECKSUM_LENGTH 0x00008000

const char *CVPKFile::lpAttributeNames[] = { "Archives", "Version" };
const char *CVPKFile::lpItemAttributeNames[] = { "Preload Bytes", "Archive", "CRC" };

CVPKFile::CVPKFile() : CPackage(), pView(0), uiArchiveCount(0), lpArchives(0), pHeader(0), pExtendedHeader(0), lpArchiveHashes(0), pDirectoryItems(0)
{

}

CVPKFile::~CVPKFile()
{
	this->Close();
}

HLPackageType CVPKFile::GetType() const
{
	return HL_PACKAGE_VPK;
}

const hlChar *CVPKFile::GetExtension() const
{
	return "vpk";
}

const hlChar *CVPKFile::GetDescription() const
{
	return "Valve Package File";
}

hlBool CVPKFile::MapString(const hlChar *&lpViewData, const hlChar *lpViewDirectoryDataEnd, const hlChar *&lpString)
{
	lpString = lpViewData;
	while(hlTrue)
	{
		if(lpViewData == lpViewDirectoryDataEnd)
		{
			LastError.SetErrorMessage("Invalid file: Mapping bounds exceeded while searching for string null terminator.");
			return hlFalse;
		}
		hlBool bMapped = *lpViewData == '\0';
		lpViewData++;
		if(bMapped)
		{
			return hlTrue;
		}
	}
	return hlTrue;
}

hlBool CVPKFile::MapDataStructures()
{
	if(!this->pMapping->Map(this->pView, 0, this->pMapping->GetMappingSize()))
	{
		return hlFalse;
	}

	this->pDirectoryItems = new CDirectoryItemList();

	const hlChar *lpViewData = static_cast<const hlChar *>(this->pView->GetView());
	const hlChar *lpViewDataEnd = static_cast<const hlChar *>(this->pView->GetView()) + this->pView->GetLength();
	const hlChar *lpViewDirectoryDataEnd = lpViewDataEnd;

	if(lpViewData + sizeof(VPKHeader) > lpViewDataEnd)
	{
		LastError.SetErrorMessage("Invalid file: The file map is not within mapping bounds.");
		return hlFalse;
	}
	this->pHeader = reinterpret_cast<const VPKHeader *>(lpViewData);

	if(this->pHeader->uiSignature != HL_VPK_SIGNATURE)
	{
		// The original version had no signature.
		this->pHeader = 0;
	}
	else
	{
		if(this->pHeader->uiVersion > 2)
		{
			LastError.SetErrorMessageFormated("Invalid VPK version (v%u): you have a version of a VPK file that HLLib does not know how to read. Check for product updates.", this->pHeader->uiVersion);
			return hlFalse;
		}
		lpViewData += sizeof(VPKHeader);
		if(this->pHeader->uiVersion >= 2)
		{
			this->pExtendedHeader = reinterpret_cast<const VPKExtendedHeader *>(lpViewData);
			lpViewData += sizeof(VPKExtendedHeader);
		}
		lpViewDirectoryDataEnd = lpViewData + this->pHeader->uiDirectoryLength;
		if(this->pExtendedHeader != 0)
		{
			this->lpArchiveHashes = reinterpret_cast<const VPKArchiveHash *>(lpViewDirectoryDataEnd);
		}
	}

	while(hlTrue)
	{
		const hlChar *lpExtension;
		if(!this->MapString(lpViewData, lpViewDirectoryDataEnd, lpExtension))
		{
			return hlFalse;
		}
		if(*lpExtension == '\0')
		{
			break;
		}

		while(hlTrue)
		{
			const hlChar *lpPath;
			if(!this->MapString(lpViewData, lpViewDirectoryDataEnd, lpPath))
			{
				return hlFalse;
			}
			if(*lpPath == '\0')
			{
				break;
			}

			while(hlTrue)
			{
				const hlChar *lpName;
				if(!this->MapString(lpViewData, lpViewDirectoryDataEnd, lpName))
				{
					return hlFalse;
				}
				if(*lpName == '\0')
				{
					break;
				}

				if(lpViewData + sizeof(VPKDirectoryEntry) > lpViewDirectoryDataEnd)
				{
					LastError.SetErrorMessage("Invalid file: The file map is not within mapping bounds.");
					return hlFalse;
				}
				const VPKDirectoryEntry *pDirectoryEntry = reinterpret_cast<const VPKDirectoryEntry *>(lpViewData);
				lpViewData += sizeof(VPKDirectoryEntry);

				const hlVoid *lpPreloadData = 0;
				if(pDirectoryEntry->uiArchiveIndex == HL_VPK_NO_ARCHIVE && pDirectoryEntry->uiEntryLength > 0)
				{
					assert(pDirectoryEntry->uiPreloadBytes == 0);
					if(lpViewDirectoryDataEnd + pDirectoryEntry->uiEntryOffset + pDirectoryEntry->uiEntryLength <= lpViewDataEnd)
					{
						lpPreloadData = lpViewDirectoryDataEnd + pDirectoryEntry->uiEntryOffset;
					}
				}
				else if(pDirectoryEntry->uiPreloadBytes > 0)
				{
					if(lpViewData + pDirectoryEntry->uiPreloadBytes > lpViewDirectoryDataEnd)
					{
						LastError.SetErrorMessage("Invalid file: The file map is not within mapping bounds.");
						return hlFalse;
					}
					lpPreloadData = lpViewData;
					lpViewData += pDirectoryEntry->uiPreloadBytes;
				}
				if(pDirectoryEntry->uiArchiveIndex != HL_VPK_NO_ARCHIVE)
				{
					if((hlUInt)pDirectoryEntry->uiArchiveIndex + 1 > this->uiArchiveCount)
					{
						this->uiArchiveCount = pDirectoryEntry->uiArchiveIndex + 1;
					}
				}

				this->pDirectoryItems->push_back(new VPKDirectoryItem(lpExtension, lpPath, lpName, pDirectoryEntry, lpPreloadData));
			}
		}
	}

	const hlChar *lpFileName = this->pMapping->GetFileName();
	if(this->uiArchiveCount > 0 && lpFileName != 0)
	{
		const hlChar *lpExtension = strrchr(lpFileName, '.');
		if(lpExtension && lpExtension - lpFileName > 3 && _strnicmp(lpExtension - 3, "dir", 3) == 0)
		{
			hlChar *lpArchiveFileName = new hlChar[strlen(lpFileName) + 2 + 1];  // We need 5 digits to print a short, but we already have 3 for dir.
			hlChar *lpArchiveNumber = lpArchiveFileName + (lpExtension - lpFileName) - 3;
			strcpy(lpArchiveFileName, lpFileName);

			this->lpArchives = new VPKArchive[this->uiArchiveCount];
			memset(this->lpArchives, 0, this->uiArchiveCount * sizeof(VPKArchive));
			for(hlUInt i = 0; i < this->uiArchiveCount; i++)
			{
				hlInt iPrinted = sprintf(lpArchiveNumber, "%0.3u", i);
				if(iPrinted > 0)
				{
					strcat(lpArchiveNumber + iPrinted, lpExtension);

					if(this->pMapping->GetMode() & HL_MODE_NO_FILEMAPPING)
					{
						this->lpArchives[i].pStream = new Streams::CFileStream(lpArchiveFileName);
						this->lpArchives[i].pMapping = new Mapping::CStreamMapping(*this->lpArchives[i].pStream);

						if(!this->lpArchives[i].pMapping->Open(this->pMapping->GetMode()))
						{
							delete this->lpArchives[i].pMapping;
							this->lpArchives[i].pMapping = 0;

							delete this->lpArchives[i].pStream;
							this->lpArchives[i].pStream = 0;
						}
					}
					else
					{
						this->lpArchives[i].pMapping = new Mapping::CFileMapping(lpArchiveFileName);

						if(!this->lpArchives[i].pMapping->Open(this->pMapping->GetMode()))
						{
							delete this->lpArchives[i].pMapping;
							this->lpArchives[i].pMapping = 0;
						}
					}
				}
			}

			delete []lpArchiveFileName;
		}
	}

	return hlTrue;
}

hlVoid CVPKFile::UnmapDataStructures()
{
	if(this->lpArchives != 0)
	{
		for(hlUInt i = 0; i < this->uiArchiveCount; i++)
		{
			if(this->lpArchives[i].pMapping != 0)
			{
				this->lpArchives[i].pMapping->Close();
				delete this->lpArchives[i].pMapping;
			}

			if(this->lpArchives[i].pStream != 0)
			{
				this->lpArchives[i].pStream->Close();
				delete this->lpArchives[i].pStream;
			}
		}
	}

	this->uiArchiveCount = 0;
	delete []this->lpArchives;
	this->lpArchives = 0;

	this->pHeader = 0;
	if(this->pDirectoryItems != 0)
	{
		for(CDirectoryItemList::iterator i = this->pDirectoryItems->begin(); i != this->pDirectoryItems->end(); ++i)
		{
			delete *i;
		}
		delete this->pDirectoryItems;
		this->pDirectoryItems = 0;
	}

	this->pMapping->Unmap(this->pView);
}

CDirectoryFolder *CVPKFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder(this);

	const hlChar *lpLastPath = 0;
	CDirectoryFolder *pLastInsertFolder = 0;

	// Loop through each file in the VPK file.
	for(CDirectoryItemList::const_iterator i = this->pDirectoryItems->begin(); i != this->pDirectoryItems->end(); ++i)
	{
		const VPKDirectoryItem *pDirectoryItem = *i;

		CDirectoryFolder *pInsertFolder;
		if(pDirectoryItem->lpPath == lpLastPath)
		{
			pInsertFolder = pLastInsertFolder;
		}
		else
		{
			pInsertFolder = pRoot;

			if(*pDirectoryItem->lpPath != '\0' && strcmp(pDirectoryItem->lpPath, " ") != 0)
			{
				// Tokenize the file path and create the directories.
				hlChar *lpPath = new hlChar[strlen(pDirectoryItem->lpPath) + 1];
				strcpy(lpPath, pDirectoryItem->lpPath);
				hlChar *lpToken = strtok(lpPath, "/\\");
				while(lpToken != 0)
				{
					// Check if the directory exists.
					CDirectoryItem *pItem = pInsertFolder->GetItem(lpToken);
					if(pItem == 0 || pItem->GetType() == HL_ITEM_FILE)
					{
						// It doesn't, create it.
						pInsertFolder = pInsertFolder->AddFolder(lpToken);
					}
					else
					{
						// It does, use it.
						pInsertFolder = static_cast<CDirectoryFolder *>(pItem);
					}
					lpToken = strtok(0, "/\\");
				}
				delete []lpPath;
			}

			lpLastPath = pDirectoryItem->lpPath;
			pLastInsertFolder = pInsertFolder;
		}

		hlChar *lpFileName = new hlChar[strlen(pDirectoryItem->lpName) + 1 + strlen(pDirectoryItem->lpExtention) + 1];
		strcpy(lpFileName, pDirectoryItem->lpName);
		strcat(lpFileName, ".");
		strcat(lpFileName, pDirectoryItem->lpExtention);

		pInsertFolder->AddFile(lpFileName, -1, const_cast<VPKDirectoryItem *>(pDirectoryItem));

		delete []lpFileName;
	}

	return pRoot;
}

hlUInt CVPKFile::GetAttributeCountInternal() const
{
	return HL_VPK_PACKAGE_COUNT;
}

const hlChar *CVPKFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_VPK_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CVPKFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_VPK_PACKAGE_Archives:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->uiArchiveCount, hlFalse);
		return hlTrue;
	case HL_VPK_PACKAGE_Version:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader ? this->pHeader->uiVersion : 0, hlFalse);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CVPKFile::GetItemAttributeCountInternal() const
{
	return HL_VPK_ITEM_COUNT;
}

const hlChar *CVPKFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_VPK_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CVPKFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			const VPKDirectoryItem *pDirectoryItem = static_cast<const VPKDirectoryItem *>(pFile->GetData());
			switch(eAttribute)
			{
				case HL_VPK_ITEM_PRELOAD_BYTES:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->pDirectoryEntry->uiPreloadBytes, hlFalse);
					return hlTrue;
				}
				case HL_VPK_ITEM_ARCHIVE:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->pDirectoryEntry->uiArchiveIndex, hlFalse);
					return hlTrue;
				}
				case HL_VPK_ITEM_CRC:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->pDirectoryEntry->uiCRC, hlTrue);
					return hlTrue;
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CVPKFile::GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const
{
	const VPKDirectoryItem *pDirectoryItem = static_cast<const VPKDirectoryItem *>(pFile->GetData());

	if(pDirectoryItem->pDirectoryEntry->uiArchiveIndex == HL_VPK_NO_ARCHIVE)
	{
		bExtractable = pDirectoryItem->lpPreloadData != 0 || (pDirectoryItem->pDirectoryEntry->uiPreloadBytes == 0 && pDirectoryItem->pDirectoryEntry->uiEntryLength == 0);
	}
	else if(pDirectoryItem->pDirectoryEntry->uiEntryLength != 0)
	{
		Mapping::CMapping *pMapping = this->lpArchives[pDirectoryItem->pDirectoryEntry->uiArchiveIndex].pMapping;

		bExtractable = pMapping ? pDirectoryItem->pDirectoryEntry->uiEntryOffset + pDirectoryItem->pDirectoryEntry->uiEntryLength <= pMapping->GetMappingSize() : hlFalse;
	}
	else
	{
		bExtractable = hlTrue;
	}

	return hlTrue;
}

hlBool CVPKFile::GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const
{
	hlBool bExtractable = hlFalse;
	if(GetFileExtractableInternal(pFile, bExtractable))
	{
		if(bExtractable)
		{
			Streams::IStream *pStream = 0;
			if(this->CreateStreamInternal(pFile, pStream))
			{
				if(pStream->Open(HL_MODE_READ))
				{
					hlULong uiChecksum = 0;
					eValidation = HL_VALIDATES_OK;

					hlULongLong uiTotalBytes = 0, uiFileBytes = pStream->GetStreamSize();
					hlUInt uiBufferSize;
					hlByte lpBuffer[HL_VPK_CHECKSUM_LENGTH];

					hlBool bCancel = hlFalse;
					hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);

					while((uiBufferSize = pStream->Read(lpBuffer, HL_VPK_CHECKSUM_LENGTH)) != 0)
					{
						if(bCancel)
						{
							// User canceled.
							eValidation = HL_VALIDATES_CANCELED;
							break;
						}

						uiChecksum = CRC32(lpBuffer, uiBufferSize, uiChecksum);

						uiTotalBytes += static_cast<hlULongLong>(uiBufferSize);

						hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);
					}

					if(!bCancel)
					{
						const VPKDirectoryItem *pDirectoryItem = static_cast<const VPKDirectoryItem *>(pFile->GetData());
						if(pDirectoryItem->pDirectoryEntry->uiCRC != uiChecksum)
						{
							eValidation = HL_VALIDATES_CORRUPT;
						}
					}

					pStream->Close();
				}
				else
				{
					eValidation = HL_VALIDATES_ERROR;
				}

				this->ReleaseStreamInternal(*pStream);
				delete pStream;
			}
			else
			{
				eValidation = HL_VALIDATES_ERROR;
			}
		}
		else
		{
			eValidation = HL_VALIDATES_INCOMPLETE;
		}
	}
	else
	{
		eValidation = HL_VALIDATES_ERROR;
	}

	return hlTrue;
}

hlBool CVPKFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	const VPKDirectoryItem *pDirectoryItem = static_cast<const VPKDirectoryItem *>(pFile->GetData());

	uiSize = pDirectoryItem->pDirectoryEntry->uiEntryLength + pDirectoryItem->pDirectoryEntry->uiPreloadBytes;

	return hlTrue;
}

hlBool CVPKFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	const VPKDirectoryItem *pDirectoryItem = static_cast<const VPKDirectoryItem *>(pFile->GetData());

	uiSize = pDirectoryItem->pDirectoryEntry->uiEntryLength + pDirectoryItem->pDirectoryEntry->uiPreloadBytes;

	return hlTrue;
}

hlBool CVPKFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	const VPKDirectoryItem *pDirectoryItem = static_cast<const VPKDirectoryItem *>(pFile->GetData());

	if(pDirectoryItem->pDirectoryEntry->uiArchiveIndex == HL_VPK_NO_ARCHIVE)
	{
		if(pDirectoryItem->lpPreloadData != 0)
		{
			pStream = new Streams::CMemoryStream(const_cast<hlVoid *>(pDirectoryItem->lpPreloadData), pDirectoryItem->pDirectoryEntry->uiEntryLength + pDirectoryItem->pDirectoryEntry->uiPreloadBytes);
		}
		else if(pDirectoryItem->pDirectoryEntry->uiPreloadBytes == 0 && pDirectoryItem->pDirectoryEntry->uiEntryLength == 0)
		{
			pStream = new Streams::CNullStream();
		}
		else
		{
			return hlFalse;
		}
	}
	else if(pDirectoryItem->pDirectoryEntry->uiEntryLength != 0)
	{
		if(this->lpArchives[pDirectoryItem->pDirectoryEntry->uiArchiveIndex].pMapping != 0)
		{
			if(pDirectoryItem->pDirectoryEntry->uiPreloadBytes != 0)
			{
				Mapping::CView *pView = 0;
				if(!this->lpArchives[pDirectoryItem->pDirectoryEntry->uiArchiveIndex].pMapping->Map(pView, pDirectoryItem->pDirectoryEntry->uiEntryOffset, pDirectoryItem->pDirectoryEntry->uiEntryLength))
				{
					return hlFalse;
				}

				hlUInt uiBufferSize = pDirectoryItem->pDirectoryEntry->uiEntryLength + pDirectoryItem->pDirectoryEntry->uiPreloadBytes;
				hlByte *lpBuffer = new hlByte[uiBufferSize];

				memcpy(lpBuffer, pDirectoryItem->lpPreloadData, pDirectoryItem->pDirectoryEntry->uiPreloadBytes);
				memcpy(lpBuffer + pDirectoryItem->pDirectoryEntry->uiPreloadBytes, pView->GetView(), pDirectoryItem->pDirectoryEntry->uiEntryLength);

				this->lpArchives[pDirectoryItem->pDirectoryEntry->uiArchiveIndex].pMapping->Unmap(pView);

				pStream = new Streams::CMemoryStream(lpBuffer, uiBufferSize);
			}
			else
			{
				pStream = new Streams::CMappingStream(*this->lpArchives[pDirectoryItem->pDirectoryEntry->uiArchiveIndex].pMapping, pDirectoryItem->pDirectoryEntry->uiEntryOffset, pDirectoryItem->pDirectoryEntry->uiEntryLength);
			}
		}
		else
		{
			return hlFalse;
		}
	}
	else if(pDirectoryItem->pDirectoryEntry->uiPreloadBytes != 0)
	{
		pStream = new Streams::CMemoryStream(const_cast<hlVoid *>(pDirectoryItem->lpPreloadData), pDirectoryItem->pDirectoryEntry->uiPreloadBytes);
	}
	else
	{
		pStream = new Streams::CNullStream();
	}

	return hlTrue;
}

hlVoid CVPKFile::ReleaseStreamInternal(Streams::IStream &Stream) const
{
	if(Stream.GetType() == HL_STREAM_MEMORY)
	{
		hlByte *lpBuffer = (hlByte *)static_cast<Streams::CMemoryStream &>(Stream).GetBuffer();
		const hlByte *lpView = (const hlByte *)this->pView->GetView();
		if(lpBuffer < lpView || lpBuffer >= lpView + this->pView->GetLength())
		{
			delete []lpBuffer;
		}
	}
}
