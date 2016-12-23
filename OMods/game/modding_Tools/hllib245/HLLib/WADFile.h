/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef WADFILE_H
#define WADFILE_H

#include "stdafx.h"
#include "Package.h"

namespace HLLib
{
	class HLLIB_API CWADFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct WADHeader
		{
			hlChar lpSignature[4];
			hlUInt uiLumpCount;
			hlUInt uiLumpOffset;
		};

		struct WADLump
		{
			hlUInt uiOffset;
			hlUInt uiDiskLength;
			hlUInt uiLength;
			hlChar iType;
			hlChar iCompression;
			hlChar iPadding0;
			hlChar iPadding1;
			hlChar lpName[16];
		};

		struct WADLumpInfo
		{
			hlUInt uiWidth;
			hlUInt uiHeight;
			hlUInt uiPaletteSize;
		};

		#pragma pack()

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];

		Mapping::CView *pHeaderView;
		Mapping::CView *pLumpView;

		const WADHeader *pHeader;
		const WADLump *lpLumps;
		WADLumpInfo *lpLumpInfo;

	public:
		CWADFile();
		virtual ~CWADFile();

		virtual HLPackageType GetType() const;
		virtual const hlChar *GetExtension() const;
		virtual const hlChar *GetDescription() const;

		hlBool GetImageSize(const CDirectoryFile *pFile, hlUInt &uiPaletteDataSize, hlUInt &uiPixelDataSize);
		hlBool GetImageData(const CDirectoryFile *pFile, hlUInt &uiWidth, hlUInt &uiHeight, hlByte *lpPaletteData, hlByte *lpPixelData);

		hlBool GetImageSize(const CDirectoryFile *pFile, hlUInt &uiPixelDataSize);
		hlBool GetImageData(const CDirectoryFile *pFile, hlUInt &uiWidth, hlUInt &uiHeight, hlByte *lpPixelData);

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
		virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;
		virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const;

		virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;
		virtual hlVoid ReleaseStreamInternal(Streams::IStream &Stream) const;

	private:
		hlBool GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, hlUInt uiMipmap = 0) const;
		hlBool GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, const hlByte *&lpPalette, const hlByte *&lpPixels, Mapping::CView *&pView, hlUInt uiMipmap = 0) const;
	};
}

#endif
