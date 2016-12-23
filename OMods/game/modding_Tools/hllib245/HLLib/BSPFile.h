/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef BSPFILE_H
#define BSPFILE_H

#include "stdafx.h"
#include "Package.h"

#define HL_BSP_LUMP_COUNT 15

namespace HLLib
{
	class HLLIB_API CBSPFile : public CPackage
	{
	private:
		#pragma pack(1)

		struct BSPLump
		{
			hlUInt uiOffset;
			hlUInt uiLength;
		};

		struct BSPHeader
		{
			hlUInt uiVersion;
			BSPLump lpLumps[HL_BSP_LUMP_COUNT];
		};

		struct BSPTextureHeader
		{
			hlUInt uiTextureCount;
			hlUInt lpOffsets[1];
		};

		struct BSPTexture
		{
			hlChar lpName[16];
			hlUInt uiWidth;
			hlUInt uiHeight;
			hlUInt lpOffsets[4];
		};

		#pragma pack()

	private:
		static const char *lpAttributeNames[];
		static const char *lpItemAttributeNames[];

		Mapping::CView *pHeaderView;
		Mapping::CView *pTextureView;

		const BSPHeader *pHeader;
		const BSPTextureHeader *pTextureHeader;

	public:
		CBSPFile();
		virtual ~CBSPFile();

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
		virtual hlVoid ReleaseStreamInternal(Streams::IStream &Stream) const;

	private:
		hlVoid GetFileName(hlChar *lpBuffer, hlUInt uiBufferSize);

		hlBool GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, hlUInt uiMipmap = 0) const;
		hlBool GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, const hlByte *&lpPalette, const hlByte *&lpPixels, hlUInt uiMipmap = 0) const;
	};
}

#endif
