/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your Option) any later
 * version.
 */

#include "HLLib.h"

using namespace HLLib;

namespace HLLib
{
	hlBool bInitialized = hlFalse;
	CError LastError = CError();

	POpenProc pOpenProc = 0;
	PCloseProc pCloseProc = 0;
	PReadProc pReadProc = 0;
	PWriteProc pWriteProc = 0;
	PSeekProc pSeekProc = 0;
	PSeekExProc pSeekExProc = 0;
	PTellProc pTellProc = 0;
	PTellExProc pTellExProc = 0;
	PSizeProc pSizeProc = 0;
	PSizeExProc pSizeExProc = 0;

	PExtractItemStartProc pExtractItemStartProc = 0;
	PExtractItemEndProc pExtractItemEndProc = 0;
	PExtractFileProgressProc pExtractFileProgressProc = 0;
	PValidateFileProgressProc pValidateFileProgressProc = 0;
	PDefragmentProgressProc pDefragmentProgressProc = 0;
	PDefragmentProgressExProc pDefragmentProgressExProc = 0;

	CPackage *pPackage = 0;
	CPackageVector *pPackageVector = 0;

	hlBool bOverwriteFiles = hlTrue;
	hlBool bReadEncrypted = hlTrue;
	hlBool bForceDefragment = hlFalse;

	hlVoid hlExtractItemStart(const HLDirectoryItem *pItem)
	{
		if(pExtractItemStartProc != 0)
		{
			pExtractItemStartProc(pItem);
		}
	}

	hlVoid hlExtractItemEnd(const HLDirectoryItem *pItem, hlBool bSuccess)
	{
		if(pExtractItemEndProc != 0)
		{
			pExtractItemEndProc(pItem, bSuccess);
		}
	}

	hlVoid hlExtractFileProgress(const HLDirectoryItem *pFile, hlULongLong uiBytesExtracted, hlULongLong uiBytesTotal, hlBool *pCancel)
	{
		if(pExtractFileProgressProc)
		{
			pExtractFileProgressProc(pFile, static_cast<hlUInt>(uiBytesExtracted), static_cast<hlUInt>(uiBytesTotal), pCancel);
		}
	}

	hlVoid hlValidateFileProgress(const HLDirectoryItem *pFile, hlULongLong uiBytesValidated, hlULongLong uiBytesTotal, hlBool *pCancel)
	{
		if(pValidateFileProgressProc)
		{
			pValidateFileProgressProc(pFile, static_cast<hlUInt>(uiBytesValidated), static_cast<hlUInt>(uiBytesTotal), pCancel);
		}
	}

	hlVoid hlDefragmentProgress(const HLDirectoryItem *pFile, hlUInt uiFilesDefragmented, hlUInt uiFilesTotal, hlULongLong uiBytesDefragmented, hlULongLong uiBytesTotal, hlBool *pCancel)
	{
		if(pDefragmentProgressProc)
		{
			pDefragmentProgressProc(pFile, uiFilesDefragmented, uiFilesTotal, static_cast<hlUInt>(uiBytesDefragmented), static_cast<hlUInt>(uiBytesTotal), pCancel);
		}
		if(pDefragmentProgressExProc)
		{
			pDefragmentProgressExProc(pFile, uiFilesDefragmented, uiFilesTotal, uiBytesDefragmented, uiBytesTotal, pCancel);
		}
	}
}

//
// hlInitialize()
// Initializes all resources.
//
HLLIB_API hlVoid hlInitialize()
{
	if(bInitialized)
	{
		return;
	}

	bInitialized = hlTrue;
	LastError = CError();

	pPackage = 0;
	pPackageVector = new CPackageVector();

	return;
}

//
// hlShutdown()
// Frees all resources.
//
HLLIB_API hlVoid hlShutdown()
{
	if(!bInitialized)
		return;

	bInitialized = hlFalse;

	pPackage = 0;

	for(hlUInt i = 0; i < pPackageVector->size(); i++)
	{
		delete (*pPackageVector)[i];
	}

	delete pPackageVector;
	pPackageVector = 0;
}

HLLIB_API hlBool hlGetBoolean(HLOption eOption)
{
	hlBool bValue = hlFalse;
	hlGetBooleanValidate(eOption, &bValue);
	return bValue;
}

HLLIB_API hlBool hlGetBooleanValidate(HLOption eOption, hlBool *pValue)
{
	switch(eOption)
	{
	case HL_OVERWRITE_FILES:
		*pValue = bOverwriteFiles;
		return hlTrue;
	case HL_READ_ENCRYPTED:
		*pValue = bReadEncrypted;
		return hlTrue;
	case HL_FORCE_DEFRAGMENT:
		*pValue = bForceDefragment;
		return hlTrue;
	case HL_PACKAGE_BOUND:
		*pValue = pPackage != 0;
		return hlTrue;
	}

	return hlFalse;
}

HLLIB_API hlVoid hlSetBoolean(HLOption eOption, hlBool bValue)
{
	switch(eOption)
	{
	case HL_OVERWRITE_FILES:
		bOverwriteFiles = bValue;
		break;
	case HL_READ_ENCRYPTED:
		bReadEncrypted = bValue;
		break;
	case HL_FORCE_DEFRAGMENT:
		bForceDefragment = bValue;
		break;
	}
}

HLLIB_API hlInt hlGetInteger(HLOption eOption)
{
	hlInt iValue = 0;
	hlGetIntegerValidate(eOption, &iValue);
	return iValue;
}

HLLIB_API hlBool hlGetIntegerValidate(HLOption eOption, hlInt *pValue)
{
	hlUInt uiValue = (hlUInt)*pValue;
	hlBool bResult = hlGetUnsignedIntegerValidate(eOption, &uiValue);
	*pValue = (hlInt)uiValue;
	return bResult;
}

HLLIB_API hlVoid hlSetInteger(HLOption eOption, hlInt iValue)
{

}

HLLIB_API hlUInt hlGetUnsignedInteger(HLOption eOption)
{
	hlUInt uiValue = 0;
	hlGetUnsignedIntegerValidate(eOption, &uiValue);
	return uiValue;
}

HLLIB_API hlBool hlGetUnsignedIntegerValidate(HLOption eOption, hlUInt *pValue)
{
	switch(eOption)
	{
	case HL_VERSION:
		*pValue = HL_VERSION_NUMBER;
		return hlTrue;
	case HL_ERROR_SYSTEM:
		*pValue = LastError.GetSystemError();
		return hlTrue;
	case HL_PACKAGE_ID:
		*pValue = HL_ID_INVALID;
		if(pPackage != 0)
		{
			for(hlUInt i = 0; i < static_cast<hlUInt>(pPackageVector->size()); i++)
			{
				if((*pPackageVector)[i] == pPackage)
				{
					*pValue = i;
					break;
				}
			}
		}
		return hlTrue;
		break;
	case HL_PACKAGE_SIZE:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = static_cast<hlUInt>(pPackage->GetMapping()->GetMappingSize());
		return hlTrue;
		break;
	case HL_PACKAGE_TOTAL_ALLOCATIONS:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = pPackage->GetMapping()->GetTotalAllocations();
		return hlTrue;
		break;
	case HL_PACKAGE_TOTAL_MEMORY_ALLOCATED:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = static_cast<hlUInt>(pPackage->GetMapping()->GetTotalMemoryAllocated());
		return hlTrue;
		break;
	case HL_PACKAGE_TOTAL_MEMORY_USED:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = static_cast<hlUInt>(pPackage->GetMapping()->GetTotalMemoryUsed());
		return hlTrue;
		break;
	default:
		return hlFalse;
	}
}

HLLIB_API hlVoid hlSetUnsignedInteger(HLOption eOption, hlUInt iValue)
{

}

HLLIB_API hlLongLong hlGetLongLong(HLOption eOption)
{
	hlLongLong iValue = 0;
	hlGetLongLongValidate(eOption, &iValue);
	return iValue;
}

HLLIB_API hlBool hlGetLongLongValidate(HLOption eOption, hlLongLong *pValue)
{
	hlULongLong uiValue = (hlLongLong)*pValue;
	hlBool bResult = hlGetUnsignedLongLongValidate(eOption, &uiValue);
	*pValue = (hlLongLong)uiValue;
	return bResult;
}

HLLIB_API hlVoid hlSetLongLong(HLOption eOption, hlLongLong iValue)
{

}

HLLIB_API hlULongLong hlGetUnsignedLongLong(HLOption eOption)
{
	hlULongLong uiValue = 0;
	hlGetUnsignedLongLongValidate(eOption, &uiValue);
	return uiValue;
}

HLLIB_API hlBool hlGetUnsignedLongLongValidate(HLOption eOption, hlULongLong *pValue)
{
	switch(eOption)
	{
	case HL_PACKAGE_ID:
		*pValue = HL_ID_INVALID;
		if(pPackage != 0)
		{
			for(hlUInt i = 0; i < static_cast<hlUInt>(pPackageVector->size()); i++)
			{
				if((*pPackageVector)[i] == pPackage)
				{
					*pValue = static_cast<hlULongLong>(i);
					break;
				}
			}
		}
		return hlTrue;
		break;
	case HL_PACKAGE_SIZE:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = pPackage->GetMapping()->GetMappingSize();
		return hlTrue;
		break;
	case HL_PACKAGE_TOTAL_ALLOCATIONS:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = static_cast<hlULongLong>(pPackage->GetMapping()->GetTotalAllocations());
		return hlTrue;
		break;
	case HL_PACKAGE_TOTAL_MEMORY_ALLOCATED:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = pPackage->GetMapping()->GetTotalMemoryAllocated();
		return hlTrue;
		break;
	case HL_PACKAGE_TOTAL_MEMORY_USED:
		if(pPackage == 0 || !pPackage->GetOpened() || !pPackage->GetMapping())
		{
			return hlFalse;
		}

		*pValue = pPackage->GetMapping()->GetTotalMemoryUsed();
		return hlTrue;
		break;
	default:
		return hlFalse;
	}
}

HLLIB_API hlVoid hlSetUnsignedLongLong(HLOption eOption, hlULongLong iValue)
{

}

HLLIB_API hlFloat hlGetFloat(HLOption eOption)
{
	hlFloat fValue = 0.0f;
	hlGetFloatValidate(eOption, &fValue);
	return fValue;
}

HLLIB_API hlBool hlGetFloatValidate(HLOption eOption, hlFloat *pValue)
{
	return hlFalse;
}

HLLIB_API hlVoid hlSetFloat(HLOption eOption, hlFloat fValue)
{
	
}

HLLIB_API const hlChar *hlGetString(HLOption eOption)
{
	const hlChar *lpValue = 0;
	hlGetStringValidate(eOption, &lpValue);
	return lpValue ? lpValue : "";
}

HLLIB_API hlBool hlGetStringValidate(HLOption eOption, const hlChar **pValue)
{
	switch(eOption)
	{
	case HL_VERSION:
		*pValue = HL_VERSION_STRING;
		return hlTrue;
	case HL_ERROR:
		*pValue = LastError.GetErrorMessage();
		return hlTrue;
	case HL_ERROR_SYSTEM:
		*pValue = LastError.GetSystemErrorMessage();
		return hlTrue;
	case HL_ERROR_SHORT_FORMATED:
		*pValue = LastError.GetShortFormattedErrorMessage();
		return hlTrue;
	case HL_ERROR_LONG_FORMATED:
		*pValue = LastError.GetLongFormattedErrorMessage();
		return hlTrue;
	default:
		return hlFalse;
	}
}

HLLIB_API hlVoid hlSetString(HLOption eOption, const hlChar *lpValue)
{

}

HLLIB_API const hlVoid *hlGetVoid(HLOption eOption)
{
	const hlVoid *lpValue = 0;
	hlGetVoidValidate(eOption, &lpValue);
	return lpValue;
}

HLLIB_API hlBool hlGetVoidValidate(HLOption eOption, const hlVoid **pValue)
{
	switch(eOption)
	{
	case HL_PROC_OPEN:
		*pValue = (const hlVoid *)pOpenProc;
		return hlTrue;
	case HL_PROC_CLOSE:
		*pValue = (const hlVoid *)pCloseProc;
		return hlTrue;
	case HL_PROC_READ:
		*pValue = (const hlVoid *)pReadProc;
		return hlTrue;
	case HL_PROC_WRITE:
		*pValue = (const hlVoid *)pWriteProc;
		return hlTrue;
	case HL_PROC_SEEK:
		*pValue = (const hlVoid *)pSeekProc;
		return hlTrue;
	case HL_PROC_SEEK_EX:
		*pValue = (const hlVoid *)pSeekExProc;
		return hlTrue;
	case HL_PROC_TELL:
		*pValue = (const hlVoid *)pTellProc;
		return hlTrue;
	case HL_PROC_TELL_EX:
		*pValue = (const hlVoid *)pTellExProc;
		return hlTrue;
	case HL_PROC_SIZE:
		*pValue = (const hlVoid *)pSizeProc;
		return hlTrue;
	case HL_PROC_SIZE_EX:
		*pValue = (const hlVoid *)pSizeExProc;
		return hlTrue;
	case HL_PROC_EXTRACT_ITEM_START:
		*pValue = (const hlVoid *)pExtractItemStartProc;
		return hlTrue;
	case HL_PROC_EXTRACT_ITEM_END:
		*pValue = (const hlVoid *)pExtractItemEndProc;
		return hlTrue;
	case HL_PROC_EXTRACT_FILE_PROGRESS:
		*pValue = (const hlVoid *)pExtractFileProgressProc;
		return hlTrue;
	case HL_PROC_VALIDATE_FILE_PROGRESS:
		*pValue = (const hlVoid *)pValidateFileProgressProc;
		return hlTrue;
	case HL_PROC_DEFRAGMENT_PROGRESS:
		*pValue = (const hlVoid *)pDefragmentProgressProc;
		return hlTrue;
	case HL_PROC_DEFRAGMENT_PROGRESS_EX:
		*pValue = (const hlVoid *)pDefragmentProgressExProc;
		return hlTrue;
	default:
		return hlFalse;
	}
}

HLLIB_API hlVoid hlSetVoid(HLOption eOption, const hlVoid *pValue)
{
	switch(eOption)
	{
	case HL_PROC_OPEN:
		pOpenProc = (POpenProc)pValue;
		break;
	case HL_PROC_CLOSE:
		pCloseProc = (PCloseProc)pValue;
		break;
	case HL_PROC_READ:
		pReadProc = (PReadProc)pValue;
		break;
	case HL_PROC_WRITE:
		pWriteProc = (PWriteProc)pValue;
		break;
	case HL_PROC_SEEK:
		pSeekProc = (PSeekProc)pValue;
		break;
	case HL_PROC_SEEK_EX:
		pSeekExProc = (PSeekExProc)pValue;
		break;
	case HL_PROC_TELL:
		pTellProc = (PTellProc)pValue;
		break;
	case HL_PROC_TELL_EX:
		pTellExProc = (PTellExProc)pValue;
		break;
	case HL_PROC_SIZE:
		pSizeProc = (PSizeProc)pValue;
		break;
	case HL_PROC_SIZE_EX:
		pSizeExProc = (PSizeExProc)pValue;
		break;
	case HL_PROC_EXTRACT_ITEM_START:
		pExtractItemStartProc = (PExtractItemStartProc)pValue;
		break;
	case HL_PROC_EXTRACT_ITEM_END:
		pExtractItemEndProc = (PExtractItemEndProc)pValue;
		break;
	case HL_PROC_EXTRACT_FILE_PROGRESS:
		pExtractFileProgressProc = (PExtractFileProgressProc)pValue;
		break;
	case HL_PROC_VALIDATE_FILE_PROGRESS:
		pValidateFileProgressProc = (PValidateFileProgressProc)pValue;
		break;
	case HL_PROC_DEFRAGMENT_PROGRESS:
		pDefragmentProgressProc = (PDefragmentProgressProc)pValue;
		break;
	case HL_PROC_DEFRAGMENT_PROGRESS_EX:
		pDefragmentProgressExProc = (PDefragmentProgressExProc)pValue;
		break;
	}
}

HLLIB_API hlBool hlAttributeGetBoolean(HLAttribute *pAttribute)
{
	if(pAttribute->eAttributeType != HL_ATTRIBUTE_BOOLEAN)
	{
		return hlFalse;
	}

	return pAttribute->Value.Boolean.bValue;
}

HLLIB_API hlVoid hlAttributeSetBoolean(HLAttribute *pAttribute, const hlChar *lpName, hlBool bValue)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_BOOLEAN;
	if(lpName != 0)
	{
		strncpy(pAttribute->lpName, lpName, sizeof(pAttribute->lpName));
		pAttribute->lpName[sizeof(pAttribute->lpName) - 1] = '\0';
	}
	pAttribute->Value.Boolean.bValue = bValue;
}

HLLIB_API hlInt hlAttributeGetInteger(HLAttribute *pAttribute)
{
	if(pAttribute->eAttributeType != HL_ATTRIBUTE_INTEGER)
	{
		return 0;
	}

	return pAttribute->Value.Integer.iValue;
}

HLLIB_API hlVoid hlAttributeSetInteger(HLAttribute *pAttribute, const hlChar *lpName, hlInt iValue)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_INTEGER;
	if(lpName != 0)
	{
		strncpy(pAttribute->lpName, lpName, sizeof(pAttribute->lpName));
		pAttribute->lpName[sizeof(pAttribute->lpName) - 1] = '\0';
	}
	pAttribute->Value.Integer.iValue = iValue;
}

HLLIB_API hlUInt hlAttributeGetUnsignedInteger(HLAttribute *pAttribute)
{
	if(pAttribute->eAttributeType != HL_ATTRIBUTE_UNSIGNED_INTEGER)
	{
		return 0;
	}

	return pAttribute->Value.UnsignedInteger.uiValue;
}

HLLIB_API hlVoid hlAttributeSetUnsignedInteger(HLAttribute *pAttribute, const hlChar *lpName, hlUInt uiValue, hlBool bHexadecimal)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_UNSIGNED_INTEGER;
	if(lpName != 0)
	{
		strncpy(pAttribute->lpName, lpName, sizeof(pAttribute->lpName));
		pAttribute->lpName[sizeof(pAttribute->lpName) - 1] = '\0';
	}
	pAttribute->Value.UnsignedInteger.uiValue = uiValue;
	pAttribute->Value.UnsignedInteger.bHexadecimal = bHexadecimal;
}

HLLIB_API hlFloat hlAttributeGetFloat(HLAttribute *pAttribute)
{
	if(pAttribute->eAttributeType != HL_ATTRIBUTE_FLOAT)
	{
		return 0.0f;
	}

	return pAttribute->Value.Float.fValue;
}

HLLIB_API hlVoid hlAttributeSetFloat(HLAttribute *pAttribute, const hlChar *lpName, hlFloat fValue)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_FLOAT;
	if(lpName != 0)
	{
		strncpy(pAttribute->lpName, lpName, sizeof(pAttribute->lpName));
		pAttribute->lpName[sizeof(pAttribute->lpName) - 1] = '\0';
	}
	pAttribute->Value.Float.fValue = fValue;
}

HLLIB_API const hlChar *hlAttributeGetString(HLAttribute *pAttribute)
{
	if(pAttribute->eAttributeType != HL_ATTRIBUTE_STRING)
	{
		return "";
	}

	return pAttribute->Value.String.lpValue;
}

HLLIB_API hlVoid hlAttributeSetString(HLAttribute *pAttribute, const hlChar *lpName, const hlChar *lpValue)
{
	pAttribute->eAttributeType = HL_ATTRIBUTE_STRING;
	if(lpName != 0)
	{
		strncpy(pAttribute->lpName, lpName, sizeof(pAttribute->lpName));
		pAttribute->lpName[sizeof(pAttribute->lpName) - 1] = '\0';
	}
	if(lpValue != 0)
	{
		strncpy(pAttribute->Value.String.lpValue, lpValue, sizeof(pAttribute->Value.String.lpValue));
		pAttribute->Value.String.lpValue[sizeof(pAttribute->Value.String.lpValue) - 1] = '\0';
	}
	else
	{
		*pAttribute->Value.String.lpValue = '\0';
	}
}


#ifdef _WIN32
//
// DllMain()
// DLL entry point.
//
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		#if DEBUG_TRACK_MEMORY
			StartTracking();
		#endif
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		hlShutdown();

		#if DEBUG_TRACK_MEMORY
			StopTracking();
			DumpTracks();
		#endif
		break;
	}
    return TRUE;
}
#endif
