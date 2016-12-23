/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "HLLib.h"
#include "WADFile.h"
#include "Streams.h"

using namespace HLLib;

const char *CWADFile::lpAttributeNames[] = { "Version" };
const char *CWADFile::lpItemAttributeNames[] = { "Width", "Height", "Palette Entries", "Mipmaps", "Compressed", "Type" };

CWADFile::CWADFile() : CPackage(), pHeaderView(0), pLumpView(0), pHeader(0), lpLumps(0), lpLumpInfo(0)
{

}

CWADFile::~CWADFile()
{
	this->Close();
}

HLPackageType CWADFile::GetType() const
{
	return HL_PACKAGE_WAD;
}

const hlChar *CWADFile::GetExtension() const
{
	return "wad";
}

const hlChar *CWADFile::GetDescription() const
{
	return "Half-Life Texture Package File";
}

hlBool CWADFile::MapDataStructures()
{
	if(sizeof(WADHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(WADHeader)))
	{
		return hlFalse;
	}
	this->pHeader = (WADHeader *)this->pHeaderView->GetView();

	if(memcmp(this->pHeader->lpSignature, "WAD3", 4) != 0)
	{
		LastError.SetErrorMessage("Invalid file: the file's signature does not match.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pLumpView, this->pHeader->uiLumpOffset, this->pHeader->uiLumpCount * sizeof(WADLump)))
	{
		return hlFalse;
	}
	this->lpLumps = (WADLump *)this->pLumpView->GetView();

	this->lpLumpInfo = new WADLumpInfo[this->pHeader->uiLumpCount];
	memset(this->lpLumpInfo, 0, sizeof(WADLumpInfo) * this->pHeader->uiLumpCount);

	return hlTrue;
}

hlVoid CWADFile::UnmapDataStructures()
{
	delete []this->lpLumpInfo;

	this->lpLumps = 0;
	this->pMapping->Unmap(this->pLumpView);

	this->pHeader = 0;
	this->pMapping->Unmap(this->pHeaderView);
}

CDirectoryFolder *CWADFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder(this);

	// Loop through each lump in the WAD file.
	for(hlUInt i = 0; i < this->pHeader->uiLumpCount; i++)
	{
		hlChar lpName[64];
		sprintf(lpName, "%s.bmp", this->lpLumps[i].lpName);

		// Add the lump as a bitmap.
		pRoot->AddFile(lpName, i);
	}

	return pRoot;
}

hlUInt CWADFile::GetAttributeCountInternal() const
{
	return HL_WAD_PACKAGE_COUNT;
}

const hlChar *CWADFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_WAD_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CWADFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_WAD_PACKAGE_VERSION:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->lpSignature[3] - '0', hlFalse);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CWADFile::GetItemAttributeCountInternal() const
{
	return HL_WAD_ITEM_COUNT;
}

const hlChar *CWADFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_WAD_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CWADFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			const WADLump *pLump = this->lpLumps + pFile->GetID();
			switch(eAttribute)
			{
				case HL_WAD_ITEM_WIDTH:
				{
					hlUInt uiWidth = 0, uiHeight = 0, uiPaletteSize = 0;
					this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize);
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiWidth, hlFalse);
					return hlTrue;
				}
				case HL_WAD_ITEM_HEIGHT:
				{
					hlUInt uiWidth = 0, uiHeight = 0, uiPaletteSize = 0;
					this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize);
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiHeight, hlFalse);
					return hlTrue;
				}
				case HL_WAD_ITEM_PALETTE_ENTRIES:
				{
					hlUInt uiWidth = 0, uiHeight = 0, uiPaletteSize = 0;
					this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize);
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiPaletteSize, hlFalse);
					return hlTrue;
				}
				case HL_WAD_ITEM_MIPMAPS:
				{
					hlUInt uiMipmaps = 0;
					if(pLump->iType == 0x42)
					{
						uiMipmaps = 1;
					}
					else if(pLump->iType == 0x43)
					{
						uiMipmaps = 4;
					}
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiMipmaps, hlFalse);
					return hlTrue;
				}
				case HL_WAD_ITEM_COMPRESSED:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], pLump->iCompression != 0);
					return hlTrue;
				}
				case HL_WAD_ITEM_TYPE:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], (hlUInt)pLump->iType, hlTrue);
					return hlTrue;
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CWADFile::GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const
{
	const WADLump *pLump = this->lpLumps + pFile->GetID();

	bExtractable = (pLump->iType == 0x42 || pLump->iType == 0x43) && pLump->iCompression == 0;

	return hlTrue;
}

hlBool CWADFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	hlUInt uiWidth, uiHeight, uiPaletteSize;
	if(!const_cast<CWADFile *>(this)->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize))
	{
		return hlFalse;
	}

	uiSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + uiPaletteSize * 4 + uiWidth * uiHeight;

	return hlTrue;
}

hlBool CWADFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpLumps[pFile->GetID()].uiDiskLength;

	return hlTrue;
}

hlBool CWADFile::GetImageSize(const CDirectoryFile *pFile, hlUInt &uiPaletteDataSize, hlUInt &uiPixelDataSize)
{
	uiPaletteDataSize = uiPixelDataSize = 0;

	if(!this->GetOpened() || pFile == 0 || pFile->GetPackage() != this)
	{
		LastError.SetErrorMessage("File does not belong to package.");
		return hlFalse;
	}

	hlUInt uiWidth, uiHeight, uiPaletteSize;
	if(!const_cast<CWADFile *>(this)->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize))
	{
		return hlFalse;
	}

	uiPaletteDataSize = uiPaletteSize * 3;
	uiPixelDataSize = uiWidth * uiHeight;

	return hlTrue;
}

hlBool CWADFile::GetImageData(const CDirectoryFile *pFile, hlUInt &uiWidth, hlUInt &uiHeight, hlByte *lpPaletteData, hlByte *lpPixelData)
{
	uiWidth = uiHeight = 0;

	if(!this->GetOpened() || pFile == 0 || pFile->GetPackage() != this)
	{
		LastError.SetErrorMessage("File does not belong to package.");
		return hlFalse;
	}

	hlUInt uiPaletteSize;
	const hlByte *lpPalette, *lpPixels;
	Mapping::CView *pView = 0;

	if(!const_cast<CWADFile *>(this)->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, lpPalette, lpPixels, pView))
	{
		return hlFalse;
	}

	memcpy(lpPaletteData, lpPalette, uiPaletteSize * 3);
	memcpy(lpPixelData, lpPixels, uiWidth * uiHeight);

	this->pMapping->Unmap(pView);

	return hlTrue;
}

hlBool CWADFile::GetImageSize(const CDirectoryFile *pFile, hlUInt &uiPixelDataSize)
{
	uiPixelDataSize = 0;

	if(!this->GetOpened() || pFile == 0 || pFile->GetPackage() != this)
	{
		LastError.SetErrorMessage("File does not belong to package.");
		return hlFalse;
	}

	hlUInt uiWidth, uiHeight, uiPaletteSize;
	if(!const_cast<CWADFile *>(this)->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize))
	{
		return hlFalse;
	}

	uiPixelDataSize = uiWidth * uiHeight * 3;

	return hlTrue;
}

hlBool CWADFile::GetImageData(const CDirectoryFile *pFile, hlUInt &uiWidth, hlUInt &uiHeight, hlByte *lpPixelData)
{
	uiWidth = uiHeight = 0;

	if(!this->GetOpened() || pFile == 0 || pFile->GetPackage() != this)
	{
		LastError.SetErrorMessage("File does not belong to package.");
		return hlFalse;
	}

	hlUInt uiPaletteSize;
	const hlByte *lpPalette, *lpPixels;
	Mapping::CView *pView = 0;

	if(!const_cast<CWADFile *>(this)->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, lpPalette, lpPixels, pView))
	{
		return hlFalse;
	}

	for(hlUInt i = 0; i < uiWidth; i++)
	{
		for(hlUInt j = 0; j < uiHeight; j++)
		{
			hlUInt uiPixelIndex = i + j * uiWidth;
			hlUInt uiPaletteIndex = (hlUInt)lpPixels[uiPixelIndex] * 3;

			uiPixelIndex *= 3;
			lpPixelData[uiPixelIndex + 0] = lpPalette[uiPaletteIndex + 0];
			lpPixelData[uiPixelIndex + 1] = lpPalette[uiPaletteIndex + 1];
			lpPixelData[uiPixelIndex + 2] = lpPalette[uiPaletteIndex + 2];
		}
	}

	this->pMapping->Unmap(pView);

	return hlTrue;
}

hlBool CWADFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	hlUInt uiWidth, uiHeight, uiPaletteSize;
	const hlByte *lpPalette, *lpPixels;
	Mapping::CView *pView = 0;

	if(!const_cast<CWADFile *>(this)->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, lpPalette, lpPixels, pView))
	{
		return hlFalse;
	}

	hlUInt uiBufferSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + uiPaletteSize * 4 + uiWidth * uiHeight;
	hlByte *lpBuffer = new hlByte[uiBufferSize];

	//
	// Allocate data.
	//

	BITMAPFILEHEADER *pFileHeader = (BITMAPFILEHEADER *)lpBuffer;
	BITMAPINFOHEADER *pInfoHeader = (BITMAPINFOHEADER *)((hlByte *)pFileHeader + sizeof(BITMAPFILEHEADER));
	hlByte *lpPaletteData = (hlByte *)pInfoHeader + sizeof(BITMAPINFOHEADER);
	hlByte *lpPixelData = lpPaletteData + uiPaletteSize * 4;

	memset(pFileHeader, 0, sizeof(BITMAPFILEHEADER));
	memset(pInfoHeader, 0, sizeof(BITMAPINFOHEADER));

	//
	// Fill in headers.
	//

	pFileHeader->bfType = ('M' << 8) | 'B';
	pFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + uiPaletteSize * 4 + uiWidth * uiHeight;
	pFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + uiPaletteSize * 4;

	pInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	pInfoHeader->biWidth = uiWidth;
	pInfoHeader->biHeight = uiHeight;
	pInfoHeader->biPlanes = 1;
	pInfoHeader->biBitCount = 8;
	pInfoHeader->biSizeImage = 0;
	pInfoHeader->biClrUsed = uiPaletteSize;
	pInfoHeader->biClrImportant = uiPaletteSize;

	//
	// Fill in Palette data.
	//

	for(hlUInt i = 0; i < uiPaletteSize; i++)
	{
		lpPaletteData[i * 4 + 0] = lpPalette[i * 3 + 2];
		lpPaletteData[i * 4 + 1] = lpPalette[i * 3 + 1];
		lpPaletteData[i * 4 + 2] = lpPalette[i * 3 + 0];
		lpPaletteData[i * 4 + 3] = 0;
	}

	//
	// Fill in Index data.
	//

	for(hlUInt i = 0; i < uiWidth; i++)
	{
		for(hlUInt j = 0; j < uiHeight; j++)
		{
			lpPixelData[i + (uiHeight - 1 - j) * uiWidth] = lpPixels[i + j * uiWidth];
		}
	}

	this->pMapping->Unmap(pView);

	pStream = new Streams::CMemoryStream(lpBuffer, uiBufferSize);

	return hlTrue;
}

hlVoid CWADFile::ReleaseStreamInternal(Streams::IStream &Stream) const
{
	delete [](hlByte *)static_cast<Streams::CMemoryStream &>(Stream).GetBuffer();
}

hlBool CWADFile::GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, hlUInt uiMipmap) const
{
	const WADLump &Lump = this->lpLumps[File.GetID()];

	if(Lump.iCompression)
	{
		LastError.SetErrorMessageFormated("Error reading lump: compression format %#.2x not supported.", Lump.iCompression);
		return hlFalse;
	}

	if(Lump.iType == 0x42)
	{
		if(uiMipmap > 0)
		{
			LastError.SetErrorMessageFormated("Error reading lump: invalid mipmap level %u.", uiMipmap);
			return hlFalse;
		}
	}
	else if(Lump.iType == 0x43)
	{
		if(uiMipmap > 3)
		{
			LastError.SetErrorMessageFormated("Error reading lump: invalid mipmap level %u.", uiMipmap);
			return hlFalse;
		}
	}
	else
	{
		LastError.SetErrorMessageFormated("Error reading lump: lump type %#.2x not supported.", Lump.iType);
		return hlFalse;
	}

	WADLumpInfo &Info = this->lpLumpInfo[File.GetID()];

	if(Info.uiWidth && Info.uiHeight && Info.uiPaletteSize)
	{
		uiWidth = Info.uiWidth;
		uiHeight = Info.uiHeight;
		uiPaletteSize = Info.uiPaletteSize;
	}
	else
	{
		Mapping::CView *pView = 0;

		if(!this->pMapping->Map(pView, Lump.uiOffset, Lump.uiDiskLength))
		{
			return hlFalse;
		}

		hlByte *lpData = (hlByte *)pView->GetView();

		// Type 0x42 has no name, type 0x43 does.  Are these flags?
		if(Lump.iType == 0x42)
		{
			// Get Width.
			uiWidth = *(hlUInt *)lpData;
			lpData += 4;

			// Get Height.
			uiHeight = *(hlUInt *)lpData;
			lpData += 4;

			// Scan past pixel data.
			lpData += (uiWidth * uiHeight);

			// Get palette size.
			uiPaletteSize = (hlUInt)(*(hlUInt16 *)lpData);
		}
		else if(Lump.iType == 0x43)
		{
			// Scan past name.
			lpData += 16;

			// Get Width.
			uiWidth = *(hlUInt *)lpData;
			lpData += 4;

			// Get Height.
			uiHeight = *(hlUInt *)lpData;
			lpData += 4;

			// Scan past pixel offset.
			lpData += 16;

			hlUInt uiPixelSize = uiWidth * uiHeight;

			switch(uiMipmap)
			{
			case 1:
				lpData += (uiPixelSize);
				break;
			case 2:
				lpData += (uiPixelSize) + (uiPixelSize / 4);
				break;
			case 3:
				lpData += (uiPixelSize) + (uiPixelSize / 4) + (uiPixelSize / 16);
				break;
			}

			// Scan past data.
			lpData += (uiPixelSize) + (uiPixelSize / 4) + (uiPixelSize / 16) + (uiPixelSize / 64);

			// Get palette size.
			uiPaletteSize = (hlUInt)(*(hlUInt16 *)lpData);
		}

		this->pMapping->Unmap(pView);

		Info.uiWidth = uiWidth;
		Info.uiHeight = uiHeight;
		Info.uiPaletteSize = uiPaletteSize;
	}

	switch(uiMipmap)
	{
	case 1:
		uiWidth /= 2;
		uiHeight /= 2;
		break;
	case 2:
		uiWidth /= 4;
		uiHeight /= 4;
		break;
	case 3:
		uiWidth /= 8;
		uiHeight /= 8;
		break;
	}

	return hlTrue;
}

hlBool CWADFile::GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, const hlByte *&lpPalette, const hlByte *&lpPixels, Mapping::CView *&pView, hlUInt uiMipmap) const
{
	const WADLump &Lump = this->lpLumps[File.GetID()];

	pView = 0;

	if(Lump.iCompression)
	{
		LastError.SetErrorMessageFormated("Error reading lump: compression format %#.2x not supported.", Lump.iCompression);
		return hlFalse;
	}

	if(Lump.iType == 0x42)
	{
		if(uiMipmap > 0)
		{
			LastError.SetErrorMessageFormated("Error reading lump: invalid mipmap level %u.", uiMipmap);
			return hlFalse;
		}
	}
	else if(Lump.iType == 0x43)
	{
		if(uiMipmap > 3)
		{
			LastError.SetErrorMessageFormated("Error reading lump: invalid mipmap level %u.", uiMipmap);
			return hlFalse;
		}
	}
	else
	{
		LastError.SetErrorMessageFormated("Error reading lump: lump type %#.2x not supported.", Lump.iType);
		return hlFalse;
	}

	if(!this->pMapping->Map(pView, Lump.uiOffset, Lump.uiDiskLength))
	{
		return hlFalse;
	}

	hlByte *lpData = (hlByte *)pView->GetView();

	// Type 0x42 has no name, type 0x43 does.  Are these flags?
	if(Lump.iType == 0x42)
	{
		// Get Width.
		uiWidth = *(hlUInt *)lpData;
		lpData += sizeof(hlUInt);

		// Get Height.
		uiHeight = *(hlUInt *)lpData;
		lpData += sizeof(hlUInt);

		// Get pixel data.
		lpPixels = lpData;
		lpData += (uiWidth * uiHeight);

		// Get palette size.
		uiPaletteSize = (hlUInt)(*(hlUInt16 *)lpData);
		lpData += sizeof(hlUInt16);

		// Get palette.
		lpPalette = lpData;
	}
	else if(Lump.iType == 0x43)
	{
		// Scan past name.
		lpData += 16;

		// Get Width.
		uiWidth = *(hlUInt *)lpData;
		lpData += sizeof(hlUInt);

		// Get Height.
		uiHeight = *(hlUInt *)lpData;
		lpData += sizeof(hlUInt);

		// Get pixel offset.
		hlUInt uiPixelOffset = *(hlUInt *)lpData;
		lpData += 16;

		lpPixels = (hlByte *)pView->GetView() + uiPixelOffset;

		hlUInt uiPixelSize = uiWidth * uiHeight;

		switch(uiMipmap)
		{
		case 1:
			lpData += (uiPixelSize);
			break;
		case 2:
			lpData += (uiPixelSize) + (uiPixelSize / 4);
			break;
		case 3:
			lpData += (uiPixelSize) + (uiPixelSize / 4) + (uiPixelSize / 16);
			break;
		}

		// Scan past data.
		lpData += (uiPixelSize) + (uiPixelSize / 4) + (uiPixelSize / 16) + (uiPixelSize / 64);

		// Get palette size.
		uiPaletteSize = (hlUInt)(*(hlUInt16 *)lpData);
		lpData += sizeof(hlUInt16);

		// Get palette.
		lpPalette = lpData;
	}

	//this->pMapping->Unmap(pView);

	switch(uiMipmap)
	{
	case 1:
		uiWidth /= 2;
		uiHeight /= 2;
		break;
	case 2:
		uiWidth /= 4;
		uiHeight /= 4;
		break;
	case 3:
		uiWidth /= 8;
		uiHeight /= 8;
		break;
	}

	return hlTrue;
}
