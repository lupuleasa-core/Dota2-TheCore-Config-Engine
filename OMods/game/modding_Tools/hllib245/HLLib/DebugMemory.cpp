/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "DebugMemory.h"

#if DEBUG_TRACK_MEMORY

#define DEBUG_TRACK_MEMORY_HASH 4				// Number of hashmap buckets.  (2^Value)
#define DEBUG_TRACK_MEMORY_ALIGNMENT 4			// Number of least significant bits to ignore in hash.  (For memory alignment.)
#ifdef _WIN32
#	define DEBUG_TRACK_DUMP_FILE "C:\\dump.txt"	// File to dump memory dump to.
#else
#	define DEBUG_TRACK_DUMP_FILE "/var/tmp/dump.txt"	// File to dump memory dump to.
#endif

struct SAllocInfo
{
	void			*pAddress;
	size_t			uiSize;
	char			lpFile[64];
	unsigned int	uiLine;
	SAllocInfo		*pNext;
};

struct SAllocList
{
	SAllocList() : uiCount(0), pFirst(0)
	{

	}

	unsigned int	uiCount;
	SAllocInfo		*pFirst;
};

bool bTrackMemory = false;

unsigned int uiTotalTracks = 0;
unsigned int uiPeakTracks = 0;

size_t uiTotalBytes = 0;
size_t uiPeakBytes = 0;

SAllocList lpAllocList[1 << DEBUG_TRACK_MEMORY_HASH];

// Disable void* to unsigned long cast warning.
#pragma warning(disable: 4311)

void StartTracking()
{
	bTrackMemory = true;
}

void StopTracking()
{
	bTrackMemory = false;
}

void AddTrack(void *pAddress, size_t uiSize, const char *lpFile, unsigned int uiLine)
{
	if(!bTrackMemory)
	{
		return;
	}

	SAllocInfo *pAllocInfo = (SAllocInfo *)malloc(sizeof(SAllocInfo));
	pAllocInfo->pAddress = pAddress;
	strncpy(pAllocInfo->lpFile, lpFile, sizeof(pAllocInfo->lpFile));
	pAllocInfo->lpFile[sizeof(pAllocInfo->lpFile) - 1] = '\0';
	pAllocInfo->uiLine = uiLine;
	pAllocInfo->uiSize = uiSize;

	SAllocList *pAllocList = &lpAllocList[((unsigned long)pAddress >> DEBUG_TRACK_MEMORY_ALIGNMENT) & ((unsigned long)~0 >> (32 - DEBUG_TRACK_MEMORY_HASH))];

	pAllocInfo->pNext = pAllocList->pFirst;
	pAllocList->pFirst = pAllocInfo;

	pAllocList->uiCount++;

	uiTotalTracks++;
	if(uiTotalTracks > uiPeakTracks)
	{
		uiPeakTracks = uiTotalTracks;
	}

	uiTotalBytes += uiSize;
	if(uiTotalBytes > uiPeakBytes)
	{
		uiPeakBytes = uiTotalBytes;
	}
}

void RemoveTrack(void *pAddress)
{
	SAllocList *pAllocList = &lpAllocList[((unsigned long)pAddress >> DEBUG_TRACK_MEMORY_ALIGNMENT) & ((unsigned long)~0 >> (32 - DEBUG_TRACK_MEMORY_HASH))];

	if(pAllocList->pFirst == 0)
	{
		return;
	}

	SAllocInfo *pLast, *pCurrent = pAllocList->pFirst;
	if(pCurrent->pAddress == pAddress)
	{
		pAllocList->uiCount--;

		uiTotalTracks--;
		uiTotalBytes -= pCurrent->uiSize;

		pAllocList->pFirst = pCurrent->pNext;
		free(pCurrent);

		return;
	}

	pLast = pCurrent;
	pCurrent = pCurrent->pNext;
	while(pCurrent)
	{
		if(pCurrent->pAddress == pAddress)
		{
			pAllocList->uiCount--;

			uiTotalTracks--;
			uiTotalBytes -= pCurrent->uiSize;

			pLast->pNext = pCurrent->pNext;
			free(pCurrent);

			return;
		}
		pLast = pCurrent;
		pCurrent = pCurrent->pNext;
	}
}

void DumpTracks()
{
	FILE *pFile = fopen(DEBUG_TRACK_DUMP_FILE, "wt");

	if(pFile == 0)
		return;

	for(unsigned int i = 0; i < (1 << DEBUG_TRACK_MEMORY_HASH); i++)
	{
		if(lpAllocList[i].uiCount > 0)
		{
			fprintf(pFile, "List: %u (%u entries)\n", i, lpAllocList[i].uiCount);

			SAllocInfo *pCurrent = lpAllocList[i].pFirst;
			while(pCurrent)
			{
				fprintf(pFile, "Address: 0x%.8x %-8.u bytes at %s (%u)\n", (unsigned long)pCurrent->pAddress, pCurrent->uiSize, pCurrent->lpFile, pCurrent->uiLine);

				pCurrent = pCurrent->pNext;
			}

			fprintf(pFile, "\n");
		}
	}
	fprintf(pFile, "Peak: %u B (%.2f KB) and %u allocations.\n", uiPeakBytes, (double)uiPeakBytes / 1024.0, uiPeakTracks);
	fprintf(pFile, "Total: %u B (%.2f KB) in %u allocations.\n", uiTotalBytes, (double)uiTotalBytes / 1024.0, uiTotalTracks);

	fclose(pFile);
}

#pragma warning(default: 4311)

#endif
