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
#include "NCFFile.h"
#include "Streams.h"
#include "Checksum.h"
#include "Utility.h"

using namespace HLLib;

#define HL_NCF_FLAG_FILE						0x00004000	// The item is a file.
#define HL_NCF_FLAG_ENCRYPTED					0x00000100	// The item is encrypted.
#define HL_NCF_FLAG_BACKUP_LOCAL				0x00000040	// Backup the item before overwriting it.
#define HL_NCF_FLAG_COPY_LOCAL					0x0000000a	// The item is to be copied to the disk.
#define HL_NCF_FLAG_COPY_LOCAL_NO_OVERWRITE 	0x00000001	// Don't overwrite the item if copying it to the disk and the item already exists.

const char *CNCFFile::lpAttributeNames[] = { "Version", "Cache ID", "Last Version Played" };
const char *CNCFFile::lpItemAttributeNames[] = { "Encrypted", "Copy Locally", "Overwrite Local Copy", "Backup Local Copy", "Flags" };

CNCFFile::CNCFFile() : CPackage(), lpRootPath(0), pHeaderView(0)
{
	this->pHeader = 0;

	this->pDirectoryHeader = 0;
	this->lpDirectoryEntries = 0;
	this->lpDirectoryNames = 0;
	this->lpDirectoryInfo1Entries = 0;
	this->lpDirectoryInfo2Entries = 0;
	this->lpDirectoryCopyEntries = 0;
	this->lpDirectoryLocalEntries = 0;

	this->pUnknownHeader = 0;
	this->lpUnknownEntries = 0;

	this->pChecksumHeader = 0;
	this->pChecksumMapHeader = 0;
	this->lpChecksumMapEntries = 0;
	this->lpChecksumEntries = 0;
}

CNCFFile::~CNCFFile()
{
	this->Close();
}

HLPackageType CNCFFile::GetType() const
{
	return HL_PACKAGE_NCF;
}

const hlChar *CNCFFile::GetExtension() const
{
	return "ncf";
}

const hlChar *CNCFFile::GetDescription() const
{
	return "Half-Life No Cache File";
}

const hlChar *CNCFFile::GetRootPath() const
{
	return this->lpRootPath;
}

hlVoid CNCFFile::SetRootPath(const hlChar *lpRootPath)
{
	if(!this->GetOpened())
	{
		return;
	}

	delete []this->lpRootPath;
	this->lpRootPath = 0;

	if(lpRootPath == 0 || *lpRootPath == '\0')
	{
		return;
	}

	this->lpRootPath = new hlChar[strlen(lpRootPath) + 1];
	strcpy(this->lpRootPath, lpRootPath);
}

hlBool CNCFFile::MapDataStructures()
{
	//
	// Determine the size of the header and validate it.
	//

	hlUInt uiHeaderSize = 0;

	// Header.

	if(sizeof(NCFHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(NCFHeader)))
	{
		return hlFalse;
	}
	this->pHeader = (NCFHeader *)this->pHeaderView->GetView();

	hlBool bNull = hlTrue;
	for(hlByte *pTest = (hlByte *)this->pHeader; pTest <  (hlByte *)this->pHeader + sizeof(NCFHeader); pTest++)
	{
		if(*pTest != 0)
		{
			bNull = hlFalse;
			break;
		}
	}

	if(bNull)
	{
		LastError.SetErrorMessage("Invalid file: the file's header is null (contains no data).");
		return hlFalse;
	}

	if(this->pHeader->uiMajorVersion != 2 || this->pHeader->uiMinorVersion != 1)
	{
		LastError.SetErrorMessageFormated("Invalid NCF version (v%u.%u): you have a version of a NCF file that HLLib does not know how to read. Check for product updates.", this->pHeader->uiMajorVersion, this->pHeader->uiMinorVersion);
		return hlFalse;
	}

	uiHeaderSize += sizeof(NCFHeader);

	// Directory.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(NCFDirectoryHeader)))
	{
		return hlFalse;
	}
	this->pDirectoryHeader = (NCFDirectoryHeader *)this->pHeaderView->GetView();

	uiHeaderSize += this->pDirectoryHeader->uiDirectorySize;/*sizeof(NCFDirectoryHeader);
				 + this->pDirectoryHeader->uiItemCount * sizeof(NCFDirectoryEntry)
				 + this->pDirectoryHeader->uiNameSize
				 + this->pDirectoryHeader->uiInfo1Count * sizeof(NCFDirectoryInfo1Entry)
				 + this->pDirectoryHeader->uiItemCount * sizeof(NCFDirectoryInfo2Entry)
				 + this->pDirectoryHeader->uiCopyCount * sizeof(NCFDirectoryCopyEntry)
				 + this->pDirectoryHeader->uiLocalCount * sizeof(NCFDirectoryLocalEntry);*/

	uiHeaderSize += sizeof(NCFUnknownHeader);

	uiHeaderSize += this->pDirectoryHeader->uiItemCount * sizeof(NCFUnknownEntry);

	// Checksums.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(NCFChecksumHeader)))
	{
		return hlFalse;
	}
	this->pChecksumHeader = (NCFChecksumHeader *)this->pHeaderView->GetView();

	uiHeaderSize += sizeof(NCFChecksumHeader) + this->pChecksumHeader->uiChecksumSize;

	//
	// Map the header.
	//

	if(!this->pMapping->Map(this->pHeaderView, 0, uiHeaderSize))
	{
		return hlFalse;
	}

	this->pHeader = (NCFHeader *)this->pHeaderView->GetView();

	this->pDirectoryHeader = (NCFDirectoryHeader *)((hlByte *)this->pHeader + sizeof(NCFHeader));

	this->lpDirectoryEntries = (NCFDirectoryEntry *)((hlByte *)this->pDirectoryHeader + sizeof(NCFDirectoryHeader));

	this->lpDirectoryNames = (hlChar *)((hlByte *)this->lpDirectoryEntries + sizeof(NCFDirectoryEntry) * this->pDirectoryHeader->uiItemCount);

	this->lpDirectoryInfo1Entries = (NCFDirectoryInfo1Entry *)((hlByte *)this->lpDirectoryNames + this->pDirectoryHeader->uiNameSize);
	this->lpDirectoryInfo2Entries = (NCFDirectoryInfo2Entry *)((hlByte *)this->lpDirectoryInfo1Entries + sizeof(NCFDirectoryInfo1Entry) * this->pDirectoryHeader->uiInfo1Count);

	this->lpDirectoryCopyEntries = (NCFDirectoryCopyEntry *)((hlByte *)this->lpDirectoryInfo2Entries + sizeof(NCFDirectoryInfo2Entry) * this->pDirectoryHeader->uiItemCount);
	this->lpDirectoryLocalEntries = (NCFDirectoryLocalEntry *)((hlByte *)this->lpDirectoryCopyEntries + sizeof(NCFDirectoryCopyEntry) * this->pDirectoryHeader->uiCopyCount);

	this->pUnknownHeader = (NCFUnknownHeader *)((hlByte *)this->pDirectoryHeader + this->pDirectoryHeader->uiDirectorySize);
	this->lpUnknownEntries = (NCFUnknownEntry *)((hlByte *)this->pUnknownHeader + sizeof(NCFUnknownHeader));

	this->pChecksumHeader = (NCFChecksumHeader *)((hlByte *)this->lpUnknownEntries + this->pDirectoryHeader->uiItemCount * sizeof(NCFUnknownEntry));
	this->pChecksumMapHeader = (NCFChecksumMapHeader *)((hlByte *)(this->pChecksumHeader) + sizeof(NCFChecksumHeader));

	this->lpChecksumMapEntries = (NCFChecksumMapEntry *)((hlByte *)(this->pChecksumMapHeader) + sizeof(NCFChecksumMapHeader));
	this->lpChecksumEntries = (NCFChecksumEntry *)((hlByte *)(this->lpChecksumMapEntries) + sizeof(NCFChecksumMapEntry) * this->pChecksumMapHeader->uiItemCount);

	return hlTrue;
}

hlVoid CNCFFile::UnmapDataStructures()
{
	delete []this->lpRootPath;
	this->lpRootPath = 0;

	this->pHeader = 0;

	this->pDirectoryHeader = 0;
	this->lpDirectoryEntries = 0;
	this->lpDirectoryNames = 0;
	this->lpDirectoryInfo1Entries = 0;
	this->lpDirectoryInfo2Entries = 0;
	this->lpDirectoryCopyEntries = 0;
	this->lpDirectoryLocalEntries = 0;

	this->pUnknownHeader = 0;
	this->lpUnknownEntries = 0;

	this->pChecksumHeader = 0;
	this->pChecksumMapHeader = 0;
	this->lpChecksumMapEntries = 0;
	this->lpChecksumEntries = 0;

	this->pMapping->Unmap(this->pHeaderView);
}

CDirectoryFolder *CNCFFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder("root", 0, 0, this, 0);

	this->CreateRoot(pRoot);

	return pRoot;
}

hlVoid CNCFFile::CreateRoot(CDirectoryFolder *pFolder)
{
	// Get the first directory item.
	hlUInt uiIndex = this->lpDirectoryEntries[pFolder->GetID()].uiFirstIndex;

	// Loop through directory items.
	while(uiIndex && uiIndex != 0xffffffff)
	{
		// Check if the item is a folder.
		if((this->lpDirectoryEntries[uiIndex].uiDirectoryFlags & HL_NCF_FLAG_FILE) == 0)
		{
			// Add the directory item to the current folder.
			CDirectoryFolder *pSubFolder = pFolder->AddFolder(this->lpDirectoryNames + this->lpDirectoryEntries[uiIndex].uiNameOffset, uiIndex);

			// Build the new folder.
			this->CreateRoot(pSubFolder);
		}
		else
		{
			// Add the directory item to the current folder.
			pFolder->AddFile(this->lpDirectoryNames + this->lpDirectoryEntries[uiIndex].uiNameOffset, uiIndex);
		}

		// Get the next directory item.
		uiIndex = this->lpDirectoryEntries[uiIndex].uiNextIndex;
	}
}

hlUInt CNCFFile::GetAttributeCountInternal() const
{
	return HL_NCF_PACKAGE_COUNT;
}

const hlChar *CNCFFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_NCF_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CNCFFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_NCF_PACKAGE_VERSION:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiMinorVersion, hlFalse);
		return hlTrue;
	case HL_NCF_PACKAGE_ID:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiCacheID, hlFalse);
		return hlTrue;
	case HL_NCF_PACKAGE_LAST_VERSION_PLAYED:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiLastVersionPlayed, hlFalse);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CNCFFile::GetItemAttributeCountInternal() const
{
	return HL_NCF_ITEM_COUNT;
}

const hlChar *CNCFFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_NCF_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CNCFFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			switch(eAttribute)
			{
				case HL_NCF_ITEM_ENCRYPTED:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_NCF_FLAG_ENCRYPTED) != 0);
					return hlTrue;
				}
				case HL_NCF_ITEM_COPY_LOCAL:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_NCF_FLAG_COPY_LOCAL) != 0);
					return hlTrue;
				}
				case HL_NCF_ITEM_OVERWRITE_LOCAL:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_NCF_FLAG_COPY_LOCAL_NO_OVERWRITE) == 0);
					return hlTrue;
				}
				case HL_NCF_ITEM_BACKUP_LOCAL:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_NCF_FLAG_BACKUP_LOCAL) != 0);
					return hlTrue;
				}
				case HL_NCF_ITEM_FLAGS:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags, hlTrue);
					return hlTrue;
				}
			}
			break;
		}
		case HL_ITEM_FOLDER:
		{
			const CDirectoryFolder *pFolder = static_cast<const CDirectoryFolder *>(pItem);
			switch(eAttribute)
			{
				case HL_NCF_ITEM_FLAGS:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], this->lpDirectoryEntries[pFolder->GetID()].uiDirectoryFlags, hlTrue);
					return hlTrue;
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CNCFFile::GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const
{
	bExtractable = hlFalse;

	if(this->lpRootPath != 0)
	{
		hlChar lpTemp[512];
		this->GetPath(pFile, lpTemp, sizeof(lpTemp));

		hlUInt uiSize;
		if(HLLib::GetFileSize(lpTemp, uiSize))
		{
			if(uiSize >= this->lpDirectoryEntries[pFile->GetID()].uiItemSize)
			{
				bExtractable = hlTrue;
			}
		}
		else
		{
			if(this->lpDirectoryEntries[pFile->GetID()].uiItemSize == 0)
			{
				bExtractable = hlTrue;
			}
		}
	}

	return hlTrue;
}

hlBool CNCFFile::GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const
{
	if(this->lpRootPath != 0)
	{
		hlChar lpTemp[512];
		this->GetPath(pFile, lpTemp, sizeof(lpTemp));

		hlUInt uiSize;
		if(HLLib::GetFileSize(lpTemp, uiSize))
		{
			if(uiSize < this->lpDirectoryEntries[pFile->GetID()].uiItemSize)
			{
				eValidation = HL_VALIDATES_INCOMPLETE;
			}
			else if(this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_NCF_FLAG_ENCRYPTED)
			{
				// No way of checking, assume it's ok.
				eValidation = HL_VALIDATES_ASSUMED_OK;
			}
			else if(this->lpDirectoryEntries[pFile->GetID()].uiChecksumIndex == 0xffffffff)
			{
				// File has no checksum.
				eValidation = HL_VALIDATES_ASSUMED_OK;
			}
			else
			{
				Streams::CFileStream Stream = Streams::CFileStream(lpTemp);

				if(Stream.Open(HL_MODE_READ))
				{
					eValidation = HL_VALIDATES_OK;

					hlULongLong uiTotalBytes = 0, uiFileBytes = Stream.GetStreamSize();
					hlUInt uiBufferSize;
					hlByte *lpBuffer = new hlByte[this->pDirectoryHeader->uiChecksumDataLength];

					const NCFChecksumMapEntry *pChecksumMapEntry = this->lpChecksumMapEntries + this->lpDirectoryEntries[pFile->GetID()].uiChecksumIndex;

					hlBool bCancel = hlFalse;
					hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);

					hlUInt i = 0;
					while((uiBufferSize = Stream.Read(lpBuffer, this->pDirectoryHeader->uiChecksumDataLength)) != 0)
					{
						if(bCancel)
						{
							// User canceled.
							eValidation = HL_VALIDATES_CANCELED;
							break;
						}

						if(i >= pChecksumMapEntry->uiChecksumCount)
						{
							// Something bad happened.
							eValidation = HL_VALIDATES_ERROR;
							break;
						}

						hlULong uiChecksum = Adler32(lpBuffer, uiBufferSize) ^ CRC32(lpBuffer, uiBufferSize);
						if(uiChecksum != this->lpChecksumEntries[pChecksumMapEntry->uiFirstChecksumIndex + i].uiChecksum)
						{
							eValidation = HL_VALIDATES_CORRUPT;
							break;
						}

						uiTotalBytes += static_cast<hlULongLong>(uiBufferSize);

						hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);

						i++;
					}

					delete []lpBuffer;

					Stream.Close();
				}
				else
				{
					eValidation = HL_VALIDATES_ERROR;
				}
			}
		}
		else
		{
			// Not found.
			if(this->lpDirectoryEntries[pFile->GetID()].uiItemSize != 0)
			{
				eValidation = HL_VALIDATES_INCOMPLETE;
			}
			else
			{
				eValidation = HL_VALIDATES_OK;
			}
		}
	}
	else
	{
		eValidation = HL_VALIDATES_ASSUMED_OK;
	}

	return hlTrue;
}

hlBool CNCFFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpDirectoryEntries[pFile->GetID()].uiItemSize;

	return hlTrue;
}

hlBool CNCFFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = 0;

	if(this->lpRootPath != 0)
	{
		hlChar lpTemp[512];
		this->GetPath(pFile, lpTemp, sizeof(lpTemp));

		HLLib::GetFileSize(lpTemp, uiSize);
	}

	return hlTrue;
}

hlBool CNCFFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	if(!bReadEncrypted && this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_NCF_FLAG_ENCRYPTED)
	{
		LastError.SetErrorMessage("File is encrypted.");
		return hlFalse;
	}

	if(this->lpRootPath != 0)
	{
		hlChar lpTemp[512];
		this->GetPath(pFile, lpTemp, sizeof(lpTemp));

		hlUInt uiSize;
		if(HLLib::GetFileSize(lpTemp, uiSize))
		{
			if(uiSize >= this->lpDirectoryEntries[pFile->GetID()].uiItemSize)
			{
				pStream = new Streams::CFileStream(lpTemp);
				return hlTrue;
			}
			else
			{
				LastError.SetErrorMessage("File is incomplete.");
				return hlFalse;
			}
		}
		else
		{
			if(this->lpDirectoryEntries[pFile->GetID()].uiItemSize == 0)
			{
				// Fake an empty stream.
				pStream = new Streams::CNullStream();
				return hlTrue;
			}
			else
			{
				LastError.SetErrorMessage("File not found.");
				return hlFalse;
			}
		}
	}
	else
	{
		LastError.SetErrorMessage("NCF files are indexes and do not contain any file data.");
		return hlFalse;
	}
}

hlVoid CNCFFile::GetPath(const CDirectoryFile *pFile, hlChar *lpPath, hlUInt uiPathSize) const
{
	hlChar *lpTemp = new hlChar[uiPathSize];

	strncpy(lpPath, pFile->GetName(), uiPathSize);
	lpPath[uiPathSize - 1] = '\0';

	const CDirectoryItem *pItem = pFile->GetParent();
	while(pItem)
	{
		strcpy(lpTemp, lpPath);

		if(pItem->GetParent() == 0)
		{
			strncpy(lpPath, this->lpRootPath, uiPathSize);
		}
		else
		{
			strncpy(lpPath, pItem->GetName(), uiPathSize);
		}
		lpPath[uiPathSize - 1] = '\0';

		strncat(lpPath, PATH_SEPARATOR_STRING, uiPathSize - strlen(lpPath) - 1);

		strncat(lpPath, lpTemp, uiPathSize - strlen(lpPath) - 1);

		pItem = pItem->GetParent();
	}

	delete []lpTemp;
}
