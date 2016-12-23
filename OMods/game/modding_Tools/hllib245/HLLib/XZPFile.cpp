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
#include "XZPFile.h"
#include "Streams.h"

using namespace HLLib;

const char *CXZPFile::lpAttributeNames[] = { "Version", "Preload Bytes" };
const char *CXZPFile::lpItemAttributeNames[] = { "Created", "Preload Bytes" };

CXZPFile::CXZPFile() : CPackage(), pHeaderView(0), pDirectoryEntryView(0), pDirectoryItemView(0), pFooterView(0), pHeader(0), lpDirectoryEntries(0), lpPreloadDirectoryEntries(0), lpPreloadDirectoryMappings(0), lpDirectoryItems(0), pFooter(0)
{

}

CXZPFile::~CXZPFile()
{
	this->Close();
}

HLPackageType CXZPFile::GetType() const
{
	return HL_PACKAGE_XZP;
}

const hlChar *CXZPFile::GetExtension() const
{
	return "xzp";
}

const hlChar *CXZPFile::GetDescription() const
{
	return "XBox Package File";
}

hlBool CXZPFile::MapDataStructures()
{
	if(sizeof(XZPHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(XZPHeader)))
	{
		return hlFalse;
	}
	this->pHeader = (XZPHeader *)this->pHeaderView->GetView();

	if(memcmp(this->pHeader->lpSignature, "piZx", 4) != 0)
	{
		LastError.SetErrorMessage("Invalid file: the file's header signature does not match.");
		return hlFalse;
	}

	if(this->pHeader->uiVersion != 6)
	{
		LastError.SetErrorMessageFormated("Invalid XZP version (v%u): you have a version of a XZP file that HLLib does not know how to read. Check for product updates.", this->pHeader->uiVersion);
		return hlFalse;
	}

	if(this->pHeader->uiHeaderLength != sizeof(XZPHeader))
	{
		LastError.SetErrorMessage("Invalid file: the file's header size does not match.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pDirectoryEntryView, sizeof(XZPHeader), this->pHeader->uiPreloadBytes ? (this->pHeader->uiDirectoryEntryCount + this->pHeader->uiPreloadDirectoryEntryCount) * sizeof(XZPDirectoryEntry) + this->pHeader->uiDirectoryEntryCount * sizeof(XZPDirectoryMapping) : this->pHeader->uiDirectoryEntryCount * sizeof(XZPDirectoryEntry)))
	{
		return hlFalse;
	}
	this->lpDirectoryEntries = (XZPDirectoryEntry *)this->pDirectoryEntryView->GetView();
	this->lpPreloadDirectoryEntries = this->pHeader->uiPreloadBytes ? this->lpDirectoryEntries + this->pHeader->uiDirectoryEntryCount : 0;
	this->lpPreloadDirectoryMappings = this->pHeader->uiPreloadBytes ? (XZPDirectoryMapping *)(this->lpPreloadDirectoryEntries + this->pHeader->uiPreloadDirectoryEntryCount) : 0;

	if(this->pHeader->uiDirectoryItemCount != 0)
	{
		if(!this->pMapping->Map(this->pDirectoryItemView, this->pHeader->uiDirectoryItemOffset, this->pHeader->uiDirectoryItemLength))
		{
			return hlFalse;
		}
		this->lpDirectoryItems = (XZPDirectoryItem *)this->pDirectoryItemView->GetView();
	}

	if(!this->pMapping->Map(this->pFooterView, this->pMapping->GetMappingSize() - sizeof(XZPFooter), sizeof(XZPFooter)))
	{
		return hlFalse;
	}
	this->pFooter = (XZPFooter *)this->pFooterView->GetView();

	if(memcmp(this->pFooter->lpSignature, "tFzX", 4) != 0)
	{
		LastError.SetErrorMessage("Invalid file: the file's footer signature does not match.");
		return hlFalse;
	}

	if(this->pFooter->uiFileLength != this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is not within mapping bounds.");
		return hlFalse;
	}

	return hlTrue;
}

hlVoid CXZPFile::UnmapDataStructures()
{
	this->pFooter = 0;
	this->pMapping->Unmap(this->pFooterView);

	this->lpDirectoryItems = 0;
	this->pMapping->Unmap(this->pDirectoryItemView);

	this->lpDirectoryEntries = 0;
	this->lpPreloadDirectoryEntries = 0;
	this->lpPreloadDirectoryMappings = 0;
	this->pMapping->Unmap(this->pDirectoryEntryView);

	this->pHeader = 0;
	this->pMapping->Unmap(this->pHeaderView);
}

CDirectoryFolder *CXZPFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder(this);

	if(this->pHeader->uiDirectoryItemCount != 0)
	{
		// Loop through each file in the XZP file.
		for(hlUInt i = 0; i < this->pHeader->uiDirectoryEntryCount; i++)
		{
			// Find it's info (file name).
			for(hlUInt j = 0; j < this->pHeader->uiDirectoryItemCount; j++)
			{
				if(this->lpDirectoryEntries[i].uiFileNameCRC == this->lpDirectoryItems[j].uiFileNameCRC)
				{
					hlChar lpFileName[256];
					strncpy(lpFileName, (hlChar *)this->lpDirectoryItems + this->lpDirectoryItems[j].uiNameOffset - this->pHeader->uiDirectoryItemOffset, sizeof(lpFileName));
					lpFileName[sizeof(lpFileName) - 1] = '\0';

					// Check if we have just a file, or if the file has directories we need to create.
					if(strchr(lpFileName, '/') == 0 && strchr(lpFileName, '\\') == 0)
					{
						pRoot->AddFile(lpFileName, i);
					}
					else
					{
						// Tokenize the file path and create the directories.
						CDirectoryFolder *pInsertFolder = pRoot;

						hlChar lpTemp[256] = "";
						hlChar *lpToken = strtok(lpFileName, "/\\");
						while(lpToken != 0)
						{
							strcpy(lpTemp, lpToken);

							lpToken = strtok(0, "/\\");

							if(lpToken != 0)
							{
								// Check if the directory exists.
								CDirectoryItem *pItem = pInsertFolder->GetItem(lpTemp);
								if(pItem == 0 || pItem->GetType() == HL_ITEM_FILE)
								{
									// It doesn't, create it.
									pInsertFolder = pInsertFolder->AddFolder(lpTemp);
								}
								else
								{
									// It does, use it.
									pInsertFolder = static_cast<CDirectoryFolder *>(pItem);
								}
							}
						}

						// The file name is the last token, add it.
						pInsertFolder->AddFile(lpTemp, i);
					}
					break;
				}
			}
		}
	}
	else
	{
		// No file name information, just file name CRCs.
		const hlChar lpLookup[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		for(hlUInt i = 0; i < this->pHeader->uiDirectoryEntryCount; i++)
		{
			hlChar lpTemp[16];

			hlChar* lpName = lpTemp;
			for(hlByte *lpCRC = (hlByte *)&this->lpDirectoryEntries[i].uiFileNameCRC; lpCRC < (hlByte *)&this->lpDirectoryEntries[i].uiFileNameCRC + sizeof(hlUInt); lpCRC++)
			{
				*lpName++ = lpLookup[(hlByte)(*lpCRC >> 4)];
				*lpName++ = lpLookup[(hlByte)(*lpCRC & 0x0F)];
			}
			*lpName = '\0';

			pRoot->AddFile(lpTemp, i);
		}
	}

	return pRoot;
}

hlUInt CXZPFile::GetAttributeCountInternal() const
{
	return HL_XZP_PACKAGE_COUNT;
}

const hlChar *CXZPFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_XZP_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CXZPFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_XZP_PACKAGE_VERSION:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiVersion, hlFalse);
		return hlTrue;
	case HL_XZP_PACKAGE_PRELOAD_BYTES:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiPreloadBytes, hlFalse);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CXZPFile::GetItemAttributeCountInternal() const
{
	return HL_XZP_ITEM_COUNT;
}

const hlChar *CXZPFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_XZP_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CXZPFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			const XZPDirectoryEntry *pDirectoryEntry = this->lpDirectoryEntries + pFile->GetID();
			switch(eAttribute)
			{
				case HL_XZP_ITEM_CREATED:
				{
					for(hlUInt i = 0; i < this->pHeader->uiDirectoryItemCount; i++)
					{
						if(this->lpDirectoryItems[i].uiFileNameCRC == pDirectoryEntry->uiFileNameCRC)
						{
							time_t Time = (time_t)this->lpDirectoryItems[i].uiTimeCreated;
							tm *pTime = localtime(&Time);

							hlChar lpTime[128];
							strftime(lpTime, sizeof(lpTime), "%c", pTime);

							hlAttributeSetString(&Attribute, this->lpItemAttributeNames[eAttribute], lpTime);
							return hlTrue;
						}
					}
					break;
				}
				case HL_XZP_ITEM_PRELOAD_BYTES:
				{
					hlUInt uiSize = 0;
					if(this->lpPreloadDirectoryMappings != 0)
					{
						hlUInt16 uiIndex = this->lpPreloadDirectoryMappings[pFile->GetID()].uiPreloadDirectoryEntryIndex;
						if(uiIndex != 0xffff && this->lpPreloadDirectoryEntries[uiIndex].uiFileNameCRC == pDirectoryEntry->uiFileNameCRC)
						{
							uiSize = this->lpPreloadDirectoryEntries[uiIndex].uiEntryLength;
						}
					}
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiSize, hlFalse);
					return hlTrue;
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CXZPFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpDirectoryEntries[pFile->GetID()].uiEntryLength;

	return hlTrue;
}

hlBool CXZPFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpDirectoryEntries[pFile->GetID()].uiEntryLength;

	return hlTrue;
}

hlBool CXZPFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	const XZPDirectoryEntry *pDirectoryEntry = this->lpDirectoryEntries + pFile->GetID();

	pStream = new Streams::CMappingStream(*this->pMapping, pDirectoryEntry->uiEntryOffset, pDirectoryEntry->uiEntryLength);

	return hlTrue;
}
