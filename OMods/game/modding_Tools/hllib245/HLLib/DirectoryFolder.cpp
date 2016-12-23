/*
 * HLLib
 * Copyright (C) 2006-2012 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "DirectoryFile.h"
#include "DirectoryFolder.h"
#include "HLLib.h"
#include "Utility.h"

#include <algorithm>

using namespace HLLib;

CDirectoryFolder::CDirectoryFolder(CPackage *pPackage) : CDirectoryItem("root", HL_ID_INVALID, 0, pPackage, 0), pDirectoryItemVector(new CDirectoryItemVector())
{

}

CDirectoryFolder::CDirectoryFolder(const hlChar *lpName, hlUInt uiID, hlVoid *pData, CPackage *pPackage, CDirectoryFolder *pParent) : CDirectoryItem(lpName, uiID, pData, pPackage, pParent), pDirectoryItemVector(new CDirectoryItemVector())
{

}

CDirectoryFolder::~CDirectoryFolder()
{
	// Delete children.
	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		delete (*this->pDirectoryItemVector)[i];
	}

	delete this->pDirectoryItemVector;
}

HLDirectoryItemType CDirectoryFolder::GetType() const
{
	return HL_ITEM_FOLDER;
}

CDirectoryFolder *CDirectoryFolder::AddFolder(const hlChar *lpName, hlUInt uiID, hlVoid *lpData)
{
	CDirectoryFolder *pFolder = new CDirectoryFolder(lpName, uiID, lpData, this->GetPackage(), this);

	this->pDirectoryItemVector->push_back(pFolder);

	return pFolder;
}

CDirectoryFile *CDirectoryFolder::AddFile(const hlChar *lpName, hlUInt uiID, hlVoid *lpData)
{
	CDirectoryFile *pFile = new CDirectoryFile(lpName, uiID, lpData, this->GetPackage(), this);

	this->pDirectoryItemVector->push_back(pFile);

	return pFile;
}

//
// GetCount()
// Returns the number of directory items in this folder.
//
hlUInt CDirectoryFolder::GetCount() const
{
	return (hlUInt)this->pDirectoryItemVector->size();
}

//
// GetItem()
// Returns the directory item at index uiIndex.
//
CDirectoryItem *CDirectoryFolder::GetItem(hlUInt uiIndex)
{
	if(uiIndex >= (hlUInt)this->pDirectoryItemVector->size())
	{
		return 0;
	}

	return (*this->pDirectoryItemVector)[uiIndex];
}

const CDirectoryItem *CDirectoryFolder::GetItem(hlUInt uiIndex) const
{
	if(uiIndex >= (hlUInt)this->pDirectoryItemVector->size())
	{
		return 0;
	}

	return (*this->pDirectoryItemVector)[uiIndex];
}

//
// GetItem()
// Returns the directory item lpName.  If the directory item
// does not exist null is returned.
//
CDirectoryItem *CDirectoryFolder::GetItem(const hlChar *lpName, HLFindType eFind)
{
	return const_cast<CDirectoryItem *>(const_cast<const CDirectoryFolder*>(this)->GetItem(lpName, eFind));
}

const CDirectoryItem *CDirectoryFolder::GetItem(const hlChar *lpName, HLFindType eFind) const
{
	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		if((pItem->GetType() == HL_ITEM_FILE && (eFind & HL_FIND_FILES)) || (pItem->GetType() == HL_ITEM_FOLDER && (eFind & HL_FIND_FOLDERS)))
		{
			if(this->Compare(lpName, pItem->GetName(), eFind) == 0)
			{
				return pItem;
			}
		}
	}

	return 0;
}

//
// GetRelativeItem()
// Returns the directory item lpPath.  If the directory item
// does not exist null is returned.  Transverses sub-folders too.
//
CDirectoryItem *CDirectoryFolder::GetRelativeItem(const hlChar *lpPath, HLFindType eFind)
{
	return const_cast<CDirectoryItem *>(const_cast<const CDirectoryFolder*>(this)->GetRelativeItem(lpPath, eFind));
}

const CDirectoryItem *CDirectoryFolder::GetRelativeItem(const hlChar *lpPath, HLFindType eFind) const
{
	const CDirectoryFolder *pFolder = this;

	hlChar *lpTemp = new hlChar[strlen(lpPath) + 1];
	strcpy(lpTemp, lpPath);

	hlChar *lpToken = strtok(lpTemp, "\\/");
	if(lpToken != 0 && this->Compare(pFolder->GetName(), lpToken, eFind) == 0)
	{
		lpToken = strtok(0, "\\/");
	}

	while(lpToken != 0)
	{
		if(*lpToken == '\0' || strcmp(lpToken, ".") == 0)
		{
			lpToken = strtok(0, "\\/");
		}
		else if(strcmp(lpToken, "..") == 0)
		{
			if(pFolder->GetParent())
			{
				pFolder = pFolder->GetParent();
			}
			else
			{
				delete []lpTemp;
				return 0;
			}

			lpToken = strtok(0, "\\/");
		}
		else
		{
			hlChar *lpNext = strtok(0, "\\/");

			const CDirectoryItem *pItem = 0;

			for(hlUInt i = 0; i < pFolder->GetCount(); i++)
			{
				const CDirectoryItem *pTemp = pFolder->GetItem(i);
				if(lpNext == 0 && pTemp->GetType() == HL_ITEM_FILE && (eFind & HL_FIND_FILES) && this->Compare(lpToken, pTemp->GetName(), eFind) == 0)
				{
					pItem = pTemp;
					break;
				}
				else if(pTemp->GetType() == HL_ITEM_FOLDER && this->Compare(lpToken, pTemp->GetName(), eFind) == 0)
				{
					pItem = pTemp;
					break;
				}
			}

			if(pItem == 0)
			{
				delete []lpTemp;
				return 0;
			}

			if(pItem->GetType() == HL_ITEM_FOLDER)
			{
				pFolder = static_cast<const CDirectoryFolder *>(pItem);
			}
			else
			{
				delete []lpTemp;
				return pItem;
			}

			lpToken = lpNext;
		}
	}

	delete []lpTemp;
	if(eFind & HL_FIND_FOLDERS)
	{
		return pFolder;
	}
	else
	{
		return 0;
	}
}

class CCompareDirectoryItems
{
private:
	HLSortField eField;
	HLSortOrder eOrder;

public:
	CCompareDirectoryItems(HLSortField eField, HLSortOrder eOrder) : eField(eField), eOrder(eOrder)
	{

	}

	bool operator()(const CDirectoryItem * const &pItem0, const CDirectoryItem * const &pItem1)
	{
		HLDirectoryItemType eType0 = pItem0->GetType();
		HLDirectoryItemType eType1 = pItem1->GetType();

		if(eType0 == HL_ITEM_FOLDER && eType1 == HL_ITEM_FILE)
		{
			return true;
		}
		else if(eType0 == HL_ITEM_FILE && eType1 == HL_ITEM_FOLDER)
		{
			return false;
		}

		hlInt iResult;

		switch(eField)
		{
			case HL_FIELD_SIZE:
			{
				hlUInt uiSize0 = eType0 == HL_ITEM_FILE ? static_cast<const CDirectoryFile *>(pItem0)->GetSize() : static_cast<const CDirectoryFolder *>(pItem0)->GetCount();
				hlUInt uiSize1 = eType1 == HL_ITEM_FILE ? static_cast<const CDirectoryFile *>(pItem1)->GetSize() : static_cast<const CDirectoryFolder *>(pItem1)->GetCount();

				iResult = (hlInt)uiSize0 - (hlInt)uiSize1;

				if(iResult != 0)
				{
					break;
				}
			}
			//case HL_FIELD_NAME:
			default:
			{
				iResult = stricmp(pItem0->GetName(), pItem1->GetName());
				break;
			}
		}

		if(eOrder == HL_ORDER_DESCENDING)
		{
			iResult *= -1;
		}

		return iResult < 0;
	}
};

hlVoid CDirectoryFolder::Sort(HLSortField eField, HLSortOrder eOrder, hlBool bRecurse)
{
	std::sort(this->pDirectoryItemVector->begin(), this->pDirectoryItemVector->end(), CCompareDirectoryItems(eField, eOrder));

	if(bRecurse)
	{
		for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
		{
			CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
			if(pItem->GetType() == HL_ITEM_FOLDER)
			{
				static_cast<CDirectoryFolder *>(pItem)->Sort(eField, eOrder, bRecurse);
			}
		}
	}
}

CDirectoryItem *CDirectoryFolder::FindFirst(const hlChar *lpSearch, HLFindType eFind)
{
	return const_cast<CDirectoryItem *>(const_cast<const CDirectoryFolder*>(this)->FindFirst(lpSearch, eFind));
}

const CDirectoryItem *CDirectoryFolder::FindFirst(const hlChar *lpSearch, HLFindType eFind) const
{
	return this->FindNext(this, 0, lpSearch, eFind);
}

CDirectoryItem *CDirectoryFolder::FindNext(const CDirectoryItem *pItem, const hlChar *lpSearch, HLFindType eFind)
{
	return const_cast<CDirectoryItem *>(const_cast<const CDirectoryFolder*>(this)->FindNext(pItem, lpSearch, eFind));
}

const CDirectoryItem *CDirectoryFolder::FindNext(const CDirectoryItem *pItem, const hlChar *lpSearch, HLFindType eFind) const
{
	if(pItem == 0)
	{
		return 0;
	}

	if(pItem->GetType() == HL_ITEM_FOLDER && !(eFind & HL_FIND_NO_RECURSE))
	{
		return this->FindNext(static_cast<const CDirectoryFolder *>(pItem), 0, lpSearch, eFind);
	}
	else
	{
		return this->FindNext(pItem->GetParent(), pItem, lpSearch, eFind);
	}
}

const CDirectoryItem *CDirectoryFolder::FindNext(const CDirectoryFolder *pFolder, const CDirectoryItem *pRelative, const hlChar *lpSearch, HLFindType eFind) const
{
	hlUInt uiFirst = 0;

	if(pRelative)
	{
		for(hlUInt i = 0; i < pFolder->GetCount(); i++)
		{
			if(pFolder->GetItem(i) == pRelative)
			{
				uiFirst = i + 1;
				break;
			}
		}
	}

	for(hlUInt i = uiFirst; i < pFolder->GetCount(); i++)
	{
		const CDirectoryItem *pTest = pFolder->GetItem(i);
		if((pTest->GetType() == HL_ITEM_FILE && (eFind & HL_FIND_FILES)) || (pTest->GetType() == HL_ITEM_FOLDER && (eFind & HL_FIND_FOLDERS)))
		{
			if(this->Match(pTest->GetName(), lpSearch, eFind))
			{
				return pTest;
			}
		}

		if(pTest->GetType() == HL_ITEM_FOLDER && !(eFind & HL_FIND_NO_RECURSE))
		{
			pTest = this->FindNext(static_cast<const CDirectoryFolder *>(pTest), 0, lpSearch, eFind);

			if(pTest != 0)
			{
				return pTest;
			}
		}
	}

	if(this == pFolder || pRelative == 0 || pFolder->GetParent() == 0)
	{
		return 0;
	}

	return this->FindNext(pFolder->GetParent(), pFolder, lpSearch, eFind);
}

hlInt CDirectoryFolder::Compare(const hlChar *lpString0, const hlChar *lpString1, HLFindType eFind) const
{
	if(eFind & HL_FIND_CASE_SENSITIVE)
	{
		return strcmp(lpString0, lpString1);
	}
	else
	{
		return stricmp(lpString0, lpString1);
	}
}

//
// Match a string to a seach string.  Search string can contain wild cards like * (to match
// a substring) and ? (to match a letter).
//
hlBool CDirectoryFolder::Match(const hlChar *lpString, const hlChar *lpSearch, HLFindType eFind) const
{
	if(eFind & HL_FIND_MODE_STRING)
	{
		return this->Compare(lpString, lpSearch, eFind);
	}
	else if(eFind & HL_FIND_MODE_SUBSTRING)
	{
		hlInt iStringLength = (hlInt)strlen(lpString);
		hlInt iSearchLength = (hlInt)strlen(lpSearch);
		hlInt iTests = iStringLength - iSearchLength;

		if(eFind & HL_FIND_CASE_SENSITIVE)
		{
			for(hlInt i = 0; i <= iTests; i++)
			{
				if(strncmp(lpString + i, lpSearch, iSearchLength) == 0)
				{
					return hlTrue;
				}
			}
		}
		else
		{
			for(hlInt i = 0; i <= iTests; i++)
			{
				if(_strnicmp(lpString + i, lpSearch, iSearchLength) == 0)
				{
					return hlTrue;
				}
			}
		}

		return hlFalse;
	}
	else /*if(eFind & HL_FIND_MODE_WILDCARD)*/
	{
		while(*lpSearch)
		{
			if(*lpSearch == '*')
			{
				if(lpSearch[1] == '*')
				{
					lpSearch++;
					continue;
				}
				else if(lpSearch[1] == '\0')
				{
					return hlTrue;
				}
				else
				{
					lpSearch++;
					while(*lpString)
					{
						if(this->Match(lpString, lpSearch, eFind))
						{
							return hlTrue;
						}
						lpString++;
					}

					return hlFalse;
				}
			}
			else if(*lpSearch == '?')
			{
				if(*lpString == '\0')
				{
					return hlFalse;
				}
				lpSearch++;
				lpString++;
			}
			else
			{
				if(*lpString == '\0')
				{
					return hlFalse;
				}
				else
				{
					hlChar iA = *lpSearch;
					hlChar iB = *lpString;

					if((eFind & HL_FIND_CASE_SENSITIVE) == 0)
					{
						if(iA >= 'a' && iA <= 'z')
						{
							iA -= 'a' - 'A';
						}

						if(iB >= 'a' && iB <= 'z')
						{
							iB -= 'a' - 'A';
						}
					}

					if(iA != iB)
					{
						return hlFalse;
					}
				}
				lpSearch++;
				lpString++;
			}
		}

		return *lpString == '\0';
	}
}

hlUInt CDirectoryFolder::GetSize(hlBool bRecurse) const
{
	hlUInt uiSize = 0;

	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		switch(pItem->GetType())
		{
		case HL_ITEM_FOLDER:
			if(bRecurse)
			{
				uiSize += static_cast<const CDirectoryFolder *>(pItem)->GetSize(bRecurse);
			}
			break;
		case HL_ITEM_FILE:
			uiSize += static_cast<const CDirectoryFile *>(pItem)->GetSize();
			break;
		}
	}

	return uiSize;
}

hlULongLong CDirectoryFolder::GetSizeEx(hlBool bRecurse) const
{
	hlULongLong uiSize = 0;

	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		switch(pItem->GetType())
		{
		case HL_ITEM_FOLDER:
			if(bRecurse)
			{
				uiSize += static_cast<const CDirectoryFolder *>(pItem)->GetSizeEx(bRecurse);
			}
			break;
		case HL_ITEM_FILE:
			uiSize += static_cast<hlULongLong>(static_cast<const CDirectoryFile *>(pItem)->GetSize());
			break;
		}
	}

	return uiSize;
}

hlUInt CDirectoryFolder::GetSizeOnDisk(hlBool bRecurse) const
{
	hlUInt uiSize = 0;

	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		switch(pItem->GetType())
		{
		case HL_ITEM_FOLDER:
			if(bRecurse)
			{
				uiSize += static_cast<const CDirectoryFolder *>(pItem)->GetSizeOnDisk(bRecurse);
			}
			break;
		case HL_ITEM_FILE:
			uiSize += static_cast<const CDirectoryFile *>(pItem)->GetSizeOnDisk();
			break;
		}
	}

	return uiSize;
}

hlULongLong CDirectoryFolder::GetSizeOnDiskEx(hlBool bRecurse) const
{
	hlULongLong uiSize = 0;

	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		switch(pItem->GetType())
		{
		case HL_ITEM_FOLDER:
			if(bRecurse)
			{
				uiSize += static_cast<const CDirectoryFolder *>(pItem)->GetSizeOnDiskEx(bRecurse);
			}
			break;
		case HL_ITEM_FILE:
			uiSize += static_cast<hlULongLong>(static_cast<const CDirectoryFile *>(pItem)->GetSizeOnDisk());
			break;
		}
	}

	return uiSize;
}

hlUInt CDirectoryFolder::GetFolderCount(hlBool bRecurse) const
{
	hlUInt uiCount = 0;

	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		switch(pItem->GetType())
		{
		case HL_ITEM_FOLDER:
			uiCount++;
			if(bRecurse)
			{
				uiCount += static_cast<const CDirectoryFolder *>(pItem)->GetFolderCount(bRecurse);
			}
			break;
		}
	}

	return uiCount;
}

hlUInt CDirectoryFolder::GetFileCount(hlBool bRecurse) const
{
	hlUInt uiCount = 0;

	for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
	{
		const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
		switch(pItem->GetType())
		{
		case HL_ITEM_FOLDER:
			if(bRecurse)
			{
				uiCount += static_cast<const CDirectoryFolder *>(pItem)->GetFileCount(bRecurse);
			}
			break;
		case HL_ITEM_FILE:
			uiCount++;
			break;
		}
	}

	return uiCount;
}
 
hlBool CDirectoryFolder::Extract(const hlChar *lpPath) const
{
	hlExtractItemStart(this);

	hlChar *lpName = new hlChar[strlen(this->GetName()) + 1];
	strcpy(lpName, this->GetName());
	RemoveIllegalCharacters(lpName);

	hlChar *lpFolderName;
	if(lpPath == 0 || *lpPath == '\0')
	{
		lpFolderName = new hlChar[strlen(lpName) + 1];
		strcpy(lpFolderName, lpName);
	}
	else
	{
		lpFolderName = new hlChar[strlen(lpPath) + 1 + strlen(lpName) + 1];
		strcpy(lpFolderName, lpPath);
		strcat(lpFolderName, PATH_SEPARATOR_STRING);
		strcat(lpFolderName, lpName);
	}

	FixupIllegalCharacters(lpFolderName);

	hlBool bResult;
	if(!CreateFolder(lpFolderName))
	{
		LastError.SetSystemErrorMessage("CreateDirectory() failed.");

		bResult = hlFalse;
	}
	else
	{
		bResult = hlTrue;

		for(hlUInt i = 0; i < this->pDirectoryItemVector->size(); i++)
		{
			const CDirectoryItem *pItem = (*this->pDirectoryItemVector)[i];
			if(pItem->GetType() != HL_ITEM_FILE || static_cast<const CDirectoryFile *>(pItem)->GetExtractable())
			{
				bResult &= pItem->Extract(lpFolderName);
			}
		}
	}

	delete []lpFolderName;
	delete []lpName;

	hlExtractItemEnd(this, bResult);

	return bResult;
}
