/*
 * HLLib
 * Copyright (C) 2006-2012 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your Option) any later
 * version.
 */

#ifndef UTILITY_H
#define UTILITY_H

#include "stdafx.h"
#include "Error.h"

namespace HLLib
{
	extern hlBool GetFileExists(const hlChar *lpPath);
	extern hlBool GetFolderExists(const hlChar *lpPath);

	extern hlBool GetFileSize(const hlChar *lpPath, hlUInt &uiFileSize);

	extern hlBool CreateFolder(const hlChar *lpPath);

	extern hlVoid FixupIllegalCharacters(hlChar *lpName);
	extern hlVoid RemoveIllegalCharacters(hlChar *lpName);

	extern hlChar NibbleToChar(hlByte uiNibble);
	extern hlUInt BufferToHexString(const hlByte *lpBuffer, hlUInt uiBufferSize, hlChar* lpString, hlUInt uiStringSize);
	extern hlUInt WStringToString(const hlWChar *lpSource, hlChar* lpDest, hlUInt uiDestSize);
}

#endif
