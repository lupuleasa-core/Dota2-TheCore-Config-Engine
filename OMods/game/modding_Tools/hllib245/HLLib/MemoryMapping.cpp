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
#include "MemoryMapping.h"

using namespace HLLib;
using namespace HLLib::Mapping;

CMemoryMapping::CMemoryMapping(hlVoid *lpData, hlULongLong uiBufferSize) : bOpened(hlFalse), uiMode(HL_MODE_INVALID), lpData(lpData), uiBufferSize(uiBufferSize)
{

}

CMemoryMapping::~CMemoryMapping()
{
	this->Close();
}

HLMappingType CMemoryMapping::GetType() const
{
	return HL_MAPPING_MEMORY;
}


const hlVoid *CMemoryMapping::GetBuffer() const
{
	return this->lpData;
}

hlULongLong CMemoryMapping::GetBufferSize() const
{
	return this->uiBufferSize;
}

hlBool CMemoryMapping::GetOpened() const
{
	return this->bOpened;
}

hlUInt CMemoryMapping::GetMode() const
{
	return this->uiMode;
}

hlBool CMemoryMapping::OpenInternal(hlUInt uiMode)
{
	assert(!this->GetOpened());

	if(this->uiBufferSize != 0 && this->lpData == 0)
	{
		LastError.SetErrorMessage("Memory stream is null.");
		return hlFalse;
	}

	if((uiMode & HL_MODE_READ) == 0 || (uiMode & HL_MODE_WRITE) != 0)
	{
		LastError.SetErrorMessageFormated("Invalid open mode (%#.8x).", uiMode);
		return hlFalse;
	}

	this->bOpened = hlTrue;
	this->uiMode = uiMode;

	return hlTrue;
}

hlVoid CMemoryMapping::CloseInternal()
{
	this->bOpened = hlFalse;
	this->uiMode = HL_MODE_INVALID;
}

hlULongLong CMemoryMapping::GetMappingSize() const
{
	return this->bOpened ? this->uiBufferSize : 0;
}

hlBool CMemoryMapping::MapInternal(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength)
{
	assert(this->GetOpened());

	if(uiOffset + uiLength > this->uiBufferSize)
	{
#ifdef _WIN32
		LastError.SetErrorMessageFormated("Requested view (%I64u, %I64u) does not fit inside mapping, (%I64u, %I64u).", uiOffset, uiLength, 0, this->uiBufferSize);
#else
		LastError.SetErrorMessageFormated("Requested view (%llu, %llu) does not fit inside mapping, (%llu, %llu).", uiOffset, uiLength, 0, this->uiBufferSize);
#endif
		return hlFalse;
	}

	pView = new CView(this, this->lpData, 0, this->uiBufferSize, uiOffset, uiLength);

	return hlTrue;
}
