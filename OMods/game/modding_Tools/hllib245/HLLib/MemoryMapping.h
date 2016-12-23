/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef MEMORYMAPPING_H
#define MEMORYMAPPING_H

#include "stdafx.h"
#include "Mapping.h"

namespace HLLib
{
	namespace Mapping
	{
		class HLLIB_API CMemoryMapping : public CMapping
		{
		private:
			hlBool bOpened;
			hlUInt uiMode;

			hlVoid *lpData;
			hlULongLong uiBufferSize;

		public:
			CMemoryMapping(hlVoid *lpData, hlULongLong uiBufferSize);
			virtual ~CMemoryMapping();

			virtual HLMappingType GetType() const;

			const hlVoid *GetBuffer() const;
			hlULongLong GetBufferSize() const;

			virtual hlBool GetOpened() const;
			virtual hlUInt GetMode() const;

			virtual hlULongLong GetMappingSize() const;

		private:
			virtual hlBool OpenInternal(hlUInt uiMode);
			virtual hlVoid CloseInternal();

			virtual hlBool MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength);
		};
	}
}

#endif
