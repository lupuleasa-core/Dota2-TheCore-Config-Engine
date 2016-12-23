/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef MAPPING_H
#define MAPPING_H

#include "stdafx.h"

namespace HLLib
{
	namespace Mapping
	{
		class CMapping;

		class HLLIB_API CView
		{
			friend class CMapping;

		private:
			CMapping *pMapping;

			hlVoid *lpView;
			hlULongLong uiOffset;
			hlULongLong uiLength;
			hlULongLong uiAllocationOffset;
			hlULongLong uiAllocationLength;

		public:
			CView(CMapping *pMapping, hlVoid *lpView, hlULongLong uiAllocationOffset, hlULongLong uiAllocationLength, hlULongLong uiOffset = 0, hlULongLong uiLength = 0);
			~CView();

			CMapping *GetMapping() const;

			const hlVoid *GetView() const;
			hlULongLong GetOffset() const;
			hlULongLong GetLength() const;
			const hlVoid *GetAllocationView() const;
			hlULongLong GetAllocationOffset() const;
			hlULongLong GetAllocationLength() const;
		};

		typedef std::list<CView *> CViewList;

		class HLLIB_API CMapping
		{
		private:
			CViewList *pViews;

		public:
			CMapping();
			virtual ~CMapping();

			virtual HLMappingType GetType() const = 0;

			virtual const hlChar *GetFileName() const;

			virtual hlBool GetOpened() const = 0;
			virtual hlUInt GetMode() const = 0;

			hlUInt GetTotalAllocations() const;
			hlULongLong GetTotalMemoryAllocated() const;
			hlULongLong GetTotalMemoryUsed() const;

			hlBool Open(hlUInt uiMode);
			hlVoid Close();

			virtual hlULongLong GetMappingSize() const = 0;

			hlBool Map(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength);
			hlBool Unmap(CView *&pView);

			hlBool Commit(CView &View);
			hlBool Commit(CView &View, hlULongLong uiOffset, hlULongLong uiLength);

		private:
			virtual hlBool OpenInternal(hlUInt uiMode) = 0;
			virtual hlVoid CloseInternal() = 0;

			virtual hlBool MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength) = 0;
			virtual hlVoid UnmapInternal(CView &View);

			virtual hlBool CommitInternal(CView &View, hlULongLong uiOffset, hlULongLong uiLength);
		};
	}
}

#endif
