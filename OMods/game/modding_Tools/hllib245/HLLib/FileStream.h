/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef FILESTREAM_H
#define FILESTREAM_H

#include "stdafx.h"
#include "Stream.h"

namespace HLLib
{
	namespace Streams
	{
		class HLLIB_API CFileStream : public IStream
		{
		private:
#ifdef _WIN32
			HANDLE hFile;
#else
			hlInt iFile;
#endif
			hlUInt uiMode;

			hlChar *lpFileName;

		public:
			CFileStream(const hlChar *lpFileName);
			~CFileStream();

			virtual HLStreamType GetType() const;

			virtual const hlChar *GetFileName() const;

			virtual hlBool GetOpened() const;
			virtual hlUInt GetMode() const;

			virtual hlBool Open(hlUInt uiMode);
			virtual hlVoid Close();

			virtual hlULongLong GetStreamSize() const;
			virtual hlULongLong GetStreamPointer() const;

			virtual hlULongLong Seek(hlLongLong iOffset, HLSeekMode eSeekMode);

			virtual hlBool Read(hlChar &cChar);
			virtual hlUInt Read(hlVoid *lpData, hlUInt uiBytes);

			virtual hlBool Write(hlChar cChar);
			virtual hlUInt Write(const hlVoid *lpData, hlUInt uiBytes);
		};
	}
}

#endif
