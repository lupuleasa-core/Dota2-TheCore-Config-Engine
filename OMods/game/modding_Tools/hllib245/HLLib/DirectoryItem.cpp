/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "DirectoryItem.h"
#include "DirectoryFile.h"
#include "DirectoryFolder.h"

using namespace HLLib;

CDirectoryItem::CDirectoryItem(const hlChar *lpName, hlUInt uiID, hlVoid *pData, CPackage *pPackage, CDirectoryFolder *pParent) : uiID(uiID), pData(pData), pPackage(pPackage), pParent(pParent)
{
	this->lpName = new hlChar[strlen(lpName) + 1];
	strcpy(this->lpName, lpName);
}

CDirectoryItem::~CDirectoryItem()
{
	delete []this->lpName;
}

//
// GetName()
// Returns this directory item's name.
//
const hlChar *CDirectoryItem::GetName() const
{
	return this->lpName;
}

//
// GetID()
// Returns this directory item's ID.
//
hlUInt CDirectoryItem::GetID() const
{
	return this->uiID;
}

//
// GetData()
// Returns this directory item's data.
//
hlVoid *CDirectoryItem::GetData()
{
	return this->pData;
}

const hlVoid *CDirectoryItem::GetData() const
{
	return this->pData;
}

//
// GetPackage()
// Returns this directory item's package.
//
CPackage *CDirectoryItem::GetPackage()
{
	return this->pPackage;
}

const CPackage *CDirectoryItem::GetPackage() const
{
	return this->pPackage;
}

//
// GetParent()
// Returns this directory item's parent item.  If this
// directory item does not have a parent null is returned.
//
CDirectoryFolder *CDirectoryItem::GetParent()
{
	return this->pParent;
}

const CDirectoryFolder *CDirectoryItem::GetParent() const
{
	return this->pParent;
}

hlVoid CDirectoryItem::GetPath(hlChar *lpPath, hlUInt uiPathSize) const
{
	hlChar *lpTemp = new hlChar[uiPathSize];

	strncpy(lpPath, this->lpName, uiPathSize);
	lpPath[uiPathSize - 1] = '\0';

	const CDirectoryItem *pItem = this->pParent;
	while(pItem)
	{
		strcpy(lpTemp, lpPath);

		strncpy(lpPath, pItem->lpName, uiPathSize);
		lpPath[uiPathSize - 1] = '\0';

		strncat(lpPath, PATH_SEPARATOR_STRING, uiPathSize - strlen(lpPath) - 1);

		strncat(lpPath, lpTemp, uiPathSize - strlen(lpPath) - 1);

		pItem = pItem->pParent;
	}

	delete []lpTemp;
}
