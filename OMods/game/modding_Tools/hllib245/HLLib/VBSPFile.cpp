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
#include "VBSPFile.h"
#include "Streams.h"
#include "Checksum.h"

using namespace HLLib;

#define HL_VBSP_LUMP_ENTITIES		0
#define HL_VBSP_LUMP_PAKFILE		40

#define HL_VBSP_ZIP_LOCAL_FILE_HEADER_SIGNATURE					0x04034b50
#define HL_VBSP_ZIP_FILE_HEADER_SIGNATURE						0x02014b50
#define HL_VBSP_ZIP_END_OF_CENTRAL_DIRECTORY_RECORD_SIGNATURE	0x06054b50

#define HL_VBSP_ZIP_CHECKSUM_LENGTH								0x00008000

const char *CVBSPFile::lpAttributeNames[] = { "Version", "Map Revision" };
const char *CVBSPFile::lpItemAttributeNames[] = { "Version", "Four CC", "Zip Disk", "Zip Comment", "Create Version", "Extract Version", "Flags", "Compression Method", "CRC", "Disk", "Comment" };

CVBSPFile::CVBSPFile() : CPackage(), pHeaderView(0), pFileHeaderView(0), pEndOfCentralDirectoryRecordView(0), pHeader(0), pEndOfCentralDirectoryRecord(0)
{

}

CVBSPFile::~CVBSPFile()
{
	this->Close();
}

HLPackageType CVBSPFile::GetType() const
{
	return HL_PACKAGE_VBSP;
}

const hlChar *CVBSPFile::GetExtension() const
{
	return "bsp";
}

const hlChar *CVBSPFile::GetDescription() const
{
	return "Half-Life 2 Level";
}

hlBool CVBSPFile::MapDataStructures()
{
	if(sizeof(VBSPHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(VBSPHeader)))
	{
		return hlFalse;
	}
	this->pHeader = static_cast<const VBSPHeader *>(this->pHeaderView->GetView());

	if(memcmp(this->pHeader->lpSignature, "VBSP", 4) != 0)
	{
		LastError.SetErrorMessage("Invalid file: the file's signature does not match.");
		return hlFalse;
	}

	// Versions:
	//  19-20:			Source
	//  21:				Source - The lump version property was moved to the start of the struct.
	//  0x00040014:		Dark Messiah - Looks like the 32 bit version has been split into two 16 bit fields.
	if((this->pHeader->iVersion < 19 || this->pHeader->iVersion > 21) && this->pHeader->iVersion != 0x00040014)
	{
		LastError.SetErrorMessageFormated("Invalid VBSP version (v%i): you have a version of a VBSP file that HLLib does not know how to read. Check for product updates.", this->pHeader->iVersion);
		return hlFalse;
	}

	if(this->pHeader->iVersion >= 21 && this->pHeader->iVersion != 0x00040014)
	{
		VBSPHeader *pHeader = new VBSPHeader;
		memcpy(pHeader, this->pHeader, sizeof(VBSPHeader));
		for(hlUInt i = 0; i < HL_VBSP_LUMP_COUNT; i++)
		{
			pHeader->lpLumps[i].uiVersion = this->pHeader->lpLumps[i].uiOffset;
			pHeader->lpLumps[i].uiOffset = this->pHeader->lpLumps[i].uiLength;
			pHeader->lpLumps[i].uiLength = this->pHeader->lpLumps[i].uiVersion;
		}
		this->pHeader = pHeader;
	}

	if(sizeof(ZIPEndOfCentralDirectoryRecord) <= this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiLength)
	{
		hlUInt uiTest, uiOffset = this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiOffset;
		while(uiOffset < this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiOffset + this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiLength - sizeof(uiTest))
		{
			Mapping::CView *pTestView = 0;

			if(!this->pMapping->Map(pTestView, uiOffset, sizeof(hlUInt)))
			{
				return hlFalse;
			}

			uiTest = *(hlUInt *)pTestView->GetView();

			this->pMapping->Unmap(pTestView);

			switch(uiTest)
			{
				case HL_VBSP_ZIP_END_OF_CENTRAL_DIRECTORY_RECORD_SIGNATURE:
				{
					if(!this->pMapping->Map(pTestView, uiOffset, sizeof(ZIPEndOfCentralDirectoryRecord)))
					{
						return hlFalse;
					}

					const ZIPEndOfCentralDirectoryRecord EndOfCentralDirRecord = *static_cast<const ZIPEndOfCentralDirectoryRecord *>(pTestView->GetView());

					this->pMapping->Unmap(pTestView);

					if(!this->pMapping->Map(this->pEndOfCentralDirectoryRecordView, uiOffset, sizeof(ZIPEndOfCentralDirectoryRecord) + EndOfCentralDirRecord.uiCommentLength))
					{
						return hlFalse;
					}

					this->pEndOfCentralDirectoryRecord = static_cast<const ZIPEndOfCentralDirectoryRecord *>(this->pEndOfCentralDirectoryRecordView->GetView());

					if(!this->pMapping->Map(this->pFileHeaderView, this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiOffset + this->pEndOfCentralDirectoryRecord->uiStartOfCentralDirOffset, this->pEndOfCentralDirectoryRecord->uiCentralDirectorySize))
					{
						return hlFalse;
					}

					return hlTrue;
				}
				case HL_VBSP_ZIP_FILE_HEADER_SIGNATURE:
				{
					if(!this->pMapping->Map(pTestView, uiOffset, sizeof(ZIPFileHeader)))
					{
						return hlFalse;
					}

					const ZIPFileHeader FileHeader = *static_cast<const ZIPFileHeader *>(pTestView->GetView());

					this->pMapping->Unmap(pTestView);

					uiOffset += sizeof(ZIPFileHeader) + FileHeader.uiFileNameLength + FileHeader.uiExtraFieldLength + FileHeader.uiFileCommentLength;
					break;
				}
				case HL_VBSP_ZIP_LOCAL_FILE_HEADER_SIGNATURE:
				{
					if(!this->pMapping->Map(pTestView, uiOffset, sizeof(ZIPLocalFileHeader)))
					{
						return hlFalse;
					}

					const ZIPLocalFileHeader LocalFileHeader = *static_cast<const ZIPLocalFileHeader *>(pTestView->GetView());

					this->pMapping->Unmap(pTestView);

					uiOffset += sizeof(ZIPLocalFileHeader) + LocalFileHeader.uiFileNameLength + LocalFileHeader.uiExtraFieldLength + LocalFileHeader.uiCompressedSize;
					break;
				}
				default:
				{
					LastError.SetErrorMessageFormated("Invalid file: unknown ZIP section signature %#.8x.", uiTest);
					return hlFalse;
				}
			}
		}

		LastError.SetErrorMessage("Invalid file: unexpected end of file while scanning for end of ZIP central directory record.");
		return hlFalse;
	}

	return hlTrue;
}

hlVoid CVBSPFile::UnmapDataStructures()
{
	this->pMapping->Unmap(this->pFileHeaderView);

	this->pEndOfCentralDirectoryRecord = 0;
	this->pMapping->Unmap(this->pEndOfCentralDirectoryRecordView);

	if(this->pHeader != this->pHeaderView->GetView())
	{
		delete this->pHeader;
	}
	this->pHeader = 0;
	this->pMapping->Unmap(this->pHeaderView);
}

CDirectoryFolder *CVBSPFile::CreateRoot()
{
	CDirectoryFolder *pRoot = new CDirectoryFolder(this);

	hlChar lpFileName[256];

	if(this->pHeader->lpLumps[HL_VBSP_LUMP_ENTITIES].uiLength != 0)
	{
		this->GetFileName(lpFileName, sizeof(lpFileName) - 4);
		if(*lpFileName == '\0')
		{
			pRoot->AddFile("entities.ent", HL_VBSP_LUMP_ENTITIES);
		}
		else
		{
			strcat(lpFileName, ".ent");
			pRoot->AddFile(lpFileName, HL_VBSP_LUMP_ENTITIES);
		}
	}

	if(this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiLength != 0)
	{
		this->GetFileName(lpFileName, sizeof(lpFileName) - 4);
		if(*lpFileName == '\0')
		{
			pRoot->AddFile("pakfile.zip", HL_VBSP_LUMP_PAKFILE);
		}
		else
		{
			strcat(lpFileName, ".zip");
			pRoot->AddFile(lpFileName, HL_VBSP_LUMP_PAKFILE);
		}
	}

	CDirectoryFolder *pLumpFolder = pRoot->AddFolder("lumps");
	for(hlUInt i = 0; i < HL_VBSP_LUMP_COUNT; i++)
	{
		if(this->pHeader->lpLumps[i].uiLength > 0)
		{
			hlChar lpTemp[256];
			this->GetFileName(lpTemp, sizeof(lpTemp) - 10);
			if(*lpTemp == '\0')
			{
				sprintf(lpFileName, "lump_l_%d.lmp", i);
			}
			else
			{
				sprintf(lpFileName, "%s_l_%d.lmp", lpTemp, i);
			}
			pLumpFolder->AddFile(lpFileName, HL_VBSP_LUMP_COUNT + i);
		}
	}

	if(this->pEndOfCentralDirectoryRecord != 0)
	{
		hlUInt uiTest, uiOffset = 0;
		while(uiOffset < this->pEndOfCentralDirectoryRecord->uiCentralDirectorySize - sizeof(uiTest))
		{
			uiTest = *(hlUInt *)((hlByte *)this->pFileHeaderView->GetView() + uiOffset);

			switch(uiTest)
			{
				case HL_VBSP_ZIP_FILE_HEADER_SIGNATURE:
				{
					ZIPFileHeader *pFileHeader = static_cast<ZIPFileHeader *>((hlVoid *)((hlByte *)this->pFileHeaderView->GetView() + uiOffset));

					hlChar *lpFileName = new hlChar[pFileHeader->uiFileNameLength + 1];
					memcpy(lpFileName, (hlByte *)pFileHeader + sizeof(ZIPFileHeader), pFileHeader->uiFileNameLength);
					lpFileName[pFileHeader->uiFileNameLength] = '\0';

					// Check if we have just a file, or if the file has directories we need to create.
					if(strchr(lpFileName, '/') == 0 && strchr(lpFileName, '\\') == 0)
					{
						pRoot->AddFile(lpFileName, HL_ID_INVALID, pFileHeader);
					}
					else
					{
						// Tokenize the file path and create the directories.
						CDirectoryFolder *pInsertFolder = pRoot;

						hlChar lpTemp[256] = "";
						hlChar *lpToken = strtok(lpFileName, "/\\");
						while(lpToken != 0)
						{
							strcpy(lpTemp, lpToken);

							lpToken = strtok(0, "/\\");

							if(lpToken != 0)
							{
								// Check if the directory exists.
								CDirectoryItem *pItem = pInsertFolder->GetItem(lpTemp);
								if(pItem == 0 || pItem->GetType() == HL_ITEM_FILE)
								{
									// It doesn't, create it.
									pInsertFolder = pInsertFolder->AddFolder(lpTemp);
								}
								else
								{
									// It does, use it.
									pInsertFolder = static_cast<CDirectoryFolder *>(pItem);
								}
							}
						}

						// The file name is the last token, add it.
						pInsertFolder->AddFile(lpTemp, HL_ID_INVALID, pFileHeader);
					}

					delete []lpFileName;

					uiOffset += sizeof(ZIPFileHeader) + pFileHeader->uiFileNameLength + pFileHeader->uiExtraFieldLength + pFileHeader->uiFileCommentLength;
					break;
				}
				default:
				{
					uiOffset = this->pEndOfCentralDirectoryRecord->uiCentralDirectorySize;
					break;
				}
			}
		}
	}

	return pRoot;
}

hlUInt CVBSPFile::GetAttributeCountInternal() const
{
	return HL_VBSP_PACKAGE_COUNT;
}

const hlChar *CVBSPFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_VBSP_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CVBSPFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_VBSP_PACKAGE_VERSION:
		hlAttributeSetInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->iVersion);
		return hlTrue;
	case HL_VBSP_PACKAGE_MAP_REVISION:
		hlAttributeSetInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->iMapRevision);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CVBSPFile::GetItemAttributeCountInternal() const
{
	return HL_VBSP_ITEM_COUNT;
}

const hlChar *CVBSPFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_VBSP_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}


hlBool CVBSPFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			if(pFile->GetData())
			{
				const ZIPFileHeader *pDirectoryItem = static_cast<const ZIPFileHeader *>(pFile->GetData());
				switch(eAttribute)
				{
					case HL_VBSP_ZIP_ITEM_CREATE_VERSION:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->uiVersionMadeBy, hlFalse);
						return hlTrue;
					}
					case HL_VBSP_ZIP_ITEM_EXTRACT_VERSION:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->uiVersionNeededToExtract, hlFalse);
						return hlTrue;
					}
					case HL_VBSP_ZIP_ITEM_FLAGS:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->uiFlags, hlTrue);
						return hlTrue;
					}
					case HL_VBSP_ZIP_ITEM_COMPRESSION_METHOD:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->uiCompressionMethod, hlTrue);
						return hlTrue;
					}
					case HL_VBSP_ZIP_ITEM_CRC:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->uiCRC32, hlTrue);
						return hlTrue;
					}
					case HL_VBSP_ZIP_ITEM_DISK:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], pDirectoryItem->uiDiskNumberStart, hlFalse);
						return hlTrue;
					}
					case HL_VBSP_ZIP_ITEM_COMMENT:
					{
						hlChar *lpComment = new hlChar[pDirectoryItem->uiFileCommentLength + 1];
						memcpy(lpComment, (hlByte *)pDirectoryItem + sizeof(ZIPFileHeader) + pDirectoryItem->uiFileNameLength + pDirectoryItem->uiExtraFieldLength, pDirectoryItem->uiFileCommentLength);
						lpComment[pDirectoryItem->uiFileCommentLength] = '\0';

						hlAttributeSetString(&Attribute, this->lpItemAttributeNames[eAttribute], lpComment);

						delete []lpComment;
						return hlTrue;
					}
				}
			}
			else
			{
				hlUInt uiID = pFile->GetID();
				if(uiID >= HL_VBSP_LUMP_COUNT)
				{
					uiID -= HL_VBSP_LUMP_COUNT;
				}

				switch(eAttribute)
				{
					case HL_VBSP_ITEM_VERSION:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], this->pHeader->lpLumps[uiID].uiVersion, hlFalse);
						return hlTrue;
					}
					case HL_VBSP_ITEM_FOUR_CC:
					{
						hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], *(hlUInt *)this->pHeader->lpLumps[uiID].lpFourCC, hlTrue);
						return hlTrue;
					}
				}

				if(this->pEndOfCentralDirectoryRecord != 0 && pItem->GetID() == HL_VBSP_LUMP_PAKFILE)
				{
					switch(eAttribute)
					{
						case HL_VBSP_ZIP_PACKAGE_DISK:
						{
							hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], this->pEndOfCentralDirectoryRecord->uiNumberOfThisDisk, hlFalse);
							return hlTrue;
						}
						case HL_VBSP_ZIP_PACKAGE_COMMENT:
						{
							hlChar *lpComment = new hlChar[this->pEndOfCentralDirectoryRecord->uiCommentLength + 1];
							memcpy(lpComment, (hlByte *)this->pEndOfCentralDirectoryRecord + sizeof(ZIPEndOfCentralDirectoryRecord), this->pEndOfCentralDirectoryRecord->uiCommentLength);
							lpComment[this->pEndOfCentralDirectoryRecord->uiCommentLength] = '\0';

							hlAttributeSetString(&Attribute, this->lpItemAttributeNames[eAttribute], lpComment);

							delete []lpComment;
							return hlTrue;
						}
					}
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CVBSPFile::GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const
{
	if(pFile->GetData())
	{
		const ZIPFileHeader *pDirectoryItem = static_cast<const ZIPFileHeader *>(pFile->GetData());

		bExtractable = pDirectoryItem->uiCompressionMethod == 0 && pDirectoryItem->uiDiskNumberStart == this->pEndOfCentralDirectoryRecord->uiNumberOfThisDisk;
	}
	else
	{
		bExtractable = hlTrue;
	}

	return hlTrue;
}

hlBool CVBSPFile::GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const
{
	if(pFile->GetData())
	{
		const ZIPFileHeader *pDirectoryItem = static_cast<const ZIPFileHeader *>(pFile->GetData());

		if(pDirectoryItem->uiCompressionMethod != 0 || pDirectoryItem->uiDiskNumberStart != this->pEndOfCentralDirectoryRecord->uiNumberOfThisDisk)
		{
			eValidation = HL_VALIDATES_ASSUMED_OK;
			return hlTrue;
		}

		hlULong uiChecksum = 0;
		Streams::IStream *pStream = 0;
		if(this->CreateStreamInternal(pFile, pStream))
		{
			if(pStream->Open(HL_MODE_READ))
			{
				hlULongLong uiTotalBytes = 0, uiFileBytes = pStream->GetStreamSize();
				hlUInt uiBufferSize;
				hlByte lpBuffer[HL_VBSP_ZIP_CHECKSUM_LENGTH];

				hlBool bCancel = hlFalse;
				hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);

				while((uiBufferSize = pStream->Read(lpBuffer, sizeof(lpBuffer))) != 0)
				{
					if(bCancel)
					{
						eValidation = HL_VALIDATES_CANCELED;
						break;
					}

					uiChecksum = CRC32(lpBuffer, uiBufferSize, uiChecksum);

					uiTotalBytes += static_cast<hlULongLong>(uiBufferSize);

					hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);
				}

				pStream->Close();
			}

			this->ReleaseStreamInternal(*pStream);
			delete pStream;
		}

		if(eValidation == HL_VALIDATES_ASSUMED_OK)
		{
			eValidation = (hlULong)pDirectoryItem->uiCRC32 == uiChecksum ? HL_VALIDATES_OK : HL_VALIDATES_CORRUPT;
		}
	}
	else
	{
		eValidation = HL_VALIDATES_ASSUMED_OK;
	}

	return hlTrue;
}

hlBool CVBSPFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	if(pFile->GetData())
	{
		const ZIPFileHeader *pDirectoryItem = static_cast<const ZIPFileHeader *>(pFile->GetData());

		uiSize = pDirectoryItem->uiUncompressedSize;
	}
	else if(pFile->GetID() < HL_VBSP_LUMP_COUNT)
	{
		uiSize = this->pHeader->lpLumps[pFile->GetID()].uiLength;
	}
	else
	{
		uiSize = sizeof(LMPHeader) + this->pHeader->lpLumps[pFile->GetID() - HL_VBSP_LUMP_COUNT].uiLength;
	}

	return hlTrue;
}

hlBool CVBSPFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	if(pFile->GetData())
	{
		const ZIPFileHeader *pDirectoryItem = static_cast<const ZIPFileHeader *>(pFile->GetData());

		uiSize = pDirectoryItem->uiCompressedSize;
	}
	else if(pFile->GetID() < HL_VBSP_LUMP_COUNT)
	{
		uiSize = this->pHeader->lpLumps[pFile->GetID()].uiLength;
	}
	else
	{
		uiSize = this->pHeader->lpLumps[pFile->GetID() - HL_VBSP_LUMP_COUNT].uiLength;
	}

	return hlTrue;
}

hlBool CVBSPFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	if(pFile->GetData())
	{
		const ZIPFileHeader *pDirectoryItem = static_cast<const ZIPFileHeader *>(pFile->GetData());

		if(pDirectoryItem->uiCompressionMethod != 0)
		{
			LastError.SetErrorMessageFormated("Compression format %#.2x not supported.", pDirectoryItem->uiCompressionMethod);
			return hlFalse;
		}

		if(pDirectoryItem->uiDiskNumberStart != this->pEndOfCentralDirectoryRecord->uiNumberOfThisDisk)
		{
			LastError.SetErrorMessageFormated("File resides on disk %u.", pDirectoryItem->uiDiskNumberStart);
			return hlFalse;
		}

		Mapping::CView *pDirectoryEnrtyView = 0;

		if(!this->pMapping->Map(pDirectoryEnrtyView, this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiOffset + pDirectoryItem->uiRelativeOffsetOfLocalHeader, sizeof(ZIPLocalFileHeader)))
		{
			return hlFalse;
		}

		const ZIPLocalFileHeader DirectoryEntry = *static_cast<const ZIPLocalFileHeader *>(pDirectoryEnrtyView->GetView());

		this->pMapping->Unmap(pDirectoryEnrtyView);

		if(DirectoryEntry.uiSignature != HL_VBSP_ZIP_LOCAL_FILE_HEADER_SIGNATURE)
		{
			LastError.SetErrorMessageFormated("Invalid file data offset %u.", pDirectoryItem->uiDiskNumberStart);
			return hlFalse;
		}

		pStream = new Streams::CMappingStream(*this->pMapping, this->pHeader->lpLumps[HL_VBSP_LUMP_PAKFILE].uiOffset + pDirectoryItem->uiRelativeOffsetOfLocalHeader + sizeof(ZIPLocalFileHeader) + DirectoryEntry.uiFileNameLength + DirectoryEntry.uiExtraFieldLength, DirectoryEntry.uiUncompressedSize);
	}
	else if(pFile->GetID() < HL_VBSP_LUMP_COUNT)
	{
		pStream = new Streams::CMappingStream(*this->pMapping, this->pHeader->lpLumps[pFile->GetID()].uiOffset, this->pHeader->lpLumps[pFile->GetID()].uiLength);
	}
	else
	{
		hlUInt uiID = pFile->GetID() - HL_VBSP_LUMP_COUNT;

		Mapping::CView *pLumpView = 0;
		if(!this->pMapping->Map(pLumpView, this->pHeader->lpLumps[uiID].uiOffset, this->pHeader->lpLumps[uiID].uiLength))
		{
			return hlFalse;
		}

		hlUInt uiBufferSize = sizeof(LMPHeader) + this->pHeader->lpLumps[uiID].uiLength;
		hlByte *lpBuffer = new hlByte[uiBufferSize];

		LMPHeader *pLMPHeader = (LMPHeader *)lpBuffer;
		pLMPHeader->iLumpOffset = sizeof(LMPHeader);
		pLMPHeader->iLumpID = uiID;
		pLMPHeader->iLumpVersion = this->pHeader->lpLumps[uiID].uiVersion;
		pLMPHeader->iLumpLength = this->pHeader->lpLumps[uiID].uiLength;
		pLMPHeader->iMapRevision = this->pHeader->iMapRevision;

		memcpy(lpBuffer + sizeof(LMPHeader), pLumpView->GetView(), this->pHeader->lpLumps[uiID].uiLength);

		pStream = new Streams::CMemoryStream(lpBuffer, uiBufferSize);

		this->pMapping->Unmap(pLumpView);
	}

	return hlTrue;
}

hlVoid CVBSPFile::ReleaseStreamInternal(Streams::IStream &Stream) const
{
	if(Stream.GetType() == HL_STREAM_MEMORY)
	{
		delete [](hlByte *)static_cast<Streams::CMemoryStream &>(Stream).GetBuffer();
	}
}

hlVoid CVBSPFile::GetFileName(hlChar *lpBuffer, hlUInt uiBufferSize)
{
	if(lpBuffer == 0 || uiBufferSize == 0)
	{
		return;
	}

	const hlChar *lpMappingName = this->pMapping->GetFileName();

	if(lpMappingName == 0 && *lpMappingName == '\0')
	{
		*lpBuffer = '\0';
		return;
	}

	const hlChar *lpForward = strrchr(lpMappingName, '\\');
	const hlChar *lpBackward = strrchr(lpMappingName, '/');
	const hlChar *lpStart = lpForward > lpBackward ? lpForward : lpBackward;

	if(lpStart == 0)
	{
		lpStart = lpMappingName;
	}
	else
	{
		lpStart++;
	}

	const hlChar *lpEnd = strrchr(lpStart, '.');

	if(lpEnd == 0)
	{
		lpEnd = lpStart + strlen(lpStart);
	}

	if((hlUInt)(lpEnd - lpStart) + 1 < uiBufferSize)
	{
		uiBufferSize = (hlUInt)(lpEnd - lpStart) + 1;
	}

	strncpy(lpBuffer, lpStart, uiBufferSize);
	lpBuffer[uiBufferSize - 1] = '\0';
}
