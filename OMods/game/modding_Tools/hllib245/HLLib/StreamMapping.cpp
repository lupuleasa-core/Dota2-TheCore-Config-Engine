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
#include "StreamMapping.h"

using namespace HLLib;
using namespace HLLib::Mapping;

CStreamMapping::CStreamMapping(Streams::IStream &Stream) : Stream(Stream)
{
	this->Stream.Close();
}

CStreamMapping::~CStreamMapping()
{
	this->Close();
}

HLMappingType CStreamMapping::GetType() const
{
	return HL_MAPPING_STREAM;
}


const Streams::IStream& CStreamMapping::GetStream() const
{
	return this->Stream;
}

const hlChar *CStreamMapping::GetFileName() const
{
	return this->Stream.GetFileName();
}

hlBool CStreamMapping::GetOpened() const
{
	return this->Stream.GetOpened();
}

hlUInt CStreamMapping::GetMode() const
{
	return this->Stream.GetMode();
}

hlBool CStreamMapping::OpenInternal(hlUInt uiMode)
{
	assert(!this->GetOpened());

	if((uiMode & HL_MODE_READ) == 0 && (uiMode & HL_MODE_WRITE) == 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);
		return hlFalse;
	}

	return this->Stream.Open(uiMode);
}

hlVoid CStreamMapping::CloseInternal()
{
	this->Stream.Close();
}

hlULongLong CStreamMapping::GetMappingSize() const
{
	return this->Stream.GetStreamSize();
}

hlBool CStreamMapping::MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength)
{
	assert(this->GetOpened());

	if(uiOffset + uiLength > this->Stream.GetStreamSize())
	{
#ifdef _WIN32
		LastError.SetErrorMessageFormated("Requested view (%I64u, %I64u) does not fit inside mapping, (%I64u, %I64u).", uiOffset, uiLength, 0, this->Stream.GetStreamSize());
#else
		LastError.SetErrorMessageFormated("Requested view (%llu, %llu) does not fit inside mapping, (%llu, %llu).", uiOffset, uiLength, 0, this->Stream.GetStreamSize());
#endif
		return hlFalse;
	}

	if(Stream.Seek(static_cast<hlLongLong>(uiOffset), HL_SEEK_BEGINNING) != uiOffset)
	{
		return hlFalse;
	}

	hlByte *lpData = new hlByte[static_cast<hlUInt>(uiLength)];

	if(Stream.Read(lpData, static_cast<hlUInt>(uiLength)) != uiLength)
	{
		delete []lpData;
		return hlFalse;
	}

	pView = new CView(this, lpData, uiOffset, uiLength);

	return hlTrue;
}

hlVoid CStreamMapping::UnmapInternal(CView &View)
{
	assert(this->GetOpened());
	assert(View.GetMapping() == this);

	delete [](hlByte *)View.GetAllocationView();
}

hlBool CStreamMapping::CommitInternal(CView &View, hlULongLong uiOffset, hlULongLong uiLength)
{
	assert(this->GetOpened());

	hlULongLong uiFileOffset = View.GetAllocationOffset() + View.GetOffset() + uiOffset;

	if(Stream.Seek(static_cast<hlLongLong>(uiFileOffset), HL_SEEK_BEGINNING) != uiFileOffset)
	{
		return hlFalse;
	}

	if(Stream.Write((const hlByte *)View.GetView() + uiOffset, static_cast<hlUInt>(uiLength)) != uiLength)
	{
		return hlFalse;
	}

	return hlTrue;
}
