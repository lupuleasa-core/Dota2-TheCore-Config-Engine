/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef DIRECTORYFILE_H
#define DIRECTORYFILE_H

#include "DirectoryItem.h"
#include "Stream.h"

namespace HLLib
{
	class HLLIB_API CDirectoryFile : public CDirectoryItem
	{
	public:
		CDirectoryFile(const hlChar *lpName, hlUInt uiID, hlVoid *pData, CPackage *pPackage, CDirectoryFolder *pParent);
		virtual ~CDirectoryFile();

		virtual HLDirectoryItemType GetType() const;

		hlBool GetExtractable() const;
		hlBool GetExtractable(hlBool &bExtractable) const;
		HLValidation GetValidation() const;
		hlBool GetValidation(HLValidation &eValidation) const;
		hlUInt GetSize() const;
		hlBool GetSize(hlUInt &uiSize) const;
		hlUInt GetSizeOnDisk() const;
		hlBool GetSizeOnDisk(hlUInt &uiSize) const;

		hlBool CreateStream(Streams::IStream *&pStream) const;
		hlVoid ReleaseStream(Streams::IStream *pStream) const;

		virtual hlBool Extract(const hlChar *lpPath) const;
	};
}

#endif
