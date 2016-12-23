/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef VBSPFILE_H
#define VBSPFILE_H

#include "stdafx.h"
#include "Package.h"

#define HL_VBSP_LUMP_COUNT 64

namespace HLLib
{
	class HLLIB_API CVBSPFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct VBSPLump
		{
			hlUInt	uiOffset;
			hlUInt	uiLength;
			hlUInt	uiVersion;							// Default to zero.
			hlChar	lpFourCC[4];						// Default to ( char )0, ( char )0, ( char )0, ( char )0.
		};

		struct VBSPHeader
		{
			hlChar		lpSignature[4];					// BSP file signature.
			hlInt		iVersion;						// BSP file version.
			VBSPLump	lpLumps[HL_VBSP_LUMP_COUNT];	// Lumps.
			hlInt		iMapRevision;					// The map's revision (iteration, version) number.
		};

		struct ZIPEndOfCentralDirectoryRecord
		{
			hlUInt uiSignature; // 4 bytes (0x06054b50)
			hlUInt16 uiNumberOfThisDisk;  // 2 bytes
			hlUInt16 uiNumberOfTheDiskWithStartOfCentralDirectory; // 2 bytes
			hlUInt16 uiCentralDirectoryEntriesThisDisk;	// 2 bytes
			hlUInt16 uiCentralDirectoryEntriesTotal;	// 2 bytes
			hlUInt uiCentralDirectorySize; // 4 bytes
			hlUInt uiStartOfCentralDirOffset; // 4 bytes
			hlUInt16 uiCommentLength; // 2 bytes
			// zip file comment follows
		};

		struct ZIPFileHeader
		{
			hlUInt uiSignature; //  4 bytes (0x02014b50) 
			hlUInt16 uiVersionMadeBy; // version made by 2 bytes 
			hlUInt16 uiVersionNeededToExtract; // version needed to extract 2 bytes 
			hlUInt16 uiFlags; // general purpose bit flag 2 bytes 
			hlUInt16 uiCompressionMethod; // compression method 2 bytes 
			hlUInt16 uiLastModifiedTime; // last mod file time 2 bytes 
			hlUInt16 uiLastModifiedDate; // last mod file date 2 bytes 
			hlUInt uiCRC32; // crc-32 4 bytes 
			hlUInt uiCompressedSize; // compressed size 4 bytes 
			hlUInt uiUncompressedSize; // uncompressed size 4 bytes 
			hlUInt16 uiFileNameLength; // file name length 2 bytes 
			hlUInt16 uiExtraFieldLength; // extra field length 2 bytes 
			hlUInt16 uiFileCommentLength; // file comment length 2 bytes 
			hlUInt16 uiDiskNumberStart; // disk number start 2 bytes 
			hlUInt16 uiInternalFileAttribs; // internal file attributes 2 bytes 
			hlUInt uiExternalFileAttribs; // external file attributes 4 bytes 
			hlUInt uiRelativeOffsetOfLocalHeader; // relative offset of local header 4 bytes 
			// file name (variable size) 
			// extra field (variable size) 
			// file comment (variable size) 
		};

		struct ZIPLocalFileHeader
		{
			hlUInt uiSignature; //local file header signature 4 bytes (0x04034b50) 
			hlUInt16 uiVersionNeededToExtract; // version needed to extract 2 bytes 
			hlUInt16 uiFlags; // general purpose bit flag 2 bytes 
			hlUInt16 uiCompressionMethod; // compression method 2 bytes 
			hlUInt16 uiLastModifiedTime; // last mod file time 2 bytes 
			hlUInt16 uiLastModifiedDate; // last mod file date 2 bytes 
			hlUInt uiCRC32; // crc-32 4 bytes 
			hlUInt uiCompressedSize; // compressed size 4 bytes 
			hlUInt uiUncompressedSize; // uncompressed size 4 bytes 
			hlUInt16 uiFileNameLength; // file name length 2 bytes 
			hlUInt16 uiExtraFieldLength; // extra field length 2 bytes 
			// file name (variable size) 
			// extra field (variable size) 
			// file data (variable size) 
		};

		struct LMPHeader
		{
			hlInt	iLumpOffset;
			hlInt	iLumpID;
			hlInt	iLumpVersion;	
			hlInt	iLumpLength;
			hlInt	iMapRevision;
		};

		#pragma pack()

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];

		Mapping::CView *pHeaderView;
		Mapping::CView *pFileHeaderView;
		Mapping::CView *pEndOfCentralDirectoryRecordView;

		const VBSPHeader *pHeader;
		const ZIPEndOfCentralDirectoryRecord *pEndOfCentralDirectoryRecord;

	public:
		CVBSPFile();
		virtual ~CVBSPFile();

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

		virtual hlBool GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const;
		virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;
		virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;

		virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;
		virtual hlVoid ReleaseStreamInternal(Streams::IStream &Stream) const;

	private:
		hlVoid GetFileName(hlChar *lpBuffer, hlUInt uiBufferSize);
	};
}

#endif
