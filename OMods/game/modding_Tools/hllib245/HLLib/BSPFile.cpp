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
#include "BSPFile.h"
#include "Streams.h"

using namespace HLLib;

#define HL_BSP_LUMP_ENTITIES		0
#define HL_BSP_LUMP_TEXTUREDATA		2
#define HL_BSP_MIPMAP_COUNT			4

const char *CBSPFile::lpAttributeNames[] = { "Version" };
const char *CBSPFile::lpItemAttributeNames[] = { "Width", "Height", "Palette Entries" };

CBSPFile::CBSPFile() : CPackage(), pHeaderView(0), pTextureView(0), pHeader(0), pTextureHeader(0)
{

}

CBSPFile::~CBSPFile()
{
	this->Close();
}

HLPackageType CBSPFile::GetType() const
{
	return HL_PACKAGE_BSP;
}

const hlChar *CBSPFile::GetExtension() const
{
	return "bsp";
}

const hlChar *CBSPFile::GetDescription() const
{
	return "Half-Life Level";
}

hlBool CBSPFile::MapDataStructures()
{
	if(sizeof(BSPHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(BSPHeader)))
	{
		return hlFalse;
	}
	this->pHeader = static_cast<const BSPHeader *>(this->pHeaderView->GetView());

	if(this->pHeader->uiVersion != 30)
	{
		LastError.SetErrorMessageFormated("Invalid BSP version (v%u): you have a version of a BSP file that HLLib does not know how to read. Check for product updates.", this->pHeader->uiVersion);
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pTextureView, this->pHeader->lpLumps[HL_BSP_LUMP_TEXTUREDATA].uiOffset, this->pHeader->lpLumps[HL_BSP_LUMP_TEXTUREDATA].uiLength))
	{
		return hlFalse;
	}
	this->pTextureHeader = static_cast<const BSPTextureHeader *>(this->pTextureView->GetView());

	return hlTrue;
}

hlVoid CBSPFile::UnmapDataStructures()
{
	this->pTextureHeader = 0;
	this->pMapping->Unmap(this->pTextureView);

	this->pHeader = 0;
	this->pMapping->Unmap(this->pHeaderView);
}

CDirectoryFolder *CBSPFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder(this);

	hlChar lpFileName[256];

	if(this->pHeader->lpLumps[HL_BSP_LUMP_ENTITIES].uiLength != 0)
	{
		this->GetFileName(lpFileName, sizeof(lpFileName) - 4);
		if(*lpFileName == '\0')
		{
			pRoot->AddFile("entities.ent", this->pTextureHeader->uiTextureCount);
		}
		else
		{
			strcat(lpFileName, ".ent");
			pRoot->AddFile(lpFileName, this->pTextureHeader->uiTextureCount);
		}
	}

	// Loop through each texture in the BSP file.
	for(hlUInt i = 0; i < this->pTextureHeader->uiTextureCount; i++)
	{
		if(this->pTextureHeader->lpOffsets[i] == -1)
		{
			continue;
		}

		const BSPTexture *pTexture = (const BSPTexture *)((const hlByte *)this->pTextureHeader + this->pTextureHeader->lpOffsets[i]);

		if(pTexture->lpOffsets[0] == 0)
		{
			continue;
		}

		sprintf(lpFileName, "%s.bmp", pTexture->lpName);

		// Add the lump as a bitmap.
		pRoot->AddFile(lpFileName, i);
	}

	return pRoot;
}

hlUInt CBSPFile::GetAttributeCountInternal() const
{
	return HL_BSP_PACKAGE_COUNT;
}

const hlChar *CBSPFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_BSP_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CBSPFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_BSP_PACKAGE_VERSION:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiVersion, hlFalse);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CBSPFile::GetItemAttributeCountInternal() const
{
	return HL_BSP_ITEM_COUNT;
}

const hlChar *CBSPFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_BSP_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CBSPFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			if(pFile->GetID() < this->pTextureHeader->uiTextureCount)
			{
				switch(eAttribute)
				{
					case HL_BSP_ITEM_WIDTH:
					{
						hlUInt uiWidth = 0, uiHeight = 0, uiPaletteSize = 0;
						this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, hlFalse);
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiWidth, hlFalse);
						return hlTrue;
					}
					case HL_BSP_ITEM_HEIGHT:
					{
						hlUInt uiWidth = 0, uiHeight = 0, uiPaletteSize = 0;
						this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, hlFalse);
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiHeight, hlFalse);
						return hlTrue;
					}
					case HL_BSP_ITEM_PALETTE_ENTRIES:
					{
						hlUInt uiWidth = 0, uiHeight = 0, uiPaletteSize = 0;
						this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, hlFalse);
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], uiPaletteSize, hlFalse);
						return hlTrue;
					}
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CBSPFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	if(pFile->GetID() < this->pTextureHeader->uiTextureCount)
	{
		hlUInt uiWidth, uiHeight, uiPaletteSize;
		if(!this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize))
		{
			return hlFalse;
		}

		uiSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + uiPaletteSize * 4 + uiWidth * uiHeight;
	}
	else
	{
		uiSize = this->pHeader->lpLumps[HL_BSP_LUMP_ENTITIES].uiLength - 1;
	}

	return hlTrue;
}

hlBool CBSPFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	if(pFile->GetID() < this->pTextureHeader->uiTextureCount)
	{
		hlUInt uiWidth, uiHeight, uiPaletteSize;
		if(!this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize))
		{
			return hlFalse;
		}

		hlUInt uiPixelSize = 0;
		for(hlUInt i = 0; i < HL_BSP_MIPMAP_COUNT; i++)
		{
			//if(pTexture->lpOffsets[i] != 0)
			{
				uiPixelSize += (uiWidth >> i) * (uiHeight >> i);
			}
		}

		uiSize = sizeof(BSPTexture) + uiPixelSize + sizeof(hlUInt16) + uiPaletteSize * 3;
	}
	else
	{
		uiSize = this->pHeader->lpLumps[HL_BSP_LUMP_ENTITIES].uiLength - 1;
	}

	return hlTrue;
}

hlBool CBSPFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	if(pFile->GetID() < this->pTextureHeader->uiTextureCount)
	{
		hlUInt uiWidth, uiHeight, uiPaletteSize;
		const hlByte *lpPalette, *lpPixels;

		if(!this->GetLumpInfo(*pFile, uiWidth, uiHeight, uiPaletteSize, lpPalette, lpPixels))
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

		pStream = new Streams::CMemoryStream(lpBuffer, uiBufferSize);
	}
	else
	{
		pStream = new Streams::CMappingStream(*this->pMapping, this->pHeader->lpLumps[HL_BSP_LUMP_ENTITIES].uiOffset, this->pHeader->lpLumps[HL_BSP_LUMP_ENTITIES].uiLength - 1);
	}

	return hlTrue;
}

hlVoid CBSPFile::ReleaseStreamInternal(Streams::IStream &Stream) const
{
	if(Stream.GetType() == HL_STREAM_MEMORY)
	{
		delete [](hlByte *)static_cast<Streams::CMemoryStream &>(Stream).GetBuffer();
	}
}

hlVoid CBSPFile::GetFileName(hlChar *lpBuffer, hlUInt uiBufferSize)
{
	if(lpBuffer == 0 || uiBufferSize == 0)
	{
		return;
	}

	const hlChar *lpMappingName = this->pMapping->GetFileName();

	if(lpMappingName == 0 && *lpMappingName == '\0')
	{
		*lpBuffer = '\0';
		return;
	}

	const hlChar *lpForward = strrchr(lpMappingName, '\\');
	const hlChar *lpBackward = strrchr(lpMappingName, '/');
	const hlChar *lpStart = lpForward > lpBackward ? lpForward : lpBackward;

	if(lpStart == 0)
	{
		lpStart = lpMappingName;
	}
	else
	{
		lpStart++;
	}

	const hlChar *lpEnd = strrchr(lpStart, '.');

	if(lpEnd == 0)
	{
		lpEnd = lpStart + strlen(lpStart);
	}

	if((hlUInt)(lpEnd - lpStart) + 1 < uiBufferSize)
	{
		uiBufferSize = (hlUInt)(lpEnd - lpStart) + 1;
	}

	strncpy(lpBuffer, lpStart, uiBufferSize);
	lpBuffer[uiBufferSize - 1] = '\0';
}

hlBool CBSPFile::GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, hlUInt uiMipmap) const
{
	if(uiMipmap >= HL_BSP_MIPMAP_COUNT)
	{
		LastError.SetErrorMessageFormated("Error reading texture: invalid mipmap level %u.", uiMipmap);
		return hlFalse;
	}

	const BSPTexture *pTexture = (const BSPTexture *)((const hlByte *)this->pTextureHeader + this->pTextureHeader->lpOffsets[File.GetID()]);

	uiWidth = pTexture->uiWidth;
	uiHeight = pTexture->uiHeight;

	hlUInt uiPixelSize = 0;
	for(hlUInt i = 0; i < HL_BSP_MIPMAP_COUNT; i++)
	{
		if(pTexture->lpOffsets[i] != 0)
		{
			uiPixelSize += (uiWidth >> i) * (uiHeight >> i);
		}
	}

	const hlByte *lpData = (const hlByte *)pTexture + pTexture->lpOffsets[0];

	uiPaletteSize = (hlUInt)*(const hlUInt16 *)(lpData + uiPixelSize);

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

hlBool CBSPFile::GetLumpInfo(const CDirectoryFile &File, hlUInt &uiWidth, hlUInt &uiHeight, hlUInt &uiPaletteSize, const hlByte *&lpPalette, const hlByte *&lpPixels, hlUInt uiMipmap) const
{
	if(uiMipmap > 3)
	{
		LastError.SetErrorMessageFormated("Error reading texture: invalid mipmap level %u.", uiMipmap);
		return hlFalse;
	}

	const BSPTexture *pTexture = (const BSPTexture *)((const hlByte *)this->pTextureHeader + this->pTextureHeader->lpOffsets[File.GetID()]);

	uiWidth = pTexture->uiWidth;
	uiHeight = pTexture->uiHeight;

	hlUInt uiPixelSize = 0;
	for(hlUInt i = 0; i < HL_BSP_MIPMAP_COUNT; i++)
	{
		if(pTexture->lpOffsets[i] != 0)
		{
			uiPixelSize += (uiWidth >> i) * (uiHeight >> i);
		}
	}

	lpPixels = (const hlByte *)pTexture + pTexture->lpOffsets[uiMipmap];

	uiPaletteSize = (hlUInt)*(const hlUInt16 *)(((const hlByte *)pTexture + pTexture->lpOffsets[0]) + uiPixelSize);

	lpPalette = ((const hlByte *)pTexture + pTexture->lpOffsets[0]) + uiPixelSize + sizeof(hlUInt16);

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
