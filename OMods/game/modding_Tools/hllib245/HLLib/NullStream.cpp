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
#include "NullStream.h"

using namespace HLLib;
using namespace HLLib::Streams;

CNullStream::CNullStream() : bOpened(hlFalse), uiMode(HL_MODE_INVALID)
{

}

CNullStream::~CNullStream()
{

}

HLStreamType CNullStream::GetType() const
{
	return HL_STREAM_NULL;
}

const hlChar *CNullStream::GetFileName() const
{
	return "";
}

hlBool CNullStream::GetOpened() const
{
	return this->bOpened;
}

hlUInt CNullStream::GetMode() const
{
	return this->uiMode;
}

hlBool CNullStream::Open(hlUInt uiMode)
{
	this->bOpened = hlTrue;
	this->uiMode = uiMode;

	return hlTrue;
}

hlVoid CNullStream::Close()
{
	this->bOpened = hlFalse;
	this->uiMode = HL_MODE_INVALID;
}

hlULongLong CNullStream::GetStreamSize() const
{
	return 0;
}

hlULongLong CNullStream::GetStreamPointer() const
{
	return 0;
}

hlULongLong CNullStream::Seek(hlLongLong iOffset, HLSeekMode eSeekMode)
{
	return 0;
}

hlBool CNullStream::Read(hlChar &cChar)
{
	return hlFalse;
}

hlUInt CNullStream::Read(hlVoid *lpData, hlUInt uiBytes)
{
	return 0;
}

hlBool CNullStream::Write(hlChar cChar)
{
	return hlFalse;
}

hlUInt CNullStream::Write(const hlVoid *lpData, hlUInt uiBytes)
{
	return 0;
}
