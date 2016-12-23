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
#include "FileMapping.h"

using namespace HLLib;
using namespace HLLib::Mapping;

#ifdef _WIN32
CFileMapping::CFileMapping(const hlChar *lpFileName) : hFile(0), hFileMapping(0), uiMode(HL_MODE_INVALID), lpView(0), uiViewSize(0)
#else
CFileMapping::CFileMapping(const hlChar *lpFileName) : iFile(-1), uiMode(HL_MODE_INVALID), lpView(0), uiViewSize(0)
#endif
{
	this->lpFileName = new hlChar[strlen(lpFileName) + 1];
	strcpy(this->lpFileName, lpFileName);

#ifdef _WIN32
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	this->uiAllocationGranularity = SystemInfo.dwAllocationGranularity;
#else
	this->uiAllocationGranularity = static_cast<hlUInt>(getpagesize());
#endif
}

CFileMapping::~CFileMapping()
{
	this->Close();

	delete []this->lpFileName;
}

HLMappingType CFileMapping::GetType() const
{
	return HL_MAPPING_FILE;
}

const hlChar *CFileMapping::GetFileName() const
{
	return this->lpFileName;
}

hlBool CFileMapping::GetOpened() const
{
#ifdef _WIN32
	return this->hFile != 0;
#else
	return this->iFile >= 0;
#endif
}

hlUInt CFileMapping::GetMode() const
{
	return this->uiMode;
}

hlBool CFileMapping::OpenInternal(hlUInt uiMode)
{
	assert(!this->GetOpened());

#ifdef _WIN32
	DWORD dwDesiredAccess = ((uiMode & HL_MODE_READ) ? GENERIC_READ : 0) | ((uiMode & HL_MODE_WRITE) ? GENERIC_WRITE : 0);
	DWORD dwShareMode = (uiMode & HL_MODE_VOLATILE) ? FILE_SHARE_READ | FILE_SHARE_WRITE : ((uiMode & HL_MODE_READ) && !(uiMode & HL_MODE_WRITE) ? FILE_SHARE_READ : 0);
	DWORD dwCreationDisposition = (uiMode & HL_MODE_WRITE) && (uiMode & HL_MODE_CREATE) ? (bOverwriteFiles ? CREATE_ALWAYS : CREATE_NEW) : ((uiMode & HL_MODE_READ) || (uiMode & HL_MODE_WRITE) ? OPEN_EXISTING : 0);

	if(dwDesiredAccess == 0 || dwCreationDisposition == 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);

		return hlFalse;
	}

	this->hFile = CreateFile(this->lpFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	if(this->hFile == INVALID_HANDLE_VALUE)
	{
		LastError.SetSystemErrorMessage("Error opening file.");

		this->hFile = 0;
		return hlFalse;
	}

	DWORD dwProtectionMode = (uiMode & HL_MODE_WRITE) ? PAGE_READWRITE : ((uiMode & HL_MODE_READ) ? PAGE_READONLY : 0);

	this->hFileMapping = CreateFileMapping(this->hFile, NULL, dwProtectionMode, 0, 0, NULL);

	if(this->hFileMapping == 0)
	{
		LastError.SetSystemErrorMessage("Failed to create file mapping object for file.");

		return hlFalse;
	}

	// Map the whole file to memory then pass pointers to the
	// master view back instead of mapping smaller views.
	if(uiMode & HL_MODE_QUICK_FILEMAPPING)
	{
		LARGE_INTEGER liFileSize;
#ifdef _WIN64
		if(GetFileSizeEx(this->hFile, &liFileSize))
#else
		if(GetFileSizeEx(this->hFile, &liFileSize) && liFileSize.HighPart == 0)
#endif
		{
			DWORD dwDesiredAccess = ((uiMode & HL_MODE_READ) ? FILE_MAP_READ : 0) | ((uiMode & HL_MODE_WRITE) ? FILE_MAP_WRITE : 0);

			this->uiViewSize = static_cast<hlULongLong>(liFileSize.QuadPart);
			this->lpView = MapViewOfFile(this->hFileMapping, dwDesiredAccess, 0, 0, static_cast<SIZE_T>(this->uiViewSize));

			if(this->lpView == 0)
			{
				LastError.SetSystemErrorMessage("Failed to map view of file. Try disabling quick file mapping.");

				return hlFalse;
			}
		}
		else
		{
			// Cannot map more than 4 GB on x86.
			uiMode &= ~HL_MODE_QUICK_FILEMAPPING;
		}
	}
#else
	hlInt iMode;
	
	if((uiMode & HL_MODE_READ) && (uiMode & HL_MODE_WRITE))
	{
		iMode = O_RDWR;
	}
	else if(uiMode & HL_MODE_READ)
	{
		iMode = O_RDONLY;
	}
	else if(uiMode & HL_MODE_WRITE)
	{
		iMode = O_WRONLY;
	}

	if((uiMode & HL_MODE_WRITE) && (uiMode & HL_MODE_CREATE))
	{
		iMode |= bOverwriteFiles ? O_CREAT | O_TRUNC : O_CREAT | O_EXCL;
	}

	if((uiMode & (HL_MODE_READ | HL_MODE_WRITE)) == 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);

		return hlFalse;
	}

	this->iFile = open(this->lpFileName, iMode | O_BINARY | O_RANDOM, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if(this->iFile < 0)
	{
		LastError.SetSystemErrorMessage("Error opening file.");

		this->iFile = -1;
		return hlFalse;
	}

	// Map the whole file to memory then pass pointers to the
	// master view back instead of mapping smaller views.
	if(uiMode & HL_MODE_QUICK_FILEMAPPING)
	{
		hlInt iProtection = ((uiMode & HL_MODE_READ) ? PROT_READ : 0) | ((uiMode & HL_MODE_WRITE) ? PROT_WRITE : 0);

		struct stat Stat;

		if(fstat(this->iFile, &Stat) < 0)
		{
			LastError.SetSystemErrorMessage("Error retrieving file size.");

			return hlFalse;
		}

		this->uiViewSize = (hlUInt)Stat.st_size;

		this->lpView = mmap(0, this->uiViewSize, iProtection, MAP_SHARED, this->iFile, 0);

		if(this->lpView == MAP_FAILED)
		{
			LastError.SetSystemErrorMessage("Failed to map view of file. Try disabling quick file mapping.");

			this->lpView = 0;
			return hlFalse;
		}
	}
#endif

	this->uiMode = uiMode;

	return hlTrue;
}

hlVoid CFileMapping::CloseInternal()
{
#ifdef _WIN32
	if(this->lpView != 0)
	{
		UnmapViewOfFile(this->lpView);
		this->lpView = 0;
	}

	this->uiViewSize = 0;

	if(this->hFileMapping != 0)
	{
		CloseHandle(this->hFileMapping);
		this->hFileMapping = 0;
	}

	if(this->hFile != 0)
	{
		CloseHandle(this->hFile);
		this->hFile = 0;
	}
#else
	if(this->lpView != 0)
	{
		munmap(this->lpView, this->uiViewSize);
		this->lpView = 0;
	}

	this->uiViewSize = 0;

	if(this->iFile >= 0)
	{
		close(this->iFile);
		this->iFile = -1;
	}
#endif
}

hlULongLong CFileMapping::GetMappingSize() const
{
	if(!this->GetOpened())
	{
		return 0;
	}

#ifdef _WIN32
	LARGE_INTEGER liFileSize;
	return GetFileSizeEx(this->hFile, &liFileSize) ? static_cast<hlULongLong>(liFileSize.QuadPart) : 0;
#else
	struct stat Stat;

	return fstat(this->iFile, &Stat) < 0 ? 0 : Stat.st_size;
#endif
}

hlBool CFileMapping::MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength)
{
	assert(this->GetOpened());

	if(this->lpView != 0)
	{
		if(uiOffset + uiLength > this->uiViewSize)
		{
#ifdef _WIN32
			LastError.SetErrorMessageFormated("Requested view (%I64u, %I64u) does not fit inside mapping, (%I64u, %I64u).", uiOffset, uiLength, 0ULL, this->uiViewSize);
#else
			LastError.SetErrorMessageFormated("Requested view (%llu, %llu) does not fit inside mapping, (%llu, %llu).", uiOffset, uiLength, 0ULL, this->uiViewSize);
#endif
			return hlFalse;
		}

		pView = new CView(this, (hlByte *)this->lpView, 0, this->uiViewSize, uiOffset, uiLength);
	}
	else
	{
		hlULongLong uiMappingSize = this->GetMappingSize();

		if(uiOffset + uiLength > uiMappingSize)
		{
#ifdef _WIN32
			LastError.SetErrorMessageFormated("Requested view (%I64u, %I64u) does not fit inside mapping, (%I64u, %I64u).", uiOffset, uiLength, 0ULL, this->uiViewSize);
#else
			LastError.SetErrorMessageFormated("Requested view (%llu, %llu) does not fit inside mapping, (%llu, %llu).", uiOffset, uiLength, 0ULL, this->uiViewSize);
#endif
			return hlFalse;
		}

		// Map multiples of the allocation granularity from the nearest allocation granularity
		// (for performance).
		hlULongLong uiGrainOffset = uiOffset % static_cast<hlULongLong>(this->uiAllocationGranularity);
		hlULongLong uiFileOffset = uiOffset - uiGrainOffset;
		hlULongLong uiFileLength = (((uiLength + uiGrainOffset + static_cast<hlULongLong>(this->uiAllocationGranularity) - 1) / static_cast<hlULongLong>(this->uiAllocationGranularity)) * static_cast<hlULongLong>(this->uiAllocationGranularity));

		if(uiFileOffset + uiFileLength > uiMappingSize)
		{
			uiFileLength = uiMappingSize - uiFileOffset;
		}

#ifdef _WIN32
		DWORD dwDesiredAccess = ((uiMode & HL_MODE_READ) ? FILE_MAP_READ : 0) | ((uiMode & HL_MODE_WRITE) ? FILE_MAP_WRITE : 0);

		hlVoid *lpView = MapViewOfFile(this->hFileMapping, dwDesiredAccess, static_cast<DWORD>(uiFileOffset >> 32), static_cast<DWORD>(uiFileOffset), static_cast<SIZE_T>(uiFileLength));

		if(lpView == 0)
		{
			LastError.SetSystemErrorMessage("Failed to map view of file. Try disabling file mapping.");
			return hlFalse;
		}
#else
		hlInt iProtection = ((uiMode & HL_MODE_READ) ? PROT_READ : 0) | ((uiMode & HL_MODE_WRITE) ? PROT_WRITE : 0);

		hlVoid *lpView = mmap(0, uiFileLength, iProtection, MAP_SHARED, this->iFile, uiFileOffset);

		if(this->lpView == MAP_FAILED)
		{
			LastError.SetSystemErrorMessage("Failed to map view of file. Try disabling file mapping.");
			return hlFalse;
		}
#endif

		pView = new CView(this, lpView, uiFileOffset, uiFileLength, uiGrainOffset, uiLength);
	}

	return hlTrue;
}

hlVoid CFileMapping::UnmapInternal(CView &View)
{
	assert(this->GetOpened());
	assert(View.GetMapping() == this);

	if(this->lpView == 0)
	{
#ifdef _WIN32
		UnmapViewOfFile((hlVoid *)View.GetAllocationView());
#else
		munmap((hlVoid *)View.GetAllocationView(), View.GetAllocationLength());
#endif
	}
}
