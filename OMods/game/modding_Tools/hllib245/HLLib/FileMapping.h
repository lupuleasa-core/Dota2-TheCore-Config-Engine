/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef FILEMAPPING_H
#define FILEMAPPING_H

#include "stdafx.h"
#include "Mapping.h"

namespace HLLib
{
	namespace Mapping
	{
		class HLLIB_API CFileMapping : public CMapping
		{
		private:
#ifdef _WIN32
			HANDLE hFile;
			HANDLE hFileMapping;
#else
			hlInt iFile;
#endif
			hlUInt uiAllocationGranularity;
			hlUInt uiMode;

			hlVoid *lpView;
			hlULongLong uiViewSize;

			hlChar *lpFileName;

		public:
			CFileMapping(const hlChar *lpFileName);
			virtual ~CFileMapping();

			virtual HLMappingType GetType() const;

			virtual const hlChar *GetFileName() const;

			virtual hlBool GetOpened() const;
			virtual hlUInt GetMode() const;

			virtual hlULongLong GetMappingSize() const;

		private:
			virtual hlBool OpenInternal(hlUInt uiMode);
			virtual hlVoid CloseInternal();

			virtual hlBool MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength);
			virtual hlVoid UnmapInternal(CView &View);
		};
	}
}

#endif
