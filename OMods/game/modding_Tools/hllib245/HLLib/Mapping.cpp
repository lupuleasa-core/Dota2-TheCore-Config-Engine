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
#include "Mapping.h"

using namespace HLLib;
using namespace HLLib::Mapping;

CView::CView(CMapping *pMapping, hlVoid *lpView, hlULongLong uiAllocationOffset, hlULongLong uiAllocationLength, hlULongLong uiOffset, hlULongLong uiLength) : pMapping(pMapping), lpView(lpView), uiAllocationOffset(uiAllocationOffset), uiAllocationLength(uiAllocationLength), uiOffset(uiOffset), uiLength(uiLength == 0 ? uiAllocationLength - uiOffset : uiLength)
{
	assert(this->uiOffset + this->uiLength <= this->uiAllocationLength);
}

CView::~CView()
{

}

CMapping *CView::GetMapping() const
{
	return this->pMapping;
}

const hlVoid *CView::GetView() const
{
	return (hlByte *)this->lpView + this->uiOffset;
}

hlULongLong CView::GetOffset() const
{
	return this->uiOffset;
}

hlULongLong CView::GetLength() const
{
	return this->uiLength;
}

hlULongLong CView::GetAllocationOffset() const
{
	return this->uiAllocationOffset;
}

hlULongLong CView::GetAllocationLength() const
{
	return this->uiAllocationLength;
}

const hlVoid *CView::GetAllocationView() const
{
	return this->lpView;
}

CMapping::CMapping() : pViews(0)
{

}

CMapping::~CMapping()
{
	assert(this->pViews == 0);
}

const hlChar *CMapping::GetFileName() const
{
	return "";
}

hlUInt CMapping::GetTotalAllocations() const
{
	if(this->pViews == 0)
	{
		return 0;
	}

	return (hlUInt)this->pViews->size();
}

hlULongLong CMapping::GetTotalMemoryAllocated() const
{
	if(this->pViews == 0)
	{
		return 0;
	}

	hlULongLong uiTotal = 0;
	for(CViewList::iterator i = this->pViews->begin(); i != this->pViews->end(); ++i)
	{
		uiTotal += (*i)->GetAllocationLength();
	}
	return uiTotal;
}

hlULongLong CMapping::GetTotalMemoryUsed() const
{
	if(this->pViews == 0)
	{
		return 0;
	}

	hlULongLong uiTotal = 0;
	for(CViewList::iterator i = this->pViews->begin(); i != this->pViews->end(); ++i)
	{
		uiTotal += (*i)->GetLength();
	}
	return uiTotal;
}

hlBool CMapping::Open(hlUInt uiMode)
{
	this->Close();

	if(this->OpenInternal(uiMode))
	{
		this->pViews = new CViewList();
		return hlTrue;
	}
	else
	{
		this->CloseInternal();
		return hlFalse;
	}
}

hlVoid CMapping::Close()
{
	if(this->pViews != 0)
	{
		for(CViewList::iterator i = this->pViews->begin(); i != this->pViews->end(); ++i)
		{
			this->UnmapInternal(**i);
		}
		delete this->pViews;
		this->pViews = 0;
	}

	this->CloseInternal();
}

hlBool CMapping::Map(CView *&pView, hlULongLong uiOffset, hlULongLong uiLength)
{
	if(!this->GetOpened())
	{
		LastError.SetErrorMessage("Mapping not open.");
		return hlFalse;
	}

	if(pView != 0)
	{
		if(pView->GetMapping() != this)
		{
			LastError.SetErrorMessage("View does not belong to mapping.");
			return hlFalse;
		}

		if(uiOffset >= pView->GetAllocationOffset() && uiOffset + uiLength <= pView->GetAllocationOffset() + pView->GetAllocationLength())
		{
			pView->uiOffset = uiOffset - pView->GetAllocationOffset();
			pView->uiLength = uiLength;
			return hlTrue;
		}
	}

	if(this->Unmap(pView) && this->MapInternal(pView, uiOffset, uiLength))
	{
		this->pViews->push_back(pView);
		return hlTrue;
	}

	return hlFalse;
}

hlBool CMapping::Unmap(CView *&pView)
{
	if(pView == 0)
	{
		return hlTrue;
	}

	if(this->GetOpened() && pView->GetMapping() == this)
	{
		for(CViewList::iterator i = this->pViews->begin(); i != this->pViews->end(); ++i)
		{
			if(*i == pView)
			{
				this->UnmapInternal(*pView);
				delete pView;
				pView = 0;

				this->pViews->erase(i);
				return hlTrue;
				break;
			}
		}
	}

	LastError.SetErrorMessage("View does not belong to mapping.");
	return hlFalse;
}

hlVoid CMapping::UnmapInternal(CView &View)
{

}

hlBool CMapping::Commit(CView &View)
{
	return this->Commit(View, 0, View.GetLength());
}

hlBool CMapping::Commit(CView &View, hlULongLong uiOffset, hlULongLong uiLength)
{
	if(!this->GetOpened() || View.GetMapping() != this)
	{
		LastError.SetErrorMessage("View does not belong to mapping.");
		return hlFalse;
	}

	if(uiOffset + uiLength > View.GetLength())
	{
#ifdef _WIN32
		LastError.SetErrorMessageFormated("Requested range (%I64u, %I64u) does not fit inside view, (%I64u, %I64u).", uiOffset, uiLength, 0ULL, View.GetLength());
#else
		LastError.SetErrorMessageFormated("Requested range (%llu, %llu) does not fit inside view, (%llu, %llu).", uiOffset, uiLength, 0ULL, View.GetLength());
#endif
		return hlFalse;
	}

	if((this->GetMode() & HL_MODE_WRITE) == 0)
	{
		// Not in write mode so nothing to do.
		return hlTrue;
	}

	return this->CommitInternal(View, uiOffset, uiLength);
}

hlBool CMapping::CommitInternal(CView &View, hlULongLong uiOffset, hlULongLong uiLength)
{
	return hlTrue;
}
