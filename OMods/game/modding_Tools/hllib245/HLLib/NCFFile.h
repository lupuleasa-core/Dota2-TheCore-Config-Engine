/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef NCFFILE_H
#define NCFFILE_H

#include "stdafx.h"
#include "Package.h"

namespace HLLib
{
	class HLLIB_API CNCFFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct NCFHeader
		{
			hlUInt uiDummy0;			// Always 0x00000001
			hlUInt uiMajorVersion;		// Always 0x00000002
			hlUInt uiMinorVersion;		// NCF version number.
			hlUInt uiCacheID;
			hlUInt uiLastVersionPlayed;
			hlUInt uiDummy3;
			hlUInt uiDummy4;
			hlUInt uiFileSize;		// Total size of NCF file in bytes.
			hlUInt uiBlockSize;		// Size of each data block in bytes.
			hlUInt uiBlockCount;	// Number of data blocks.
			hlUInt uiDummy5;
		};

		struct NCFDirectoryHeader
		{
			hlUInt uiDummy0;				// Always 0x00000004
			hlUInt uiCacheID;				// Cache ID.
			hlUInt uiLastVersionPlayed;		// NCF file version.
			hlUInt uiItemCount;				// Number of items in the directory.	
			hlUInt uiFileCount;				// Number of files in the directory.
			hlUInt uiChecksumDataLength;	// Always 0x00008000.  Data per checksum?
			hlUInt uiDirectorySize;			// Size of lpNCFDirectoryEntries & lpNCFDirectoryNames & lpNCFDirectoryInfo1Entries & lpNCFDirectoryInfo2Entries & lpNCFDirectoryCopyEntries & lpNCFDirectoryLocalEntries in bytes.
			hlUInt uiNameSize;				// Size of the directory names in bytes.
			hlUInt uiInfo1Count;			// Number of Info1 entires.
			hlUInt uiCopyCount;				// Number of files to copy.
			hlUInt uiLocalCount;			// Number of files to keep local.
			hlUInt uiDummy1;
			hlUInt uiDummy2;
			hlUInt uiChecksum;				// Header checksum.
		};

		struct NCFDirectoryEntry
		{
			hlUInt uiNameOffset;		// Offset to the directory item name from the end of the directory items.
			hlUInt uiItemSize;			// Size of the item.  (If file, file size.  If folder, num items.)
			hlUInt uiChecksumIndex;		// Checksome index. (0xFFFFFFFF == None).
			hlUInt uiDirectoryFlags;	// Flags for the directory item.  (0x00000000 == Folder).
			hlUInt uiParentIndex;		// Index of the parent directory item.  (0xFFFFFFFF == None).
			hlUInt uiNextIndex;			// Index of the next directory item.  (0x00000000 == None).
			hlUInt uiFirstIndex;		// Index of the first directory item.  (0x00000000 == None).
		};

		struct NCFDirectoryInfo1Entry
		{
			hlUInt uiDummy0;
		};

		struct NCFDirectoryInfo2Entry
		{
			hlUInt uiDummy0;
		};

		struct NCFDirectoryCopyEntry
		{
			hlUInt uiDirectoryIndex;	// Index of the directory item.
		};

		struct NCFDirectoryLocalEntry
		{
			hlUInt uiDirectoryIndex;	// Index of the directory item.
		};

		struct NCFUnknownHeader
		{
			hlUInt uiDummy0;			// Always 0x00000001
			hlUInt uiDummy1;			// Always 0x00000000
		};

		struct NCFUnknownEntry
		{
			hlUInt uiDummy0;
		};

		struct NCFChecksumHeader
		{
			hlUInt uiDummy0;			// Always 0x00000001
			hlUInt uiChecksumSize;		// Size of LPNCFCHECKSUMHEADER & LPNCFCHECKSUMMAPHEADER & in bytes.
		};

		struct NCFChecksumMapHeader
		{
			hlUInt uiDummy0;			// Always 0x14893721
			hlUInt uiDummy1;			// Always 0x00000001
			hlUInt uiItemCount;			// Number of items.
			hlUInt uiChecksumCount;		// Number of checksums.
		};

		struct NCFChecksumMapEntry
		{
			hlUInt uiChecksumCount;			// Number of checksums.
			hlUInt uiFirstChecksumIndex;	// Index of first checksum.
		};

		struct NCFChecksumEntry
		{
			hlULong uiChecksum;				// Checksum.
		};

		#pragma pack()

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];

		hlChar *lpRootPath;

		Mapping::CView *pHeaderView;

		NCFHeader *pHeader;

		NCFDirectoryHeader *pDirectoryHeader;
		NCFDirectoryEntry *lpDirectoryEntries;
		hlChar *lpDirectoryNames;
		NCFDirectoryInfo1Entry *lpDirectoryInfo1Entries;
		NCFDirectoryInfo2Entry *lpDirectoryInfo2Entries;
		NCFDirectoryCopyEntry *lpDirectoryCopyEntries;
		NCFDirectoryLocalEntry *lpDirectoryLocalEntries;

		NCFUnknownHeader *pUnknownHeader;
		NCFUnknownEntry *lpUnknownEntries;

		NCFChecksumHeader *pChecksumHeader;
		NCFChecksumMapHeader *pChecksumMapHeader;
		NCFChecksumMapEntry *lpChecksumMapEntries;
		NCFChecksumEntry *lpChecksumEntries;

	public:
		CNCFFile();
		virtual ~CNCFFile();

		virtual HLPackageType GetType() const;
		virtual const hlChar *GetExtension() const;
		virtual const hlChar *GetDescription() const;

		const hlChar *GetRootPath() const;
		hlVoid SetRootPath(const hlChar *lpRootPath);

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

		virtual hlBool GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const;
		virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;
		virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;

		virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;

	private:
		hlVoid CreateRoot(CDirectoryFolder *pFolder);

		hlVoid GetPath(const CDirectoryFile *pFile, hlChar *lpPath, hlUInt uiPathSize) const;
	};
}

#endif
