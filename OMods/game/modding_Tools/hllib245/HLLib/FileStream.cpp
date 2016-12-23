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
#include "FileStream.h"

using namespace HLLib;
using namespace HLLib::Streams;

#ifdef _WIN32
CFileStream::CFileStream(const hlChar *lpFileName) : hFile(0), uiMode(HL_MODE_INVALID)
#else
CFileStream::CFileStream(const hlChar *lpFileName) : iFile(-1), uiMode(HL_MODE_INVALID)
#endif
{
	this->lpFileName = new hlChar[strlen(lpFileName) + 1];
	strcpy(this->lpFileName, lpFileName);
}

CFileStream::~CFileStream()
{
	this->Close();

	delete []this->lpFileName;
}

HLStreamType CFileStream::GetType() const
{
	return HL_STREAM_FILE;
}

const hlChar *CFileStream::GetFileName() const
{
	return this->lpFileName;
}

hlBool CFileStream::GetOpened() const
{
#ifdef _WIN32
	return this->hFile != 0;
#else
	return this->iFile >= 0;
#endif
}

hlUInt CFileStream::GetMode() const
{
	return this->uiMode;
}

hlBool CFileStream::Open(hlUInt uiMode)
{
	this->Close();

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
#endif

	this->uiMode = uiMode;

	return hlTrue;
}

hlVoid CFileStream::Close()
{
	if(this->GetOpened())
	{
#ifdef _WIN32
		CloseHandle(this->hFile);
		this->hFile = 0;
#else
		close(this->iFile);
		this->iFile = -1;
#endif

		this->uiMode = HL_MODE_INVALID;
	}
}

hlULongLong CFileStream::GetStreamSize() const
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

hlULongLong CFileStream::GetStreamPointer() const
{
	if(!this->GetOpened())
	{
		return 0;
	}

#ifdef _WIN32
	LARGE_INTEGER liOffset;
	liOffset.QuadPart = 0;

	LARGE_INTEGER liPointer;
	return SetFilePointerEx(this->hFile, liOffset, &liPointer, FILE_CURRENT) ? static_cast<hlULongLong>(liPointer.QuadPart) : 0;
#else
	 return (hlUInt)lseek(this->iFile, 0, SEEK_CUR);
#endif
}

hlULongLong CFileStream::Seek(hlLongLong iOffset, HLSeekMode eSeekMode)
{
	if(!this->GetOpened())
	{
		return 0;
	}

#ifdef _WIN32
	DWORD dwMode = FILE_BEGIN;
	switch(eSeekMode)
	{
	/*case HL_SEEK_BEGINNING:
		dwMode = FILE_BEGIN;
		break;*/
	case HL_SEEK_CURRENT:
		dwMode = FILE_CURRENT;
		break;
	case HL_SEEK_END:
		dwMode = FILE_END;
		break;
	}

	LARGE_INTEGER liOffset;
	liOffset.QuadPart = iOffset;

	LARGE_INTEGER liPointer;
	return SetFilePointerEx(this->hFile, liOffset, &liPointer, dwMode) ? static_cast<hlULongLong>(liPointer.QuadPart) : 0;
#else
	hlInt iMode = SEEK_SET;
	switch(eSeekMode)
	{
	/*case HL_SEEK_BEGINNING:
		iMode = SEEK_SET;
		break;*/
	case HL_SEEK_CURRENT:
		iMode = SEEK_CUR;
		break;
	case HL_SEEK_END:
		iMode = SEEK_END;
		break;
	}

	return (hlUInt)lseek(this->iFile, iOffset, iMode);
#endif
}

hlBool CFileStream::Read(hlChar &cChar)
{
	if(!this->GetOpened())
	{
		return hlFalse;
	}

	if((this->uiMode & HL_MODE_READ) == 0)
	{
		LastError.SetErrorMessage("Stream not in read mode.");
		return hlFalse;
	}

#ifdef _WIN32
	hlULong ulBytesRead = 0;

	if(!ReadFile(this->hFile, &cChar, 1, &ulBytesRead, NULL))
	{
		LastError.SetSystemErrorMessage("ReadFile() failed.");
	}

	return ulBytesRead == 1;
#else
	hlInt iBytesRead = read(this->iFile, &cChar, 1);

	if(iBytesRead < 0)
	{
		LastError.SetSystemErrorMessage("read() failed.");
	}

	return iBytesRead == 1;
#endif
}

hlUInt CFileStream::Read(hlVoid *lpData, hlUInt uiBytes)
{
	if(!this->GetOpened())
	{
		return 0;
	}

	if((this->uiMode & HL_MODE_READ) == 0)
	{
		LastError.SetErrorMessage("Stream not in read mode.");
		return 0;
	}

#ifdef _WIN32
	hlULong ulBytesRead = 0;

	if(!ReadFile(this->hFile, lpData, uiBytes, &ulBytesRead, NULL))
	{
		LastError.SetSystemErrorMessage("ReadFile() failed.");
	}

	return (hlUInt)ulBytesRead;
#else
	hlInt iBytesRead = read(this->iFile, lpData, uiBytes);

	if(iBytesRead < 0)
	{
		LastError.SetSystemErrorMessage("read() failed.");
	}

	return (hlUInt)iBytesRead;
#endif
}

hlBool CFileStream::Write(hlChar cChar)
{
	if(!this->GetOpened())
	{
		return hlFalse;
	}

	if((this->uiMode & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessage("Stream not in write mode.");
		return hlFalse;
	}

#ifdef _WIN32
	hlULong ulBytesWritten = 0;

	if(!WriteFile(this->hFile, &cChar, 1, &ulBytesWritten, NULL))
	{
		LastError.SetSystemErrorMessage("WriteFile() failed.");
	}

	return ulBytesWritten == 1;
#else
	hlInt iBytesWritten = write(this->iFile, &cChar, 1);

	if(iBytesWritten < 0)
	{
		LastError.SetSystemErrorMessage("write() failed.");
	}

	return iBytesWritten == 1;
#endif
}

hlUInt CFileStream::Write(const hlVoid *lpData, hlUInt uiBytes)
{
	if(!this->GetOpened())
	{
		return 0;
	}

	if((this->uiMode & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessage("Stream not in write mode.");
		return 0;
	}

#ifdef _WIN32
	hlULong ulBytesWritten = 0;

	if(!WriteFile(this->hFile, lpData, uiBytes, &ulBytesWritten, NULL))
	{
		LastError.SetSystemErrorMessage("WriteFile() failed.");
	}

	return (hlUInt)ulBytesWritten;
#else
	hlInt iBytesWritten = write(this->iFile, lpData, uiBytes);

	if(iBytesWritten < 0)
	{
		LastError.SetSystemErrorMessage("write() failed.");
	}

	return (hlUInt)iBytesWritten;
#endif
}
