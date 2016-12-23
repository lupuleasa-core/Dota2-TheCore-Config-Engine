/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef DIRECTORYITEM_H
#define DIRECTORYITEM_H

#include "stdafx.h"

namespace HLLib
{
	class CPackage;
	class CDirectoryFolder;

	class HLLIB_API CDirectoryItem
	{
	private:
		hlChar *lpName;
		hlUInt uiID;
		hlVoid *pData;
		CPackage *pPackage;
		CDirectoryFolder *pParent;

	public:
		CDirectoryItem(const hlChar *lpName, hlUInt uiID, hlVoid *pData, CPackage *pPackage, CDirectoryFolder *pParent);
		virtual ~CDirectoryItem();

		virtual HLDirectoryItemType GetType() const = 0;

		const hlChar *GetName() const;
		hlUInt GetID() const;
		hlVoid *GetData();
		const hlVoid *GetData() const;

		CPackage *GetPackage();
		const CPackage *GetPackage() const;
		CDirectoryFolder *GetParent();
		const CDirectoryFolder *GetParent() const;

		hlVoid GetPath(hlChar *lpPath, hlUInt uiPathSize) const;

		virtual hlBool Extract(const hlChar *lpPath) const = 0;
	};
}

#endif
