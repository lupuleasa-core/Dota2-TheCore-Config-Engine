/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef GCFFILE_H
#define GCFFILE_H

#include "stdafx.h"
#include "Package.h"

namespace HLLib
{
	namespace Streams
	{
		class CGCFStream;
	}

	class HLLIB_API CGCFFile : public CPackage
	{
		friend class Streams::CGCFStream;

	private:
		#pragma pack(1)

		struct GCFHeader
		{
			hlUInt uiDummy0;		// Always 0x00000001
			hlUInt uiMajorVersion;	// Always 0x00000001
			hlUInt uiMinorVersion;	// GCF version number.
			hlUInt uiCacheID;
			hlUInt uiLastVersionPlayed;
			hlUInt uiDummy1;
			hlUInt uiDummy2;
			hlUInt uiFileSize;		// Total size of GCF file in bytes.
			hlUInt uiBlockSize;		// Size of each data block in bytes.
			hlUInt uiBlockCount;	// Number of data blocks.
			hlUInt uiDummy3;
		};

		struct GCFBlockEntryHeader
		{
			hlUInt uiBlockCount;	// Number of data blocks.
			hlUInt uiBlocksUsed;	// Number of data blocks that point to data.
			hlUInt uiDummy0;
			hlUInt uiDummy1;
			hlUInt uiDummy2;
			hlUInt uiDummy3;
			hlUInt uiDummy4;
			hlUInt uiChecksum;		// Header checksum.
		};

		struct GCFBlockEntry
		{
			hlUInt uiEntryFlags;				// Flags for the block entry.  0x200F0000 == Not used.
			hlUInt uiFileDataOffset;			// The offset for the data contained in this block entry in the file.
			hlUInt uiFileDataSize;				// The length of the data in this block entry.
			hlUInt uiFirstDataBlockIndex;		// The index to the first data block of this block entry's data.
			hlUInt uiNextBlockEntryIndex;		// The next block entry in the series.  (N/A if == BlockCount.)
			hlUInt uiPreviousBlockEntryIndex;	// The previous block entry in the series.  (N/A if == BlockCount.)
			hlUInt uiDirectoryIndex;			// The index of the block entry in the directory.
		};

		struct GCFFragmentationMapHeader
		{
			hlUInt uiBlockCount;		// Number of data blocks.
			hlUInt uiFirstUnusedEntry;	// The index of the first unused fragmentation map entry.
			hlUInt uiTerminator;		// The block entry terminator; 0 = 0x0000ffff or 1 = 0xffffffff.
			hlUInt uiChecksum;			// Header checksum.
		};

		struct GCFFragmentationMap
		{
			hlUInt uiNextDataBlockIndex;		// The index of the next data block.
		};

		// The below section is part of version 5 but not version 6.

		struct GCFBlockEntryMapHeader
		{
			hlUInt uiBlockCount;			// Number of data blocks.	
			hlUInt uiFirstBlockEntryIndex;	// Index of the first block entry.
			hlUInt uiLastBlockEntryIndex;	// Index of the last block entry.
			hlUInt uiDummy0;
			hlUInt uiChecksum;				// Header checksum.
		};

		struct GCFBlockEntryMap
		{
			hlUInt uiPreviousBlockEntryIndex;	// The previous block entry.  (N/A if == BlockCount.)
			hlUInt uiNextBlockEntryIndex;		// The next block entry.  (N/A if == BlockCount.)
		};

		// End section.

		struct GCFDirectoryHeader
		{
			hlUInt uiDummy0;				// Always 0x00000004
			hlUInt uiCacheID;				// Cache ID.
			hlUInt uiLastVersionPlayed;		// GCF file version.
			hlUInt uiItemCount;				// Number of items in the directory.	
			hlUInt uiFileCount;				// Number of files in the directory.
			hlUInt uiDummy1;				// Always 0x00008000.  Data per checksum?
			hlUInt uiDirectorySize;			// Size of lpGCFDirectoryEntries & lpGCFDirectoryNames & lpGCFDirectoryInfo1Entries & lpGCFDirectoryInfo2Entries & lpGCFDirectoryCopyEntries & lpGCFDirectoryLocalEntries in bytes.
			hlUInt uiNameSize;				// Size of the directory names in bytes.
			hlUInt uiInfo1Count;			// Number of Info1 entires.
			hlUInt uiCopyCount;				// Number of files to copy.
			hlUInt uiLocalCount;			// Number of files to keep local.
			hlUInt uiDummy2;
			hlUInt uiDummy3;
			hlUInt uiChecksum;				// Header checksum.
		};

		struct GCFDirectoryEntry
		{
			hlUInt uiNameOffset;		// Offset to the directory item name from the end of the directory items.
			hlUInt uiItemSize;			// Size of the item.  (If file, file size.  If folder, num items.)
			hlUInt uiChecksumIndex;		// Checksome index. (0xFFFFFFFF == None).
			hlUInt uiDirectoryFlags;	// Flags for the directory item.  (0x00000000 == Folder).
			hlUInt uiParentIndex;		// Index of the parent directory item.  (0xFFFFFFFF == None).
			hlUInt uiNextIndex;			// Index of the next directory item.  (0x00000000 == None).
			hlUInt uiFirstIndex;		// Index of the first directory item.  (0x00000000 == None).
		};

		struct GCFDirectoryInfo1Entry
		{
			hlUInt uiDummy0;
		};

		struct GCFDirectoryInfo2Entry
		{
			hlUInt uiDummy0;
		};

		struct GCFDirectoryCopyEntry
		{
			hlUInt uiDirectoryIndex;	// Index of the directory item.
		};

		struct GCFDirectoryLocalEntry
		{
			hlUInt uiDirectoryIndex;	// Index of the directory item.
		};

		// The below header was added in version 4 or version 5.

		struct GCFDirectoryMapHeader
		{
			hlUInt uiDummy0;			// Always 0x00000001
			hlUInt uiDummy1;			// Always 0x00000000
		};

		struct GCFDirectoryMapEntry
		{
			hlUInt uiFirstBlockIndex;	// Index of the first data block. (N/A if == BlockCount.)
		};

		struct GCFChecksumHeader
		{
			hlUInt uiDummy0;			// Always 0x00000001
			hlUInt uiChecksumSize;		// Size of LPGCFCHECKSUMHEADER & LPGCFCHECKSUMMAPHEADER & in bytes.
		};

		struct GCFChecksumMapHeader
		{
			hlUInt uiDummy0;			// Always 0x14893721
			hlUInt uiDummy1;			// Always 0x00000001
			hlUInt uiItemCount;			// Number of items.
			hlUInt uiChecksumCount;		// Number of checksums.
		};

		struct GCFChecksumMapEntry
		{
			hlUInt uiChecksumCount;			// Number of checksums.
			hlUInt uiFirstChecksumIndex;	// Index of first checksum.
		};

		struct GCFChecksumEntry
		{
			hlULong uiChecksum;				// Checksum.
		};

		struct GCFDataBlockHeader
		{
			hlUInt uiLastVersionPlayed;		// GCF file version.  This field is not part of all file versions.
			hlUInt uiBlockCount;			// Number of data blocks.
			hlUInt uiBlockSize;				// Size of each data block in bytes.
			hlUInt uiFirstBlockOffset;		// Offset to first data block.
			hlUInt uiBlocksUsed;			// Number of data blocks that contain data.
			hlUInt uiChecksum;				// Header checksum.
		};

		#pragma pack()

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];

		Mapping::CView *pHeaderView;

		GCFHeader *pHeader;

		GCFBlockEntryHeader *pBlockEntryHeader;
		GCFBlockEntry *lpBlockEntries;

		GCFFragmentationMapHeader *pFragmentationMapHeader;
		GCFFragmentationMap *lpFragmentationMap;

		// The below section is part of version 5 but not version 6.
		GCFBlockEntryMapHeader *pBlockEntryMapHeader;
		GCFBlockEntryMap *lpBlockEntryMap;

		GCFDirectoryHeader *pDirectoryHeader;
		GCFDirectoryEntry *lpDirectoryEntries;
		hlChar *lpDirectoryNames;
		GCFDirectoryInfo1Entry *lpDirectoryInfo1Entries;
		GCFDirectoryInfo2Entry *lpDirectoryInfo2Entries;
		GCFDirectoryCopyEntry *lpDirectoryCopyEntries;
		GCFDirectoryLocalEntry *lpDirectoryLocalEntries;

		GCFDirectoryMapHeader *pDirectoryMapHeader;
		GCFDirectoryMapEntry *lpDirectoryMapEntries;

		GCFChecksumHeader *pChecksumHeader;
		GCFChecksumMapHeader *pChecksumMapHeader;
		GCFChecksumMapEntry *lpChecksumMapEntries;
		GCFChecksumEntry *lpChecksumEntries;

		GCFDataBlockHeader *pDataBlockHeader;

		CDirectoryItem **lpDirectoryItems;

	public:
		CGCFFile();
		virtual ~CGCFFile();

		virtual HLPackageType GetType() const;
		virtual const hlChar *GetExtension() const;
		virtual const hlChar *GetDescription() const;

	protected:
		virtual hlBool MapDataStructures();
		virtual hlVoid UnmapDataStructures();

		virtual hlBool DefragmentInternal();

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

		hlVoid GetItemFragmentation(hlUInt uiDirectoryItemIndex, hlUInt &uiBlocksFragmented, hlUInt &uiBlocksUsed) const;
	};
}

#endif
