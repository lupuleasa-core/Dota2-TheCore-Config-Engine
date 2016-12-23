/*
 * HLLib
 * Copyright (C) 2006-2012 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "HLLib.h"
#include "DirectoryItems.h"
#include "Mappings.h"
#include "Streams.h"
#include "Packages.h"
#include "Wrapper.h"

using namespace HLLib;
using namespace HLLib::Streams;
using namespace HLLib::Mapping;

//
// Directory Item
//

HLLIB_API HLDirectoryItemType hlItemGetType(const HLDirectoryItem *pItem)
{
	return static_cast<const CDirectoryItem *>(pItem)->GetType();
}

HLLIB_API const hlChar *hlItemGetName(const HLDirectoryItem *pItem)
{
	return static_cast<const CDirectoryItem *>(pItem)->GetName();
}

HLLIB_API hlUInt hlItemGetID(const HLDirectoryItem *pItem)
{
	return static_cast<const CDirectoryItem *>(pItem)->GetID();
}

HLLIB_API const hlVoid *hlItemGetData(const HLDirectoryItem *pItem)
{
	return static_cast<const CDirectoryItem *>(pItem)->GetData();
}

HLLIB_API hlUInt hlItemGetPackage(const HLDirectoryItem *pItem)
{
	if(bInitialized)
	{
		const CPackage *pPackage = static_cast<const CDirectoryItem *>(pItem)->GetPackage();

		for(hlUInt i = 0; i < (hlUInt)pPackageVector->size(); i++)
		{
			if((*pPackageVector)[i] == pPackage)
			{
				return i;
			}
		}
	}

	return HL_ID_INVALID;
}

HLLIB_API HLDirectoryItem *hlItemGetParent(HLDirectoryItem *pItem)
{
	return static_cast<CDirectoryItem *>(pItem)->GetParent();
}

HLLIB_API hlBool hlItemGetSize(const HLDirectoryItem *pItem, hlUInt *pSize)
{
	*pSize = 0;

	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		*pSize = static_cast<const CDirectoryFolder *>(pItem)->GetSize();
	}
	else if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		return static_cast<const CDirectoryFile *>(pItem)->GetSize(*pSize);
	}

	return hlFalse;
}

HLLIB_API hlBool hlItemGetSizeEx(const HLDirectoryItem *pItem, hlULongLong *pSize)
{
	*pSize = 0;

	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		*pSize = static_cast<const CDirectoryFolder *>(pItem)->GetSizeEx();
	}
	else if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		hlUInt uiSize;
		if(static_cast<const CDirectoryFile *>(pItem)->GetSize(uiSize))
		{
			*pSize = static_cast<hlULongLong>(uiSize);
			return hlTrue;
		}
	}

	return hlFalse;
}

HLLIB_API hlBool hlItemGetSizeOnDisk(const HLDirectoryItem *pItem, hlUInt *pSize)
{
	*pSize = 0;

	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		*pSize = static_cast<const CDirectoryFolder *>(pItem)->GetSizeOnDisk();
	}
	else if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		return static_cast<const CDirectoryFile *>(pItem)->GetSizeOnDisk(*pSize);
	}

	return hlFalse;
}

HLLIB_API hlBool hlItemGetSizeOnDiskEx(const HLDirectoryItem *pItem, hlULongLong *pSize)
{
	*pSize = 0;

	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		*pSize = static_cast<const CDirectoryFolder *>(pItem)->GetSizeOnDiskEx();
	}
	else if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		hlUInt uiSize;
		if(static_cast<const CDirectoryFile *>(pItem)->GetSizeOnDisk(uiSize))
		{
			*pSize = static_cast<hlULongLong>(uiSize);
			return hlTrue;
		}
	}

	return hlFalse;
}

HLLIB_API hlVoid hlItemGetPath(const HLDirectoryItem *pItem, hlChar *lpPath, hlUInt uiPathSize)
{
	static_cast<const CDirectoryItem *>(pItem)->GetPath(lpPath, uiPathSize);
}

HLLIB_API hlBool hlItemExtract(HLDirectoryItem *pItem, const hlChar *lpPath)
{
	return static_cast<CDirectoryItem *>(pItem)->Extract(lpPath);
}

//
// Directory Folder
//

HLLIB_API hlUInt hlFolderGetCount(const HLDirectoryItem *pItem)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetCount();
	}

	return 0;
}

HLLIB_API HLDirectoryItem *hlFolderGetItem(HLDirectoryItem *pItem, hlUInt uiIndex)
{
	if(static_cast<CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<CDirectoryFolder *>(pItem)->GetItem(uiIndex);
	}

	return 0;
}

HLLIB_API HLDirectoryItem *hlFolderGetItemByName(HLDirectoryItem *pItem, const hlChar *lpName, HLFindType eFind)
{
	if(static_cast<CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<CDirectoryFolder *>(pItem)->GetItem(lpName, eFind);
	}

	return 0;
}

HLLIB_API HLDirectoryItem *hlFolderGetItemByPath(HLDirectoryItem *pItem, const hlChar *lpPath, HLFindType eFind)
{
	if(static_cast<CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<CDirectoryFolder *>(pItem)->GetRelativeItem(lpPath, eFind);
	}

	return 0;
}

HLLIB_API hlVoid hlFolderSort(HLDirectoryItem *pItem, HLSortField eField, HLSortOrder eOrder, hlBool bRecurse)
{
	if(static_cast<CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<CDirectoryFolder *>(pItem)->Sort(eField, eOrder, bRecurse);
	}
}

HLLIB_API HLDirectoryItem *hlFolderFindFirst(HLDirectoryItem *pFolder, const hlChar *lpSearch, HLFindType eFind)
{
	if(static_cast<CDirectoryItem *>(pFolder)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<CDirectoryFolder *>(pFolder)->FindFirst(lpSearch, eFind);
	}

	return 0;
}

HLLIB_API HLDirectoryItem *hlFolderFindNext(HLDirectoryItem *pFolder, HLDirectoryItem *pItem, const hlChar *lpSearch, HLFindType eFind)
{
	if(static_cast<CDirectoryItem *>(pFolder)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<CDirectoryFolder *>(pFolder)->FindNext(static_cast<CDirectoryItem *>(pItem), lpSearch, eFind);
	}

	return 0;
}

HLLIB_API hlUInt hlFolderGetSize(const HLDirectoryItem *pItem, hlBool bRecurse)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetSize(bRecurse);
	}

	return 0;
}

HLLIB_API hlULongLong hlFolderGetSizeEx(const HLDirectoryItem *pItem, hlBool bRecurse)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetSizeEx(bRecurse);
	}

	return 0;
}

HLLIB_API hlUInt hlFolderGetSizeOnDisk(const HLDirectoryItem *pItem, hlBool bRecurse)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetSizeOnDisk(bRecurse);
	}

	return 0;
}

HLLIB_API hlULongLong hlFolderGetSizeOnDiskEx(const HLDirectoryItem *pItem, hlBool bRecurse)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetSizeOnDiskEx(bRecurse);
	}

	return 0;
}

HLLIB_API hlUInt hlFolderGetFolderCount(const HLDirectoryItem *pItem, hlBool bRecurse)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetFolderCount(bRecurse);
	}

	return 0;
}

HLLIB_API hlUInt hlFolderGetFileCount(const HLDirectoryItem *pItem, hlBool bRecurse)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FOLDER)
	{
		return static_cast<const CDirectoryFolder *>(pItem)->GetFileCount(bRecurse);
	}

	return 0;
}

//
// Directory File
//

HLLIB_API hlUInt hlFileGetExtractable(const HLDirectoryItem *pItem)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		return static_cast<const CDirectoryFile *>(pItem)->GetExtractable();
	}

	return 0;
}

HLLIB_API HLValidation hlFileGetValidation(const HLDirectoryItem *pItem)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		return static_cast<const CDirectoryFile *>(pItem)->GetValidation();
	}

	return HL_VALIDATES_ASSUMED_OK;
}

HLLIB_API hlUInt hlFileGetSize(const HLDirectoryItem *pItem)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		return static_cast<const CDirectoryFile *>(pItem)->GetSize();
	}

	return 0;
}

HLLIB_API hlUInt hlFileGetSizeOnDisk(const HLDirectoryItem *pItem)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		return static_cast<const CDirectoryFile *>(pItem)->GetSizeOnDisk();
	}

	return 0;
}

HLLIB_API hlBool hlFileCreateStream(HLDirectoryItem *pItem, HLStream **pStream)
{
	*pStream = 0;

	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		IStream *pTemp = 0;
		hlBool bResult = static_cast<CDirectoryFile *>(pItem)->CreateStream(pTemp);
		*pStream = pTemp;

		return bResult;
	}

	return hlFalse;
}

HLLIB_API hlVoid hlFileReleaseStream(HLDirectoryItem *pItem, HLStream *pStream)
{
	if(static_cast<const CDirectoryItem *>(pItem)->GetType() == HL_ITEM_FILE)
	{
		static_cast<CDirectoryFile *>(pItem)->ReleaseStream(static_cast<IStream *>(pStream));
	}
}

//
// Stream
//

HLLIB_API HLStreamType hlStreamGetType(const HLStream *pStream)
{
	return static_cast<const IStream *>(pStream)->GetType();
}

HLLIB_API hlBool hlStreamGetOpened(const HLStream *pStream)
{
	return static_cast<const IStream *>(pStream)->GetOpened();
}

HLLIB_API hlUInt hlStreamGetMode(const HLStream *pStream)
{
	return static_cast<const IStream *>(pStream)->GetMode();
}

HLLIB_API hlBool hlStreamOpen(HLStream *pStream, hlUInt uiMode)
{
	return static_cast<IStream *>(pStream)->Open(uiMode);
}

HLLIB_API hlVoid hlStreamClose(HLStream *pStream)
{
	static_cast<IStream *>(pStream)->Close();
}

HLLIB_API hlUInt hlStreamGetStreamSize(const HLStream *pStream)
{
	return static_cast<hlUInt>(static_cast<const IStream *>(pStream)->GetStreamSize());
}

HLLIB_API hlULongLong hlStreamGetStreamSizeEx(const HLStream *pStream)
{
	return static_cast<const IStream *>(pStream)->GetStreamSize();
}

HLLIB_API hlUInt hlStreamGetStreamPointer(const HLStream *pStream)
{
	return static_cast<hlUInt>(static_cast<const IStream *>(pStream)->GetStreamPointer());
}

HLLIB_API hlULongLong hlStreamGetStreamPointerEx(const HLStream *pStream)
{
	return static_cast<const IStream *>(pStream)->GetStreamPointer();
}

HLLIB_API hlUInt hlStreamSeek(HLStream *pStream, hlLongLong iOffset, HLSeekMode eSeekMode)
{
	return static_cast<hlUInt>(static_cast<IStream *>(pStream)->Seek(iOffset, eSeekMode));
}

HLLIB_API hlULongLong hlStreamSeekEx(HLStream *pStream, hlLongLong iOffset, HLSeekMode eSeekMode)
{
	return static_cast<IStream *>(pStream)->Seek(iOffset, eSeekMode);
}

HLLIB_API hlBool hlStreamReadChar(HLStream *pStream, hlChar *pChar)
{
	return static_cast<IStream *>(pStream)->Read(*pChar);
}

HLLIB_API hlUInt hlStreamRead(HLStream *pStream, hlVoid *lpData, hlUInt uiBytes)
{
	return static_cast<IStream *>(pStream)->Read(lpData, uiBytes);
}

HLLIB_API hlBool hlStreamWriteChar(HLStream *pStream, hlChar iChar)
{
	return static_cast<IStream *>(pStream)->Write(iChar);
}

HLLIB_API hlUInt hlStreamWrite(HLStream *pStream, const hlVoid *lpData, hlUInt uiBytes)
{
	return static_cast<IStream *>(pStream)->Write(lpData, uiBytes);
}

//
// Package
//

HLLIB_API hlBool hlBindPackage(hlUInt uiPackage)
{
	if(!bInitialized)
	{
		LastError.SetErrorMessage("HLLib not initialized.");
		return hlFalse;
	}

	if(uiPackage >= pPackageVector->size() || (*pPackageVector)[uiPackage] == 0)
	{
		LastError.SetErrorMessage("Invalid package.");
		return hlFalse;
	}

	pPackage = (*pPackageVector)[uiPackage];

	return hlTrue;
}

HLLIB_API HLPackageType hlGetPackageTypeFromName(const hlChar *lpName)
{
	const char *lpExt = strrchr(lpName, '.');

	if(lpExt == 0)
	{
		return HL_PACKAGE_NONE;
	}

	// Create the package.
	/*if(stricmp(lpExt, ".bsp") == 0)
	{
		return HL_PACKAGE_BSP;
		return HL_PACKAGE_VBSP;
	}
	else*/ if(stricmp(lpExt, ".gcf") == 0)
	{
		return HL_PACKAGE_GCF;
	}
	else if(stricmp(lpExt, ".ncf") == 0)
	{
		return HL_PACKAGE_NCF;
	}
	else if(stricmp(lpExt, ".pak") == 0)
	{
		return HL_PACKAGE_PAK;
	}
	else if(stricmp(lpExt, ".vpk") == 0)
	{
		return HL_PACKAGE_VPK;
	}
	else if(stricmp(lpExt, ".wad") == 0)
	{
		return HL_PACKAGE_WAD;
	}
	else if(stricmp(lpExt, ".xzp") == 0)
	{
		return HL_PACKAGE_XZP;
	}
	else if(stricmp(lpExt, ".zip") == 0)
	{
		return HL_PACKAGE_ZIP;
	}

	return HL_PACKAGE_NONE;
}

struct HLPackageTest
{
	HLPackageType ePackageType;
	hlUInt uiTestLength;
	hlByte lpTest[HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE];
};

HLPackageTest lpPackageTests[] =
{
	{ HL_PACKAGE_BSP, 4, { 0x1e, 0x00, 0x00, 0x00 } },
	{ HL_PACKAGE_GCF, 8, { 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 } },
	{ HL_PACKAGE_NCF, 8, { 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 } },
	{ HL_PACKAGE_PAK, 4, { 'P', 'A', 'C', 'K' } },
	{ HL_PACKAGE_SGA, 8, { '_', 'A', 'R', 'C', 'H', 'I', 'V', 'E' } },
	{ HL_PACKAGE_VBSP, 4, { 'V', 'B', 'S', 'P' } },
	{ HL_PACKAGE_VPK, 4, { 0x34, 0x12, 0xaa, 0x55 } },
	{ HL_PACKAGE_WAD, 4, { 'W', 'A', 'D', '3' } },
	{ HL_PACKAGE_XZP, 4, { 'p', 'i', 'Z', 'x' } },
	{ HL_PACKAGE_ZIP, 2, { 'P', 'K' } },
	{ HL_PACKAGE_NONE, 0, { } }
};

HLLIB_API HLPackageType hlGetPackageTypeFromMemory(const hlVoid *lpBuffer, hlUInt uiBufferSize)
{
	if(uiBufferSize == 0)
	{
		return HL_PACKAGE_NONE;
	}

	for(HLPackageTest *pPackageTest = lpPackageTests; pPackageTest->ePackageType != HL_PACKAGE_NONE; pPackageTest++)
	{
		if(pPackageTest->uiTestLength <= uiBufferSize && memcmp(lpBuffer, pPackageTest->lpTest, pPackageTest->uiTestLength) == 0)
		{
			return pPackageTest->ePackageType;
		}
	}

	return HL_PACKAGE_NONE;
}

HLLIB_API HLPackageType hlGetPackageTypeFromStream(HLStream *pStream)
{
	IStream &Stream = *static_cast<IStream *>(pStream);

	hlUInt uiBufferSize;
	hlByte lpBuffer[HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE];

	if(Stream.GetOpened())
	{
		hlULongLong uiPointer = Stream.GetStreamPointer();

		uiBufferSize = Stream.Read(lpBuffer, sizeof(lpBuffer));

		Stream.Seek(static_cast<hlLongLong>(uiPointer), HL_SEEK_BEGINNING);
	}
	else
	{
		if(!Stream.Open(HL_MODE_READ))
		{
			return HL_PACKAGE_NONE;
		}

		uiBufferSize = Stream.Read(lpBuffer, sizeof(lpBuffer));

		Stream.Close();
	}

	return hlGetPackageTypeFromMemory(lpBuffer, uiBufferSize);
}

HLLIB_API hlBool hlCreatePackage(HLPackageType ePackageType, hlUInt *uiPackage)
{
	if(!bInitialized)
	{
		LastError.SetErrorMessage("HLLib not initialized.");
		return hlFalse;
	}

	CPackage* pNewPackage = 0;
	switch(ePackageType)
	{
	case HL_PACKAGE_NONE:
		LastError.SetErrorMessage("Unsupported package type.");
		return hlFalse;
		break;
	case HL_PACKAGE_BSP:
		pNewPackage = new CBSPFile();
		break;
	case HL_PACKAGE_GCF:
		pNewPackage = new CGCFFile();
		break;
	case HL_PACKAGE_NCF:
		pNewPackage = new CNCFFile();
		break;
	case HL_PACKAGE_PAK:
		pNewPackage = new CPAKFile();
		break;
	case HL_PACKAGE_SGA:
		pNewPackage = new CSGAFile();
		break;
	case HL_PACKAGE_VBSP:
		pNewPackage = new CVBSPFile();
		break;
	case HL_PACKAGE_VPK:
		pNewPackage = new CVPKFile();
		break;
	case HL_PACKAGE_WAD:
		pNewPackage = new CWADFile();
		break;
	case HL_PACKAGE_XZP:
		pNewPackage = new CXZPFile();
		break;
	case HL_PACKAGE_ZIP:
		pNewPackage = new CZIPFile();
		break;
	default:
		LastError.SetErrorMessageFormated("Invalid package type %d.", ePackageType);
		return hlFalse;
	}

	for(hlUInt i = 0; i < (hlUInt)pPackageVector->size(); i++)
	{
		if((*pPackageVector)[i] == 0)
		{
			(*pPackageVector)[i] = pNewPackage;
			*uiPackage = i;
			return hlTrue;
		}
	}

	pPackageVector->push_back(pNewPackage);
	*uiPackage = (hlUInt)pPackageVector->size() - 1;

	return hlTrue;
}

HLLIB_API hlVoid hlDeletePackage(hlUInt uiPackage)
{
	if(!bInitialized)
		return;

	if(uiPackage >= pPackageVector->size())
		return;

	if((*pPackageVector)[uiPackage] == 0)
		return;

	if((*pPackageVector)[uiPackage] == pPackage)
	{
		pPackage = 0;
	}

	delete (*pPackageVector)[uiPackage];
	(*pPackageVector)[uiPackage] = 0;
}

HLLIB_API HLPackageType hlPackageGetType()
{
	if(pPackage == 0)
	{
		return HL_PACKAGE_NONE;
	}

	return pPackage->GetType();
}

HLLIB_API const hlChar *hlPackageGetExtension()
{
	if(pPackage == 0)
	{
		return "";
	}

	return pPackage->GetExtension();
}

HLLIB_API const hlChar *hlPackageGetDescription()
{
	if(pPackage == 0)
	{
		return "";
	}

	return pPackage->GetDescription();
}

HLLIB_API hlBool hlPackageGetOpened()
{
	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->GetOpened();
}

HLLIB_API hlBool hlPackageOpenFile(const hlChar *lpFileName, hlUInt uiMode)
{
	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->Open(lpFileName, uiMode);
}

HLLIB_API hlBool hlPackageOpenMemory(hlVoid *lpData, hlUInt uiBufferSize, hlUInt uiMode)
{
	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->Open(lpData, uiBufferSize, uiMode);
}

HLLIB_API hlBool hlPackageOpenProc(hlVoid *pUserData, hlUInt uiMode)
{
	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->Open(pUserData, uiMode);
}

HLLIB_API hlBool hlPackageOpenStream(HLStream *pStream, hlUInt uiMode)
{
	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->Open(*static_cast<IStream *>(pStream), uiMode);
}

HLLIB_API hlVoid hlPackageClose()
{
	if(pPackage == 0)
	{
		return;
	}

	pPackage->Close();
}

HLLIB_API hlBool hlPackageDefragment()
{
	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->Defragment();
}

HLLIB_API HLDirectoryItem *hlPackageGetRoot()
{
	if(pPackage == 0)
	{
		return 0;
	}

	return pPackage->GetRoot();
}

HLLIB_API hlUInt hlPackageGetAttributeCount()
{
	if(pPackage == 0)
	{
		return 0;
	}

	return pPackage->GetAttributeCount();
}

HLLIB_API const hlChar *hlPackageGetAttributeName(HLPackageAttribute eAttribute)
{
	if(pPackage == 0)
	{
		return 0;
	}

	return pPackage->GetAttributeName(eAttribute);
}

HLLIB_API hlBool hlPackageGetAttribute(HLPackageAttribute eAttribute, HLAttribute *pAttribute)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_INVALID;

	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->GetAttribute(eAttribute, *pAttribute);
}

HLLIB_API hlUInt hlPackageGetItemAttributeCount()
{
	if(pPackage == 0)
	{
		return 0;
	}

	return pPackage->GetItemAttributeCount();
}

HLLIB_API const hlChar *hlPackageGetItemAttributeName(HLPackageAttribute eAttribute)
{
	if(pPackage == 0)
	{
		return 0;
	}

	return pPackage->GetItemAttributeName(eAttribute);
}

HLLIB_API hlBool hlPackageGetItemAttribute(const HLDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute *pAttribute)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_INVALID;

	if(pPackage == 0)
	{
		return hlFalse;
	}

	return pPackage->GetItemAttribute(static_cast<const CDirectoryItem *>(pItem), eAttribute, *pAttribute);
}

HLLIB_API hlBool hlPackageGetExtractable(const HLDirectoryItem *pFile, hlBool *pExtractable)
{
	*pExtractable = hlFalse;

	if(pPackage == 0)
	{
		return hlFalse;
	}

	if(static_cast<const CDirectoryItem *>(pFile)->GetType() == HL_ITEM_FILE)
	{
		return pPackage->GetFileExtractable(static_cast<const CDirectoryFile *>(pFile), *pExtractable);
	}

	return hlFalse;
}

HLLIB_API hlBool hlPackageGetFileSize(const HLDirectoryItem *pFile, hlUInt *pSize)
{
	*pSize = 0;

	if(pPackage == 0)
	{
		return hlFalse;
	}

	if(static_cast<const CDirectoryItem *>(pFile)->GetType() == HL_ITEM_FILE)
	{
		return pPackage->GetFileSize(static_cast<const CDirectoryFile *>(pFile), *pSize);
	}

	return hlFalse;
}

HLLIB_API hlBool hlPackageGetFileSizeOnDisk(const HLDirectoryItem *pFile, hlUInt *pSize)
{
	*pSize = 0;

	if(pPackage == 0)
	{
		return hlFalse;
	}

	if(static_cast<const CDirectoryItem *>(pFile)->GetType() == HL_ITEM_FILE)
	{
		return pPackage->GetFileSizeOnDisk(static_cast<const CDirectoryFile *>(pFile), *pSize);
	}

	return hlFalse;
}

HLLIB_API hlBool hlPackageCreateStream(const HLDirectoryItem *pFile, HLStream **pStream)
{
	*pStream = 0;

	if(pPackage == 0)
	{
		return hlFalse;
	}

	if(static_cast<const CDirectoryItem *>(pFile)->GetType() == HL_ITEM_FILE)
	{
		IStream *pTemp = 0;
		hlBool bResult = pPackage->CreateStream(static_cast<const CDirectoryFile *>(pFile), pTemp);
		*pStream = pTemp;

		return bResult;
	}

	return hlFalse;
}

HLLIB_API hlVoid hlPackageReleaseStream(HLStream *pStream)
{
	if(pPackage == 0)
	{
		return;
	}

	pPackage->ReleaseStream(static_cast<IStream *>(pStream));
}

HLLIB_API const hlChar *hlNCFFileGetRootPath()
{
	if(pPackage == 0 || pPackage->GetType() != HL_PACKAGE_NCF)
	{
		return 0;
	}

	return static_cast<CNCFFile *>(pPackage)->GetRootPath();
}

HLLIB_API hlVoid hlNCFFileSetRootPath(const hlChar *lpRootPath)
{
	if(pPackage == 0 || pPackage->GetType() != HL_PACKAGE_NCF)
	{
		return;
	}

	static_cast<CNCFFile *>(pPackage)->SetRootPath(lpRootPath);
}

HLLIB_API hlBool hlWADFileGetImageSizePaletted(const HLDirectoryItem *pFile, hlUInt *uiPaletteDataSize, hlUInt *uiPixelDataSize)
{
	if(pPackage == 0 || pPackage->GetType() != HL_PACKAGE_WAD)
	{
		return hlFalse;
	}

	return static_cast<CWADFile *>(pPackage)->GetImageSize(static_cast<const CDirectoryFile *>(pFile), *uiPaletteDataSize, *uiPixelDataSize);
}

HLLIB_API hlBool hlWADFileGetImageDataPaletted(const HLDirectoryItem *pFile, hlUInt *uiWidth, hlUInt *uiHeight, hlByte **lpPaletteData, hlByte **lpPixelData)
{
	if(pPackage == 0 || pPackage->GetType() != HL_PACKAGE_WAD)
	{
		return hlFalse;
	}

	return static_cast<CWADFile *>(pPackage)->GetImageData(static_cast<const CDirectoryFile *>(pFile), *uiWidth, *uiHeight, *lpPaletteData, *lpPixelData);
}

HLLIB_API hlBool hlWADFileGetImageSize(const HLDirectoryItem *pFile, hlUInt *uiPixelDataSize)
{
	if(pPackage == 0 || pPackage->GetType() != HL_PACKAGE_WAD)
	{
		return hlFalse;
	}

	return static_cast<CWADFile *>(pPackage)->GetImageSize(static_cast<const CDirectoryFile *>(pFile), *uiPixelDataSize);
}

HLLIB_API hlBool hlWADFileGetImageData(const HLDirectoryItem *pFile, hlUInt *uiWidth, hlUInt *uiHeight, hlByte **lpPixelData)
{
	if(pPackage == 0 || pPackage->GetType() != HL_PACKAGE_WAD)
	{
		return hlFalse;
	}

	return static_cast<CWADFile *>(pPackage)->GetImageData(static_cast<const CDirectoryFile *>(pFile), *uiWidth, *uiHeight, *lpPixelData);
}
