/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your Option) any later
 * version.
 */

#ifndef HLLIB_H
#define HLLIB_H

#include "stdafx.h"
#include "Error.h"
#include "Package.h"

namespace HLLib
{
	typedef std::vector<CPackage *> CPackageVector;

	extern hlBool bInitialized;
	extern CError LastError;

	extern POpenProc pOpenProc;
	extern PCloseProc pCloseProc;
	extern PReadProc pReadProc;
	extern PWriteProc pWriteProc;
	extern PSeekProc pSeekProc;
	extern PSeekExProc pSeekExProc;
	extern PTellProc pTellProc;
	extern PTellExProc pTellExProc;
	extern PSizeProc pSizeProc;
	extern PSizeExProc pSizeExProc;

	extern PExtractItemStartProc pExtractItemStartProc;
	extern PExtractItemEndProc pExtractItemEndProc;
	extern PExtractFileProgressProc pExtractFileProgressProc;
	extern PValidateFileProgressProc pValidateFileProgressProc;
	extern PDefragmentProgressProc pDefragmentProgressProc;
	extern PDefragmentProgressExProc pDefragmentProgressExProc;

	hlVoid hlExtractItemStart(const HLDirectoryItem *pItem);
	hlVoid hlExtractItemEnd(const HLDirectoryItem *pItem, hlBool bSuccess);
	hlVoid hlExtractFileProgress(const HLDirectoryItem *pFile, hlULongLong uiBytesExtracted, hlULongLong uiBytesTotal, hlBool *pCancel);
	hlVoid hlValidateFileProgress(const HLDirectoryItem *pFile, hlULongLong uiBytesValidated, hlULongLong uiBytesTotal, hlBool *pCancel);
	hlVoid hlDefragmentProgress(const HLDirectoryItem *pFile, hlUInt uiFilesDefragmented, hlUInt uiFilesTotal, hlULongLong uiBytesDefragmented, hlULongLong uiBytesTotal, hlBool *pCancel);

	extern CPackage *pPackage;
	extern CPackageVector *pPackageVector;

	extern hlBool bOverwriteFiles;
	extern hlBool bReadEncrypted;
	extern hlBool bForceDefragment;
}

#ifdef __cplusplus
extern "C" {
#endif

HLLIB_API hlVoid hlInitialize();
HLLIB_API hlVoid hlShutdown();

//
// Get/Set
//

HLLIB_API hlBool hlGetBoolean(HLOption eOption);
HLLIB_API hlBool hlGetBooleanValidate(HLOption eOption, hlBool *pValue);
HLLIB_API hlVoid hlSetBoolean(HLOption eOption, hlBool bValue);

HLLIB_API hlInt hlGetInteger(HLOption eOption);
HLLIB_API hlBool hlGetIntegerValidate(HLOption eOption, hlInt *pValue);
HLLIB_API hlVoid hlSetInteger(HLOption eOption, hlInt iValue);

HLLIB_API hlUInt hlGetUnsignedInteger(HLOption eOption);
HLLIB_API hlBool hlGetUnsignedIntegerValidate(HLOption eOption, hlUInt *pValue);
HLLIB_API hlVoid hlSetUnsignedInteger(HLOption eOption, hlUInt iValue);

HLLIB_API hlLongLong hlGetLongLong(HLOption eOption);
HLLIB_API hlBool hlGetLongLongValidate(HLOption eOption, hlLongLong *pValue);
HLLIB_API hlVoid hlSetLongLong(HLOption eOption, hlLongLong iValue);

HLLIB_API hlULongLong hlGetUnsignedLongLong(HLOption eOption);
HLLIB_API hlBool hlGetUnsignedLongLongValidate(HLOption eOption, hlULongLong *pValue);
HLLIB_API hlVoid hlSetUnsignedLongLong(HLOption eOption, hlULongLong iValue);

HLLIB_API hlFloat hlGetFloat(HLOption eOption);
HLLIB_API hlBool hlGetFloatValidate(HLOption eOption, hlFloat *pValue);
HLLIB_API hlVoid hlSetFloat(HLOption eOption, hlFloat fValue);

HLLIB_API const hlChar *hlGetString(HLOption eOption);
HLLIB_API hlBool hlGetStringValidate(HLOption eOption, const hlChar **pValue);
HLLIB_API hlVoid hlSetString(HLOption eOption, const hlChar *lpValue);

HLLIB_API const hlVoid *hlGetVoid(HLOption eOption);
HLLIB_API hlBool hlGetVoidValidate(HLOption eOption, const hlVoid **pValue);
HLLIB_API hlVoid hlSetVoid(HLOption eOption, const hlVoid *pValue);

//
// Attributes
//

HLLIB_API hlBool hlAttributeGetBoolean(HLAttribute *pAttribute);
HLLIB_API hlVoid hlAttributeSetBoolean(HLAttribute *pAttribute, const hlChar *lpName, hlBool bValue);

HLLIB_API hlInt hlAttributeGetInteger(HLAttribute *pAttribute);
HLLIB_API hlVoid hlAttributeSetInteger(HLAttribute *pAttribute, const hlChar *lpName, hlInt iValue);

HLLIB_API hlUInt hlAttributeGetUnsignedInteger(HLAttribute *pAttribute);
HLLIB_API hlVoid hlAttributeSetUnsignedInteger(HLAttribute *pAttribute, const hlChar *lpName, hlUInt uiValue, hlBool bHexadecimal);

HLLIB_API hlFloat hlAttributeGetFloat(HLAttribute *pAttribute);
HLLIB_API hlVoid hlAttributeSetFloat(HLAttribute *pAttribute, const hlChar *lpName, hlFloat fValue);

HLLIB_API const hlChar *hlAttributeGetString(HLAttribute *pAttribute);
HLLIB_API hlVoid hlAttributeSetString(HLAttribute *pAttribute, const hlChar *lpName, const hlChar *lpValue);

#ifdef __cplusplus
}
#endif

#endif
