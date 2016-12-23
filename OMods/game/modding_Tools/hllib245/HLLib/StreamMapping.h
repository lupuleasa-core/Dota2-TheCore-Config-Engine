/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef STREAMMAPPING_H
#define STREAMMAPPING_H

#include "stdafx.h"
#include "Stream.h"
#include "Mapping.h"

namespace HLLib
{
	namespace Mapping
	{
		class HLLIB_API CStreamMapping : public CMapping
		{
		private:
			Streams::IStream &Stream;

		public:
			CStreamMapping(Streams::IStream &Stream);
			virtual ~CStreamMapping();

			virtual HLMappingType GetType() const;

			const Streams::IStream& GetStream() const;
			virtual const hlChar *GetFileName() const;

			virtual hlBool GetOpened() const;
			virtual hlUInt GetMode() const;

			virtual hlULongLong GetMappingSize() const;

		private:
			virtual hlBool OpenInternal(hlUInt uiMode);
			virtual hlVoid CloseInternal();

			virtual hlBool MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength);
			virtual hlVoid UnmapInternal(CView &View);

			virtual hlBool CommitInternal(CView &View, hlULongLong uiOffset, hlULongLong uiLength);
		};
	}
}

#endif
