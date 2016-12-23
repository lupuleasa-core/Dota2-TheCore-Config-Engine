/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef STREAM_H
#define STREAM_H

#include "stdafx.h"

namespace HLLib
{
	namespace Streams
	{
		class HLLIB_API IStream
		{
		public:
			virtual ~IStream();

			virtual HLStreamType GetType() const = 0;

			virtual const hlChar *GetFileName() const = 0;

			virtual hlBool GetOpened() const = 0;
			virtual hlUInt GetMode() const = 0;

			virtual hlBool Open(hlUInt uiMode) = 0;
			virtual hlVoid Close() = 0;

			virtual hlULongLong GetStreamSize() const = 0;
			virtual hlULongLong GetStreamPointer() const = 0;

			virtual hlULongLong Seek(hlLongLong iOffset, HLSeekMode eSeekMode) = 0;

			virtual hlBool Read(hlChar &cChar) = 0;
			virtual hlUInt Read(hlVoid *lpData, hlUInt uiBytes) = 0;

			virtual hlBool Write(hlChar cChar) = 0;
			virtual hlUInt Write(const hlVoid *lpData, hlUInt uiBytes) = 0;
		};
	}
}

#endif
