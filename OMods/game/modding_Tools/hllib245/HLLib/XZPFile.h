/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef XZPFILE_H
#define XZPFILE_H

#include "stdafx.h"
#include "Package.h"

namespace HLLib
{
	class HLLIB_API CXZPFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct XZPHeader
		{
			hlChar lpSignature[4];
			hlUInt uiVersion;
			hlUInt uiPreloadDirectoryEntryCount;
			hlUInt uiDirectoryEntryCount;
			hlUInt uiPreloadBytes;
			hlUInt uiHeaderLength;
			hlUInt uiDirectoryItemCount;
			hlUInt uiDirectoryItemOffset;
			hlUInt uiDirectoryItemLength;
		};

		struct XZPDirectoryEntry
		{
			hlUInt uiFileNameCRC;
			hlUInt uiEntryLength;
			hlUInt uiEntryOffset;
		};

		struct XZPDirectoryMapping
		{
			hlUInt16 uiPreloadDirectoryEntryIndex;
		};

		struct XZPDirectoryItem
		{
			hlUInt uiFileNameCRC;
			hlUInt uiNameOffset;
			hlUInt uiTimeCreated;
		};

		struct XZPFooter
		{
			hlUInt uiFileLength;
			hlChar lpSignature[4];
		};

		#pragma pack()

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];

		Mapping::CView *pHeaderView;
		Mapping::CView *pDirectoryEntryView;
		Mapping::CView *pDirectoryItemView;
		Mapping::CView *pFooterView;

		const XZPHeader *pHeader;
		const XZPDirectoryEntry *lpDirectoryEntries;
		const XZPDirectoryEntry *lpPreloadDirectoryEntries;
		const XZPDirectoryMapping *lpPreloadDirectoryMappings;
		const XZPDirectoryItem *lpDirectoryItems;
		const XZPFooter *pFooter;

	public:
		CXZPFile();
		virtual ~CXZPFile();

		virtual HLPackageType GetType() const;
		virtual const hlChar *GetExtension() const;
		virtual const hlChar *GetDescription() const;

	protected:
		virtual hlBool MapDataStructures();
		virtual hlVoid UnmapDataStructures();

		virtual CDirectoryFolder *CreateRoot();

		virtual hlUInt GetAttributeCountInternal() const;
		virtual const hlChar *GetAttributeNameInternal(HLPackageAttribute eAttribute) const;
		virtual hlBool GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

		virtual hlUInt GetItemAttributeCountInternal() const;
		virtual const hlChar *GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const;
		virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

		virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;
		virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;

		virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;
	};
}

#endif
