/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef PAKFILE_H
#define PAKFILE_H

#include "stdafx.h"
#include "Package.h"

namespace HLLib
{
	class HLLIB_API CPAKFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct PAKHeader
		{
			hlChar lpSignature[4];
			hlUInt uiDirectoryOffset;
			hlUInt uiDirectoryLength;
		};

		struct PAKDirectoryItem
		{
			hlChar lpItemName[56];
			hlUInt uiItemOffset;
			hlUInt uiItemLength;
		};

		#pragma pack()

	private:
		Mapping::CView *pHeaderView;
		Mapping::CView *pDirectoryItemView;

		const PAKHeader *pHeader;
		const PAKDirectoryItem *lpDirectoryItems;

	public:
		CPAKFile();
		virtual ~CPAKFile();

		virtual HLPackageType GetType() const;
		virtual const hlChar *GetExtension() const;
		virtual const hlChar *GetDescription() const;

	protected:
		virtual hlBool MapDataStructures();
		virtual hlVoid UnmapDataStructures();

		virtual CDirectoryFolder *CreateRoot();

		virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;
		virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;

		virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;
	};
}

#endif
