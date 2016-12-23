/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "DirectoryFile.h"
#include "DirectoryFolder.h"
#include "HLLib.h"
#include "Streams.h"
#include "Package.h"
#include "Utility.h"

using namespace HLLib;

CDirectoryFile::CDirectoryFile(const hlChar *lpName, hlUInt uiID, hlVoid *pData, CPackage *pPackage, CDirectoryFolder *pParent) : CDirectoryItem(lpName, uiID, pData, pPackage, pParent)
{

}

CDirectoryFile::~CDirectoryFile()
{

}

HLDirectoryItemType CDirectoryFile::GetType() const
{
	return HL_ITEM_FILE;
}

hlBool CDirectoryFile::GetExtractable() const
{
	hlBool bExtractable = 0;
	this->GetPackage()->GetFileExtractable(this, bExtractable);
	return bExtractable;
}

hlBool CDirectoryFile::GetExtractable(hlBool &bExtractable) const
{
	return this->GetPackage()->GetFileExtractable(this, bExtractable);
}

HLValidation CDirectoryFile::GetValidation() const
{
	HLValidation eValidation = HL_VALIDATES_ASSUMED_OK;
	this->GetPackage()->GetFileValidation(this, eValidation);
	return eValidation;
}

hlBool CDirectoryFile::GetValidation(HLValidation &eValidation) const
{
	return this->GetPackage()->GetFileValidation(this, eValidation);
}

hlUInt CDirectoryFile::GetSize() const
{
	hlUInt uiSize = 0;
	this->GetPackage()->GetFileSize(this, uiSize);
	return uiSize;
}

hlBool CDirectoryFile::GetSize(hlUInt &uiSize) const
{
	return this->GetPackage()->GetFileSize(this, uiSize);
}

hlUInt CDirectoryFile::GetSizeOnDisk() const
{
	hlUInt uiSize = 0;
	this->GetPackage()->GetFileSizeOnDisk(this, uiSize);
	return uiSize;
}

hlBool CDirectoryFile::GetSizeOnDisk(hlUInt &uiSize) const
{
	return this->GetPackage()->GetFileSizeOnDisk(this, uiSize);
}

hlBool CDirectoryFile::CreateStream(Streams::IStream *&pStream) const
{
	return this->GetPackage()->CreateStream(this, pStream);
}

hlVoid CDirectoryFile::ReleaseStream(Streams::IStream *pStream) const
{
	return this->GetPackage()->ReleaseStream(pStream);
}

hlBool CDirectoryFile::Extract(const hlChar *lpPath) const
{
	hlExtractItemStart(this);

	hlChar *lpName = new hlChar[strlen(this->GetName()) + 1];
	strcpy(lpName, this->GetName());
	RemoveIllegalCharacters(lpName);

	hlChar *lpFileName;
	if(lpPath == 0 || *lpPath == '\0')
	{
		lpFileName = new hlChar[strlen(lpName) + 1];
		strcpy(lpFileName, lpName);
	}
	else
	{
		lpFileName = new hlChar[strlen(lpPath) + 1 + strlen(lpName) + 1];
		strcpy(lpFileName, lpPath);
		strcat(lpFileName, PATH_SEPARATOR_STRING);
		strcat(lpFileName, lpName);
	}

	FixupIllegalCharacters(lpFileName);

	hlBool bResult;
	if(!bOverwriteFiles && GetFileExists(lpFileName))
	{
		bResult = hlTrue;
	}
	else
	{
		bResult = hlFalse;

		Streams::IStream *pInput = 0;

		if(this->GetPackage()->CreateStream(this, pInput))
		{
			if(pInput->Open(HL_MODE_READ))
			{
				Streams::CFileStream Output = Streams::CFileStream(lpFileName);

				if(Output.Open(HL_MODE_WRITE | HL_MODE_CREATE))
				{
					hlUInt uiTotalBytes = 0, uiFileBytes = this->GetSize();
					hlByte lpBuffer[HL_DEFAULT_COPY_BUFFER_SIZE];

					hlBool bCancel = hlFalse;
					hlExtractFileProgress(this, uiTotalBytes, uiFileBytes, &bCancel);

					while(hlTrue)
					{
						if(bCancel)
						{
							LastError.SetErrorMessage("Canceled by user.");
						}

						hlUInt uiBytes = pInput->Read(lpBuffer, sizeof(lpBuffer));

						if(uiBytes == 0)
						{
							bResult = uiTotalBytes == pInput->GetStreamSize();
							break;
						}

						if(Output.Write(lpBuffer, uiBytes) != uiBytes)
						{
							break;
						}

						uiTotalBytes += uiBytes;

						hlExtractFileProgress(this, uiTotalBytes, uiFileBytes, &bCancel);
					}

					Output.Close();
				}

				pInput->Close();
			}

			this->GetPackage()->ReleaseStream(pInput);
		}
	}

	delete []lpFileName;
	delete []lpName;

	hlExtractItemEnd(this, bResult);

	return bResult;
}
