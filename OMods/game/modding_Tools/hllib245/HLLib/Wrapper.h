/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef WRAPPER_H
#define WRAPPER_H

#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Directory Item
//

HLLIB_API HLDirectoryItemType hlItemGetType(const HLDirectoryItem *pItem);

HLLIB_API const hlChar *hlItemGetName(const HLDirectoryItem *pItem);
HLLIB_API hlUInt hlItemGetID(const HLDirectoryItem *pItem);
HLLIB_API const hlVoid *hlItemGetData(const HLDirectoryItem *pItem);

HLLIB_API hlUInt hlItemGetPackage(const HLDirectoryItem *pItem);
HLLIB_API HLDirectoryItem *hlItemGetParent(HLDirectoryItem *pItem);

HLLIB_API hlBool hlItemGetSize(const HLDirectoryItem *pItem, hlUInt *pSize);
HLLIB_API hlBool hlItemGetSizeEx(const HLDirectoryItem *pItem, hlULongLong *pSize);
HLLIB_API hlBool hlItemGetSizeOnDisk(const HLDirectoryItem *pItem, hlUInt *pSize);
HLLIB_API hlBool hlItemGetSizeOnDiskEx(const HLDirectoryItem *pItem, hlULongLong *pSize);

HLLIB_API hlVoid hlItemGetPath(const HLDirectoryItem *pItem, hlChar *lpPath, hlUInt uiPathSize);
HLLIB_API hlBool hlItemExtract(HLDirectoryItem *pItem, const hlChar *lpPath);

//
// Directory Folder
//

HLLIB_API hlUInt hlFolderGetCount(const HLDirectoryItem *pItem);

HLLIB_API HLDirectoryItem *hlFolderGetItem(HLDirectoryItem *pItem, hlUInt uiIndex);
HLLIB_API HLDirectoryItem *hlFolderGetItemByName(HLDirectoryItem *pItem, const hlChar *lpName, HLFindType eFind);
HLLIB_API HLDirectoryItem *hlFolderGetItemByPath(HLDirectoryItem *pItem, const hlChar *lpPath, HLFindType eFind);

HLLIB_API hlVoid hlFolderSort(HLDirectoryItem *pItem, HLSortField eField, HLSortOrder eOrder, hlBool bRecurse);

HLLIB_API HLDirectoryItem *hlFolderFindFirst(HLDirectoryItem *pFolder, const hlChar *lpSearch, HLFindType eFind);
HLLIB_API HLDirectoryItem *hlFolderFindNext(HLDirectoryItem *pFolder, HLDirectoryItem *pItem, const hlChar *lpSearch, HLFindType eFind);

HLLIB_API hlUInt hlFolderGetSize(const HLDirectoryItem *pItem, hlBool bRecurse);
HLLIB_API hlULongLong hlFolderGetSizeEx(const HLDirectoryItem *pItem, hlBool bRecurse);
HLLIB_API hlUInt hlFolderGetSizeOnDisk(const HLDirectoryItem *pItem, hlBool bRecurse);
HLLIB_API hlULongLong hlFolderGetSizeOnDiskEx(const HLDirectoryItem *pItem, hlBool bRecurse);
HLLIB_API hlUInt hlFolderGetFolderCount(const HLDirectoryItem *pItem, hlBool bRecurse);
HLLIB_API hlUInt hlFolderGetFileCount(const HLDirectoryItem *pItem, hlBool bRecurse);

//
// Directory File
//

HLLIB_API hlUInt hlFileGetExtractable(const HLDirectoryItem *pItem);
HLLIB_API HLValidation hlFileGetValidation(const HLDirectoryItem *pItem);
HLLIB_API hlUInt hlFileGetSize(const HLDirectoryItem *pItem);
HLLIB_API hlUInt hlFileGetSizeOnDisk(const HLDirectoryItem *pItem);

HLLIB_API hlBool hlFileCreateStream(HLDirectoryItem *pItem, HLStream **pStream);
HLLIB_API hlVoid hlFileReleaseStream(HLDirectoryItem *pItem, HLStream *pStream);

//
// Stream
//

HLLIB_API HLStreamType hlStreamGetType(const HLStream *pStream);

HLLIB_API hlBool hlStreamGetOpened(const HLStream *pStream);
HLLIB_API hlUInt hlStreamGetMode(const HLStream *pStream);

HLLIB_API hlBool hlStreamOpen(HLStream *pStream, hlUInt uiMode);
HLLIB_API hlVoid hlStreamClose(HLStream *pStream);

HLLIB_API hlUInt hlStreamGetStreamSize(const HLStream *pStream);
HLLIB_API hlULongLong hlStreamGetStreamSizeEx(const HLStream *pStream);
HLLIB_API hlUInt hlStreamGetStreamPointer(const HLStream *pStream);
HLLIB_API hlULongLong hlStreamGetStreamPointerEx(const HLStream *pStream);

HLLIB_API hlUInt hlStreamSeek(HLStream *pStream, hlLongLong iOffset, HLSeekMode eSeekMode);
HLLIB_API hlULongLong hlStreamSeekEx(HLStream *pStream, hlLongLong iOffset, HLSeekMode eSeekMode);

HLLIB_API hlBool hlStreamReadChar(HLStream *pStream, hlChar *pChar);
HLLIB_API hlUInt hlStreamRead(HLStream *pStream, hlVoid *lpData, hlUInt uiBytes);

HLLIB_API hlBool hlStreamWriteChar(HLStream *pStream, hlChar iChar);
HLLIB_API hlUInt hlStreamWrite(HLStream *pStream, const hlVoid *lpData, hlUInt uiBytes);

//
// Package
//

HLLIB_API hlBool hlBindPackage(hlUInt uiPackage);

HLLIB_API HLPackageType hlGetPackageTypeFromName(const hlChar *lpName);
HLLIB_API HLPackageType hlGetPackageTypeFromMemory(const hlVoid *lpBuffer, hlUInt uiBufferSize);
HLLIB_API HLPackageType hlGetPackageTypeFromStream(HLStream *pStream);
HLLIB_API hlBool hlCreatePackage(HLPackageType ePackageType, hlUInt *uiPackage);
HLLIB_API hlVoid hlDeletePackage(hlUInt uiPackage);

HLLIB_API HLPackageType hlPackageGetType();
HLLIB_API const hlChar *hlPackageGetExtension();
HLLIB_API const hlChar *hlPackageGetDescription();

HLLIB_API hlBool hlPackageGetOpened();

HLLIB_API hlBool hlPackageOpenFile(const hlChar *lpFileName, hlUInt uiMode);
HLLIB_API hlBool hlPackageOpenMemory(hlVoid *lpData, hlUInt uiBufferSize, hlUInt uiMode);
HLLIB_API hlBool hlPackageOpenProc(hlVoid *pUserData, hlUInt uiMode);
HLLIB_API hlBool hlPackageOpenStream(HLStream *pStream, hlUInt uiMode);
HLLIB_API hlVoid hlPackageClose();

HLLIB_API hlBool hlPackageDefragment();

HLLIB_API HLDirectoryItem *hlPackageGetRoot();

HLLIB_API hlUInt hlPackageGetAttributeCount();
HLLIB_API const hlChar *hlPackageGetAttributeName(HLPackageAttribute eAttribute);
HLLIB_API hlBool hlPackageGetAttribute(HLPackageAttribute eAttribute, HLAttribute *pAttribute);

HLLIB_API hlUInt hlPackageGetItemAttributeCount();
HLLIB_API const hlChar *hlPackageGetItemAttributeName(HLPackageAttribute eAttribute);
HLLIB_API hlBool hlPackageGetItemAttribute(const HLDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute *pAttribute);

HLLIB_API hlBool hlPackageGetExtractable(const HLDirectoryItem *pFile, hlBool *pExtractable);
HLLIB_API hlBool hlPackageGetFileSize(const HLDirectoryItem *pFile, hlUInt *pSize);
HLLIB_API hlBool hlPackageGetFileSizeOnDisk(const HLDirectoryItem *pFile, hlUInt *pSize);
HLLIB_API hlBool hlPackageCreateStream(const HLDirectoryItem *pFile, HLStream **pStream);
HLLIB_API hlVoid hlPackageReleaseStream(HLStream *pStream);

HLLIB_API const hlChar *hlNCFFileGetRootPath();
HLLIB_API hlVoid hlNCFFileSetRootPath(const hlChar *lpRootPath);

HLLIB_API hlBool hlWADFileGetImageSizePaletted(const HLDirectoryItem *pFile, hlUInt *uiPaletteDataSize, hlUInt *uiPixelDataSize);
HLLIB_API hlBool hlWADFileGetImageDataPaletted(const HLDirectoryItem *pFile, hlUInt *uiWidth, hlUInt *uiHeight, hlByte **lpPaletteData, hlByte **lpPixelData);
HLLIB_API hlBool hlWADFileGetImageSize(const HLDirectoryItem *pFile, hlUInt *uiPixelDataSize);
HLLIB_API hlBool hlWADFileGetImageData(const HLDirectoryItem *pFile, hlUInt *uiWidth, hlUInt *uiHeight, hlByte **lpPixelData);

#ifdef __cplusplus
}
#endif

#endif
