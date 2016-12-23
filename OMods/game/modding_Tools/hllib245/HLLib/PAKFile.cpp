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
#include "PAKFile.h"
#include "Streams.h"

using namespace HLLib;

CPAKFile::CPAKFile() : CPackage(), pHeaderView(0), pDirectoryItemView(0), pHeader(0), lpDirectoryItems(0)
{

}

CPAKFile::~CPAKFile()
{
	this->Close();
}

HLPackageType CPAKFile::GetType() const
{
	return HL_PACKAGE_PAK;
}

const hlChar *CPAKFile::GetExtension() const
{
	return "pak";
}

const hlChar *CPAKFile::GetDescription() const
{
	return "Half-Life Package File";
}

hlBool CPAKFile::MapDataStructures()
{
	if(sizeof(PAKHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(PAKHeader)))
	{
		return hlFalse;
	}
	this->pHeader = (PAKHeader *)this->pHeaderView->GetView();

	if(memcmp(this->pHeader->lpSignature, "PACK", 4) != 0)
	{
		LastError.SetErrorMessage("Invalid file: the file's signature does not match.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pDirectoryItemView, this->pHeader->uiDirectoryOffset, this->pHeader->uiDirectoryLength))
	{
		return hlFalse;
	}
	this->lpDirectoryItems = (PAKDirectoryItem *)this->pDirectoryItemView->GetView();

	return hlTrue;
}

hlVoid CPAKFile::UnmapDataStructures()
{
	this->lpDirectoryItems = 0;
	this->pMapping->Unmap(this->pDirectoryItemView);

	this->pHeader = 0;
	this->pMapping->Unmap(this->pHeaderView);
}

CDirectoryFolder *CPAKFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder(this);

	hlUInt uiItemCount = this->pHeader->uiDirectoryLength / sizeof(PAKDirectoryItem);

	// Loop through each file in the PAK file.
	for(hlUInt i = 0; i < uiItemCount; i++)
	{
		hlChar lpFileName[56];
		strcpy(lpFileName, this->lpDirectoryItems[i].lpItemName);

		// Check if we have just a file, or if the file has directories we need to create.
		if(strchr(lpFileName, '/') == 0 && strchr(lpFileName, '\\') == 0)
		{
			pRoot->AddFile(lpFileName, i);
		}
		else
		{
			// Tokenize the file path and create the directories.
			CDirectoryFolder *pInsertFolder = pRoot;

			hlChar lpTemp[56] = "";
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
	}

	return pRoot;
}

hlBool CPAKFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpDirectoryItems[pFile->GetID()].uiItemLength;

	return hlTrue;
}

hlBool CPAKFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpDirectoryItems[pFile->GetID()].uiItemLength;

	return hlTrue;
}

hlBool CPAKFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	const PAKDirectoryItem *pDirectoryItem = this->lpDirectoryItems + pFile->GetID();

	pStream = new Streams::CMappingStream(*this->pMapping, pDirectoryItem->uiItemOffset, pDirectoryItem->uiItemLength);

	return hlTrue;
}
