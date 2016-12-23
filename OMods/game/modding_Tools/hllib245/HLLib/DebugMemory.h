/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your Option) any later
 * version.
 */

#ifndef DEBUGMEMORY_H
#define DEBUGMEMORY_H

#ifdef _DEBUG
#	define DEBUG_TRACK_MEMORY 1
#else
#	define DEBUG_TRACK_MEMORY 0
#endif

#if DEBUG_TRACK_MEMORY

#	if _MSC_VER
#		define _CRT_SECURE_NO_WARNINGS
#		define _CRT_NONSTDC_NO_DEPRECATE
#	endif

#	include <memory>

void StartTracking();
void StopTracking();
void AddTrack(void *pAddress, size_t uiSize, const char *lpFile, unsigned int uiLine);
void RemoveTrack(void *pAddress);
void DumpTracks();

inline void *__cdecl operator new(size_t uiSize, const char *lpFile, int iLine)
{
	void *pAddress = (void *)malloc(uiSize);
	AddTrack(pAddress, uiSize, lpFile, iLine);
	return pAddress;
}

inline void __cdecl operator delete(void *pAddress, const char *lpFile, int iLine)
{
	if(pAddress != 0)
	{
		RemoveTrack(pAddress);
		free(pAddress);
	}
}

inline void __cdecl operator delete(void *pAddress)
{
	if(pAddress != 0)
	{
		RemoveTrack(pAddress);
		free(pAddress);
	}
}

inline void *__cdecl operator new[](size_t uiSize, const char *lpFile, int iLine)
{
	void *pAddress = (void *)malloc(uiSize);
	AddTrack(pAddress, uiSize, lpFile, iLine);
	return pAddress;
}

inline void __cdecl operator delete[](void *pAddress, const char *lpFile, int iLine)
{
	if(pAddress != 0)
	{
		RemoveTrack(pAddress);
		free(pAddress);
	}
}

inline void __cdecl operator delete[](void *pAddress)
{
	if(pAddress != 0)
	{
		RemoveTrack(pAddress);
		free(pAddress);
	}
}

#	define DEBUG_NEW new(__FILE__, __LINE__)
#	define new DEBUG_NEW

#	endif

#endif
