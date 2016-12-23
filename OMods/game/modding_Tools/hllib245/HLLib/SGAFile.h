/*
 * HLLib
 * Copyright (C) 2006-2012 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef SGAFILE_H
#define SGAFILE_H

#include "stdafx.h"
#include "Package.h"

namespace HLLib
{
	class HLLIB_API CSGAFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct SGAHeaderBase
		{
			hlChar lpSignature[8];
			hlUShort uiMajorVersion;
			hlUShort uiMinorVersion;
		};

		struct SGAHeader4 : public SGAHeaderBase
		{
			hlByte lpFileMD5[16];
			hlWChar lpName[64];
			hlByte lpHeaderMD5[16];
			hlUInt uiHeaderLength;
			hlUInt uiFileDataOffset;
			hlUInt uiDummy0;
		};

		struct SGAHeader6 : public SGAHeaderBase
		{
			hlWChar lpName[64];
			hlUInt uiHeaderLength;
			hlUInt uiFileDataOffset;
			hlUInt uiDummy0;
		};

		template<typename T>
		struct SGADirectoryHeader
		{
			hlUInt uiSectionOffset;
			T uiSectionCount;
			hlUInt uiFolderOffset;
			T uiFolderCount;
			hlUInt uiFileOffset;
			T uiFileCount;
			hlUInt uiStringTableOffset;
			T uiStringTableCount;
		};

		typedef SGADirectoryHeader<hlUShort> SGADirectoryHeader4;
		typedef SGADirectoryHeader<hlUInt> SGADirectoryHeader5;

		struct SGADirectoryHeader7 : public SGADirectoryHeader5
		{
			hlUInt uiHashTableOffset;
			hlUInt uiBlockSize;
		};

		template<typename T>
		struct SGASection
		{
			hlChar lpAlias[64];
			hlChar lpName[64];
			T uiFolderStartIndex;
			T uiFolderEndIndex;
			T uiFileStartIndex;
			T uiFileEndIndex;
			T uiFolderRootIndex;
		};

		typedef SGASection<hlUShort> SGASection4;
		typedef SGASection<hlUInt> SGASection5;

		template<typename T>
		struct SGAFolder
		{
			hlUInt uiNameOffset;
			T uiFolderStartIndex;
			T uiFolderEndIndex;
			T uiFileStartIndex;
			T uiFileEndIndex;
		};

		typedef SGAFolder<hlUShort> SGAFolder4;
		typedef SGAFolder<hlUInt> SGAFolder5;

		struct SGAFile4
		{
			hlUInt uiNameOffset;
			hlUInt uiOffset;
			hlUInt uiSizeOnDisk;
			hlUInt uiSize;
			hlUInt uiTimeModified;
			hlByte uiDummy0;
			hlByte uiType;
		};

		struct SGAFile6 : public SGAFile4
		{
			hlUInt uiCRC32;
		};

		struct SGAFile7 : public SGAFile6
		{
			hlUInt uiHashOffset;
		};

		struct SGAFileHeader
		{
			hlChar lpName[256];
			hlUInt uiCRC32;
		};

		enum SGAFileVerification
		{
			VERIFICATION_NONE,
			VERIFICATION_CRC,
			VERIFICATION_CRC_BLOCKS,
			VERIFICATION_MD5_BLOCKS,
			VERIFICATION_SHA1_BLOCKS,
			VERIFICATION_COUNT,
		};

		#pragma pack()

		class ISGADirectory
		{
		public:
			virtual ~ISGADirectory() = 0;

		public:
			virtual hlBool MapDataStructures() = 0;
			virtual hlVoid UnmapDataStructures() = 0;

			virtual CDirectoryFolder *CreateRoot() = 0;

			virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const = 0;

			virtual hlBool GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const = 0;
			virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const = 0;
			virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const = 0;
			virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const = 0;

			virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const = 0;
			virtual hlVoid ReleaseStreamInternal(Streams::IStream &Stream) const = 0;
		};

		// Specialization SGAFile7 and up where the CRC moved to the header and the CRC is of the compressed data and there are stronger hashes.
		template<typename TSGAHeader, typename TSGADirectoryHeader, typename TSGASection, typename TSGAFolder, typename TSGAFile>
		class CSGASpecializedDirectory : public ISGADirectory
		{
		public:
			typedef typename TSGAHeader SGAHeader;
			typedef typename TSGADirectoryHeader SGADirectoryHeader;
			typedef typename TSGASection SGASection;
			typedef typename TSGAFolder SGAFolder;
			typedef typename TSGAFile SGAFile;

			CSGASpecializedDirectory(CSGAFile& File);

		protected:
			CSGAFile& File;

			Mapping::CView *pHeaderDirectoryView;
			const SGADirectoryHeader *pDirectoryHeader;
			const SGASection *lpSections;
			const SGAFolder *lpFolders;
			const SGAFile *lpFiles;
			const hlChar *lpStringTable;

		public:
			virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

			virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		};

		// Specialization SGAFile4 where the CRC was stored in a SGAFileHeader located before the file data.
		template<typename TSGAHeader, typename TSGADirectoryHeader, typename TSGASection, typename TSGAFolder>
		class CSGASpecializedDirectory<TSGAHeader, TSGADirectoryHeader, TSGASection, TSGAFolder, SGAFile4> : public ISGADirectory
		{
		public:
			typedef typename TSGAHeader SGAHeader;
			typedef typename TSGADirectoryHeader SGADirectoryHeader;
			typedef typename TSGASection SGASection;
			typedef typename TSGAFolder SGAFolder;
			typedef CSGAFile::SGAFile4 SGAFile;

			CSGASpecializedDirectory(CSGAFile& File);

		protected:
			CSGAFile& File;

			Mapping::CView *pHeaderDirectoryView;
			const SGADirectoryHeader *pDirectoryHeader;
			const SGASection *lpSections;
			const SGAFolder *lpFolders;
			const SGAFile *lpFiles;
			const hlChar *lpStringTable;

		public:
			virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

			virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		};

		// Specialization SGAFile6 where the CRC moved to the header and the CRC is of the compressed data.
		template<typename TSGAHeader, typename TSGADirectoryHeader, typename TSGASection, typename TSGAFolder>
		class CSGASpecializedDirectory<TSGAHeader, TSGADirectoryHeader, TSGASection, TSGAFolder, SGAFile6> : public ISGADirectory
		{
		public:
			typedef typename TSGAHeader SGAHeader;
			typedef typename TSGADirectoryHeader SGADirectoryHeader;
			typedef typename TSGASection SGASection;
			typedef typename TSGAFolder SGAFolder;
			typedef CSGAFile::SGAFile6 SGAFile;

			CSGASpecializedDirectory(CSGAFile& File);

		protected:
			CSGAFile& File;

			Mapping::CView *pHeaderDirectoryView;
			const SGADirectoryHeader *pDirectoryHeader;
			const SGASection *lpSections;
			const SGAFolder *lpFolders;
			const SGAFile *lpFiles;
			const hlChar *lpStringTable;

		public:
			virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

			virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		};

		template<typename TSGAHeader, typename TSGADirectoryHeader, typename TSGASection, typename TSGAFolder, typename TSGAFile>
		class CSGADirectory : public CSGASpecializedDirectory<TSGAHeader, TSGADirectoryHeader, TSGASection, TSGAFolder, TSGAFile>
		{
		public:
			CSGADirectory(CSGAFile& File);
			virtual ~CSGADirectory();

		public:
			virtual hlBool MapDataStructures();
			virtual hlVoid UnmapDataStructures();

			virtual CDirectoryFolder *CreateRoot();

			virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

			virtual hlBool GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const;
			virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;
			virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;

			virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;
			virtual hlVoid ReleaseStreamInternal(Streams::IStream &Stream) const;

		private:
			hlVoid CreateFolder(CDirectoryFolder *pParent, hlUInt uiFolderIndex);
		};

		typedef CSGADirectory<SGAHeader4, SGADirectoryHeader4, SGASection4, SGAFolder4, SGAFile4> CSGADirectory4;
		typedef CSGADirectory<SGAHeader4, SGADirectoryHeader5, SGASection5, SGAFolder5, SGAFile4> CSGADirectory5;
		typedef CSGADirectory<SGAHeader6, SGADirectoryHeader5, SGASection5, SGAFolder5, SGAFile6> CSGADirectory6;
		typedef CSGADirectory<SGAHeader6, SGADirectoryHeader7, SGASection5, SGAFolder5, SGAFile7> CSGADirectory7;

		friend CSGADirectory4;
		friend CSGADirectory5;
		friend CSGADirectory6;
		friend CSGADirectory7;

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];
		static const char *lpVerificationNames[];

		Mapping::CView *pHeaderView;
		const SGAHeaderBase *pHeader;

		ISGADirectory* pDirectory;

	public:
		CSGAFile();
		virtual ~CSGAFile();

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
	};
}

#endif
