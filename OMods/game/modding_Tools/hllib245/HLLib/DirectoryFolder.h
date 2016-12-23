/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef DIRECTORYFOLDER_H
#define DIRECTORYFOLDER_H

#include "DirectoryItem.h"
#include "DirectoryFile.h"

#include <vector>

namespace HLLib
{
	class HLLIB_API CDirectoryFolder : public CDirectoryItem
	{
	private:
		typedef std::vector<CDirectoryItem *> CDirectoryItemVector;

	private:
		CDirectoryItemVector *pDirectoryItemVector;

	public:
		CDirectoryFolder(CPackage *pPackage);
		CDirectoryFolder(const hlChar *lpName, hlUInt uiID, hlVoid *pData, CPackage *pPackage, CDirectoryFolder *pParent);
		virtual ~CDirectoryFolder();

		virtual HLDirectoryItemType GetType() const;

		CDirectoryFolder *AddFolder(const hlChar *lpName, hlUInt uiID = HL_ID_INVALID, hlVoid *lpData = 0);
		CDirectoryFile *AddFile(const hlChar *lpName, hlUInt uiID = HL_ID_INVALID, hlVoid *lpData = 0);

		hlUInt GetCount() const;
		CDirectoryItem *GetItem(hlUInt uiIndex);
		const CDirectoryItem *GetItem(hlUInt uiIndex) const;
		CDirectoryItem *GetItem(const hlChar *lpName, HLFindType eFind = HL_FIND_ALL);
		const CDirectoryItem *GetItem(const hlChar *lpName, HLFindType eFind = HL_FIND_ALL) const;
		CDirectoryItem *GetRelativeItem(const hlChar *lpPath, HLFindType eFind = HL_FIND_ALL);
		const CDirectoryItem *GetRelativeItem(const hlChar *lpPath, HLFindType eFind = HL_FIND_ALL) const;

		hlVoid Sort(HLSortField eField = HL_FIELD_NAME, HLSortOrder eOrder = HL_ORDER_ASCENDING, hlBool bRecurse = hlTrue);

		CDirectoryItem *FindFirst(const hlChar *lpSearch, HLFindType eFind = HL_FIND_ALL);
		const CDirectoryItem *FindFirst(const hlChar *lpSearch, HLFindType eFind = HL_FIND_ALL) const;
		CDirectoryItem *FindNext(const CDirectoryItem *pItem, const hlChar *lpSearch, HLFindType eFind = HL_FIND_ALL);
		const CDirectoryItem *FindNext(const CDirectoryItem *pItem, const hlChar *lpSearch, HLFindType eFind = HL_FIND_ALL) const;

		hlUInt GetSize(hlBool bRecurse = hlTrue) const;
		hlULongLong GetSizeEx(hlBool bRecurse = hlTrue) const;
		hlUInt GetSizeOnDisk(hlBool bRecurse = hlTrue) const;
		hlULongLong GetSizeOnDiskEx(hlBool bRecurse = hlTrue) const;
		hlUInt GetFolderCount(hlBool bRecurse = hlTrue) const;
		hlUInt GetFileCount(hlBool bRecurse = hlTrue) const;

		virtual hlBool Extract(const hlChar *lpPath) const;

	private:
		hlInt Compare(const hlChar *lpString0, const hlChar *lpString1, HLFindType eFind) const;
		hlBool Match(const hlChar *lpString, const hlChar *lpSearch, HLFindType eFind) const;
		const CDirectoryItem *FindNext(const CDirectoryFolder *pFolder, const CDirectoryItem *pRelative, const hlChar *lpSearch, HLFindType eFind = HL_FIND_ALL) const;
	};
}

#endif
