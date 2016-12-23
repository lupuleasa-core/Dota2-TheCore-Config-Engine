/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef PACKAGE_H
#define PACKAGE_H

#include "stdafx.h"
#include "DirectoryItems.h"
#include "Mapping.h"
#include "Stream.h"

namespace HLLib
{
	typedef std::list<Streams::IStream *> CStreamList;

	class HLLIB_API CPackage
	{
	private:
		hlBool bDeleteStream;
		hlBool bDeleteMapping;
		Streams::IStream *pStream;

	protected:
		Mapping::CMapping *pMapping;

		CDirectoryFolder *pRoot;

	private:
		mutable CStreamList *pStreams;

	public:
		CPackage();
		virtual ~CPackage();

		virtual HLPackageType GetType() const = 0;
		virtual const hlChar *GetExtension() const = 0;
		virtual const hlChar *GetDescription() const = 0;

		hlBool GetOpened() const;

		hlBool Open(Streams::IStream &Stream, hlUInt uiMode);
		hlBool Open(Mapping::CMapping &Mapping, hlUInt uiMode);
		hlBool Open(const hlChar *lpFileName, hlUInt uiMode);
		hlBool Open(hlVoid *lpData, hlUInt uiBufferSize, hlUInt uiMode);
		hlBool Open(hlVoid *pUserData, hlUInt uiMode);
		hlVoid Close();

		hlBool Defragment();

		const Mapping::CMapping* GetMapping() const;
		CDirectoryFolder *GetRoot();
		const CDirectoryFolder *GetRoot() const;

		hlUInt GetAttributeCount() const;
		const hlChar *GetAttributeName(HLPackageAttribute eAttribute) const;
		hlBool GetAttribute(HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

		hlUInt GetItemAttributeCount() const;
		const hlChar *GetItemAttributeName(HLPackageAttribute eAttribute) const;
		hlBool GetItemAttribute(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

		hlBool GetFileExtractable(const CDirectoryFile *pFile, hlBool &bExtractable) const;
		hlBool GetFileValidation(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		hlBool GetFileSize(const CDirectoryFile *pFile, hlUInt &uiSize) const;
		hlBool GetFileSizeOnDisk(const CDirectoryFile *pFile, hlUInt &uiSize) const;

		hlBool CreateStream(const CDirectoryFile *pFile, Streams::IStream *&pStream) const;
		hlVoid ReleaseStream(Streams::IStream *pStream) const;

	protected:
		virtual hlBool MapDataStructures() = 0;
		virtual hlVoid UnmapDataStructures() = 0;

		virtual hlBool DefragmentInternal();

		virtual CDirectoryFolder *CreateRoot() = 0;
		virtual hlVoid ReleaseRoot();

		virtual hlUInt GetAttributeCountInternal() const;
		virtual const hlChar *GetAttributeNameInternal(HLPackageAttribute eAttribute) const;
		virtual hlBool GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

		virtual hlUInt GetItemAttributeCountInternal() const;
		virtual const hlChar *GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const;
		virtual hlBool GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const;

		virtual hlBool GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const;
		virtual hlBool GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const;
		virtual hlBool GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const = 0;
		virtual hlBool GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const = 0;

		virtual hlBool CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const = 0;
		virtual hlVoid ReleaseStreamInternal(Streams::IStream &Stream) const;

	private:
		hlBool Open(Streams::IStream *pStream, hlUInt uiMode, hlBool bDeleteStream);
		hlBool Open(Mapping::CMapping *pMapping, hlUInt uiMode, hlBool bDeleteMapping);
	};
}

#endif
