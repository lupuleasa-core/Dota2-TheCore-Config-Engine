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
#include "GCFFile.h"
#include "Streams.h"
#include "Checksum.h"

using namespace HLLib;

#define HL_GCF_FLAG_FILE						0x00004000	// The item is a file.
#define HL_GCF_FLAG_ENCRYPTED					0x00000100	// The item is encrypted.
#define HL_GCF_FLAG_BACKUP_LOCAL				0x00000040	// Backup the item before overwriting it.
#define HL_GCF_FLAG_COPY_LOCAL					0x0000000a	// The item is to be copied to the disk.
#define HL_GCF_FLAG_COPY_LOCAL_NO_OVERWRITE 	0x00000001	// Don't overwrite the item if copying it to the disk and the item already exists.

#define HL_GCF_CHECKSUM_LENGTH					0x00008000	// The maximum data allowed in a 32 bit checksum.

const char *CGCFFile::lpAttributeNames[] = { "Version", "Cache ID", "Allocated Blocks", "Used Blocks", "Block Length", "Last Version Played" };
const char *CGCFFile::lpItemAttributeNames[] = { "Encrypted", "Copy Locally", "Overwrite Local Copy", "Backup Local Copy", "Flags", "Fragmentation" };

CGCFFile::CGCFFile() : CPackage(), pHeaderView(0)
{
	this->pHeader = 0;

	this->pBlockEntryHeader = 0;
	this->lpBlockEntries = 0;

	this->pFragmentationMapHeader = 0;
	this->lpFragmentationMap = 0;

	this->pBlockEntryMapHeader = 0;
	this->lpBlockEntryMap = 0;

	this->pDirectoryHeader = 0;
	this->lpDirectoryEntries = 0;
	this->lpDirectoryNames = 0;
	this->lpDirectoryInfo1Entries = 0;
	this->lpDirectoryInfo2Entries = 0;
	this->lpDirectoryCopyEntries = 0;
	this->lpDirectoryLocalEntries = 0;

	this->pDirectoryMapHeader = 0;
	this->lpDirectoryMapEntries = 0;

	this->pChecksumHeader = 0;
	this->pChecksumMapHeader = 0;
	this->lpChecksumMapEntries = 0;
	this->lpChecksumEntries = 0;

	this->pDataBlockHeader = 0;

	this->lpDirectoryItems = 0;
}

CGCFFile::~CGCFFile()
{
	this->Close();
}

HLPackageType CGCFFile::GetType() const
{
	return HL_PACKAGE_GCF;
}

const hlChar *CGCFFile::GetExtension() const
{
	return "gcf";
}

const hlChar *CGCFFile::GetDescription() const
{
	return "Half-Life Game Cache File";
}

hlBool CGCFFile::MapDataStructures()
{
	//
	// Determine the size of the header and validate it.
	//

	hlUInt uiVersion = 0;
	hlUInt uiHeaderSize = 0;

	// Header.

	if(sizeof(GCFHeader) > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's header.");
		return hlFalse;
	}

	if(!this->pMapping->Map(this->pHeaderView, 0, sizeof(GCFHeader)))
	{
		return hlFalse;
	}
	this->pHeader = (GCFHeader *)this->pHeaderView->GetView();

	hlBool bNull = hlTrue;
	for(hlByte *pTest = (hlByte *)this->pHeader; pTest <  (hlByte *)this->pHeader + sizeof(GCFHeader); pTest++)
	{
		if(*pTest != 0)
		{
			bNull = hlFalse;
			break;
		}
	}

	if(bNull)
	{
		LastError.SetErrorMessage("Invalid file: the file's header is null (contains no data).");
		return hlFalse;
	}

	if(this->pHeader->uiMajorVersion != 1 || (this->pHeader->uiMinorVersion != 3 && this->pHeader->uiMinorVersion != 5 && this->pHeader->uiMinorVersion != 6))
	{
		LastError.SetErrorMessageFormated("Invalid GCF version (v%u): you have a version of a GCF file that HLLib does not know how to read. Check for product updates.", this->pHeader->uiMinorVersion);
		return hlFalse;
	}

	uiVersion = this->pHeader->uiMinorVersion;
	uiHeaderSize += sizeof(GCFHeader);

	// Block entries.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(GCFBlockEntryHeader)))
	{
		return hlFalse;
	}
	this->pBlockEntryHeader = (GCFBlockEntryHeader *)this->pHeaderView->GetView();

	uiHeaderSize += sizeof(GCFBlockEntryHeader) + this->pBlockEntryHeader->uiBlockCount * sizeof(GCFBlockEntry);

	// Fragmentation map.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(GCFFragmentationMapHeader)))
	{
		return hlFalse;
	}
	this->pFragmentationMapHeader = (GCFFragmentationMapHeader *)this->pHeaderView->GetView();

	uiHeaderSize += sizeof(GCFFragmentationMapHeader) + this->pFragmentationMapHeader->uiBlockCount * sizeof(GCFFragmentationMap);

	// Block entry map.

	if(uiVersion < 6)
	{
		if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(GCFBlockEntryMapHeader)))
		{
			return hlFalse;
		}
		this->pBlockEntryMapHeader = (GCFBlockEntryMapHeader *)this->pHeaderView->GetView();

		uiHeaderSize += sizeof(GCFBlockEntryMapHeader) + this->pBlockEntryMapHeader->uiBlockCount * sizeof(GCFBlockEntryMap);
	}

	// Directory.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(GCFDirectoryHeader)))
	{
		return hlFalse;
	}
	this->pDirectoryHeader = (GCFDirectoryHeader *)this->pHeaderView->GetView();

	uiHeaderSize += this->pDirectoryHeader->uiDirectorySize;/*sizeof(GCFDirectoryHeader);
				 + this->pDirectoryHeader->uiItemCount * sizeof(GCFDirectoryEntry)
				 + this->pDirectoryHeader->uiNameSize
				 + this->pDirectoryHeader->uiInfo1Count * sizeof(GCFDirectoryInfo1Entry)
				 + this->pDirectoryHeader->uiItemCount * sizeof(GCFDirectoryInfo2Entry)
				 + this->pDirectoryHeader->uiCopyCount * sizeof(GCFDirectoryCopyEntry)
				 + this->pDirectoryHeader->uiLocalCount * sizeof(GCFDirectoryLocalEntry);*/

	if(uiVersion >= 5)
	{
		uiHeaderSize += sizeof(GCFDirectoryMapHeader);
	}

	uiHeaderSize += this->pDirectoryHeader->uiItemCount * sizeof(GCFDirectoryMapEntry);

	// Checksums.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(GCFChecksumHeader)))
	{
		return hlFalse;
	}
	this->pChecksumHeader = (GCFChecksumHeader *)this->pHeaderView->GetView();

	uiHeaderSize += sizeof(GCFChecksumHeader) + this->pChecksumHeader->uiChecksumSize;

	// Data blocks.

	if(!this->pMapping->Map(this->pHeaderView, uiHeaderSize, sizeof(GCFDataBlockHeader)))
	{
		return hlFalse;
	}
	this->pDataBlockHeader = (GCFDataBlockHeader *)this->pHeaderView->GetView();

	// It seems that some GCF files may have allocated only the data blocks that are used.  Extraction,
	// validation and defragmentation should fail cleanly if an unallocated data block is indexed so
	// leave this check out for now.
	/*if(this->pDataBlockHeader->uiFirstBlockOffset + this->pDataBlockHeader->uiBlocksUsed * this->pDataBlockHeader->uiBlockSize > this->pMapping->GetMappingSize())
	{
		LastError.SetErrorMessage("Invalid file: the file map is too small for it's data blocks.");
		return hlFalse;
	}*/

	if(uiVersion < 5)
	{
		// See note below.
		uiHeaderSize += sizeof(GCFDataBlockHeader) - sizeof(hlUInt);
	}
	else
	{
		uiHeaderSize += sizeof(GCFDataBlockHeader);
	}

	//
	// Map the header.
	//

	if(!this->pMapping->Map(this->pHeaderView, 0, uiHeaderSize))
	{
		return hlFalse;
	}

	this->pHeader = (GCFHeader *)this->pHeaderView->GetView();

	this->pBlockEntryHeader = (GCFBlockEntryHeader *)((hlByte *)this->pHeader + sizeof(GCFHeader));
	this->lpBlockEntries = (GCFBlockEntry *)((hlByte *)this->pBlockEntryHeader + sizeof(GCFBlockEntryHeader));

	this->pFragmentationMapHeader = (GCFFragmentationMapHeader *)((hlByte *)this->lpBlockEntries + sizeof(GCFBlockEntry) * this->pBlockEntryHeader->uiBlockCount);
	this->lpFragmentationMap = (GCFFragmentationMap *)((hlByte *)this->pFragmentationMapHeader + sizeof(GCFFragmentationMapHeader));

	if(uiVersion < 6)
	{
		this->pBlockEntryMapHeader = (GCFBlockEntryMapHeader *)((hlByte *)this->lpFragmentationMap + sizeof(GCFFragmentationMap) * this->pFragmentationMapHeader->uiBlockCount);
		this->lpBlockEntryMap = (GCFBlockEntryMap *)((hlByte *)this->pBlockEntryMapHeader + sizeof(GCFBlockEntryMapHeader));

		this->pDirectoryHeader = (GCFDirectoryHeader *)((hlByte *)this->lpBlockEntryMap + sizeof(GCFBlockEntryMap) * this->pBlockEntryMapHeader->uiBlockCount);
	}
	else
	{
		this->pBlockEntryMapHeader = 0;
		this->lpBlockEntryMap = 0;

		this->pDirectoryHeader = (GCFDirectoryHeader *)((hlByte *)this->lpFragmentationMap + sizeof(GCFFragmentationMap) * this->pFragmentationMapHeader->uiBlockCount);
	}

	this->lpDirectoryEntries = (GCFDirectoryEntry *)((hlByte *)this->pDirectoryHeader + sizeof(GCFDirectoryHeader));

	this->lpDirectoryNames = (hlChar *)((hlByte *)this->lpDirectoryEntries + sizeof(GCFDirectoryEntry) * this->pDirectoryHeader->uiItemCount);

	this->lpDirectoryInfo1Entries = (GCFDirectoryInfo1Entry *)((hlByte *)this->lpDirectoryNames + this->pDirectoryHeader->uiNameSize);
	this->lpDirectoryInfo2Entries = (GCFDirectoryInfo2Entry *)((hlByte *)this->lpDirectoryInfo1Entries + sizeof(GCFDirectoryInfo1Entry) * this->pDirectoryHeader->uiInfo1Count);

	this->lpDirectoryCopyEntries = (GCFDirectoryCopyEntry *)((hlByte *)this->lpDirectoryInfo2Entries + sizeof(GCFDirectoryInfo2Entry) * this->pDirectoryHeader->uiItemCount);
	this->lpDirectoryLocalEntries = (GCFDirectoryLocalEntry *)((hlByte *)this->lpDirectoryCopyEntries + sizeof(GCFDirectoryCopyEntry) * this->pDirectoryHeader->uiCopyCount);

	if(uiVersion < 5)
	{
		this->pDirectoryMapHeader = 0;
		this->lpDirectoryMapEntries = (GCFDirectoryMapEntry *)((hlByte *)this->pDirectoryHeader + this->pDirectoryHeader->uiDirectorySize);
	}
	else
	{
		this->pDirectoryMapHeader = (GCFDirectoryMapHeader *)((hlByte *)this->pDirectoryHeader + this->pDirectoryHeader->uiDirectorySize);
		this->lpDirectoryMapEntries = (GCFDirectoryMapEntry *)((hlByte *)this->pDirectoryMapHeader + sizeof(GCFDirectoryMapHeader));
	}

	this->pChecksumHeader = (GCFChecksumHeader *)((hlByte *)this->lpDirectoryMapEntries + sizeof(GCFDirectoryMapEntry) * this->pDirectoryHeader->uiItemCount);
	this->pChecksumMapHeader = (GCFChecksumMapHeader *)((hlByte *)(this->pChecksumHeader) + sizeof(GCFChecksumHeader));

	this->lpChecksumMapEntries = (GCFChecksumMapEntry *)((hlByte *)(this->pChecksumMapHeader) + sizeof(GCFChecksumMapHeader));
	this->lpChecksumEntries = (GCFChecksumEntry *)((hlByte *)(this->lpChecksumMapEntries) + sizeof(GCFChecksumMapEntry) * this->pChecksumMapHeader->uiItemCount);

	if(uiVersion < 5)
	{
		// In version 3 the GCFDataBlockHeader is missing the uiLastVersionPlayed field.
		// The below hack makes the file map correctly.
		this->pDataBlockHeader = (GCFDataBlockHeader *)((hlByte *)this->pChecksumMapHeader + this->pChecksumHeader->uiChecksumSize - sizeof(hlUInt));
	}
	else
	{
		this->pDataBlockHeader = (GCFDataBlockHeader *)((hlByte *)this->pChecksumMapHeader + this->pChecksumHeader->uiChecksumSize);
	}

	return hlTrue;
}

hlVoid CGCFFile::UnmapDataStructures()
{
	delete []this->lpDirectoryItems;
	this->lpDirectoryItems = 0;

	this->pHeader = 0;

	this->pBlockEntryHeader = 0;
	this->lpBlockEntries = 0;

	this->pFragmentationMapHeader = 0;
	this->lpFragmentationMap = 0;

	this->pBlockEntryMapHeader = 0;
	this->lpBlockEntryMap = 0;

	this->pDirectoryHeader = 0;
	this->lpDirectoryEntries = 0;
	this->lpDirectoryNames = 0;
	this->lpDirectoryInfo1Entries = 0;
	this->lpDirectoryInfo2Entries = 0;
	this->lpDirectoryCopyEntries = 0;
	this->lpDirectoryLocalEntries = 0;

	this->pDirectoryMapHeader = 0;
	this->lpDirectoryMapEntries = 0;

	this->pChecksumHeader = 0;
	this->pChecksumMapHeader = 0;
	this->lpChecksumMapEntries = 0;
	this->lpChecksumEntries = 0;

	this->pDataBlockHeader = 0;

	this->pMapping->Unmap(this->pHeaderView);
}

hlBool CGCFFile::DefragmentInternal()
{
	hlBool bError = hlFalse, bCancel = hlFalse;
	hlUInt uiFilesDefragmented = 0, uiFilesTotal = 0;
	hlULongLong uiBytesDefragmented = 0, uiBytesTotal = 0;

	// Check the current fragmentation state.
	{
		hlUInt uiBlocksFragmented = 0;
		hlUInt uiBlocksUsed = 0;

		for(hlUInt i = 0; i < this->pDirectoryHeader->uiItemCount; i++)
		{
			if(this->lpDirectoryEntries[i].uiDirectoryFlags & HL_GCF_FLAG_FILE)
			{
				hlUInt uiFileBlocksFragmented = 0;
				hlUInt uiFileBlocksUsed = 0;

				this->GetItemFragmentation(i, uiFileBlocksFragmented, uiFileBlocksUsed);

				uiBlocksFragmented += uiFileBlocksFragmented;
				uiBlocksUsed += uiFileBlocksUsed;

				uiFilesTotal++;
				uiBytesTotal += static_cast<hlULongLong>(uiFileBlocksUsed * this->pDataBlockHeader->uiBlockSize);
			}
		}

		// If there are no data blocks to defragment, and we don't want to sort the data blocks
		// lexicographically, then we're done.
		if((uiBlocksFragmented == 0 && !bForceDefragment) || uiBlocksUsed == 0)
		{
			hlDefragmentProgress(0, uiFilesTotal, uiFilesTotal, uiBytesTotal, uiBytesTotal, &bCancel);

			return hlTrue;
		}
	}

	Mapping::CView *pCurrentView = 0, *pIncrementedView = 0;
	hlUInt uiIncrement = 0;

	//hlUInt uiDataBlockTerminator = this->pDataBlockHeader->uiBlockCount >= 0x0000ffff ? 0xffffffff : 0x0000ffff;
	hlUInt uiDataBlockTerminator = this->pFragmentationMapHeader->uiTerminator == 0 ? 0x0000ffff : 0xffffffff;

	hlByte *lpDataBlock = new hlByte[this->pDataBlockHeader->uiBlockSize];

	// Step through each data block in each file as it appears in the directory and defragment (order each data
	// block sequentially).  This is a slow brute force approach, but since related half-life 2 files
	// often start with the same name we *may* expect a performance boost from odering files lexicographically
	// (as they appear in the directory).  That's my justification at least...
	for(hlUInt i = 0; i < this->pDirectoryHeader->uiItemCount && !bError && !bCancel; i++)
	{
		if(this->lpDirectoryEntries[i].uiDirectoryFlags & HL_GCF_FLAG_FILE)
		{
			hlUInt uiCurrentBlockEntryIndex = this->lpDirectoryMapEntries[i].uiFirstBlockIndex;

			while(uiCurrentBlockEntryIndex != this->pDataBlockHeader->uiBlockCount)
			{
				hlUInt uiCurrentBlockEntrySize = 0;
				hlUInt uiCurrentDataBlockIndex = this->lpBlockEntries[uiCurrentBlockEntryIndex].uiFirstDataBlockIndex;
				hlUInt uiLastDataBlockIndex = this->pDataBlockHeader->uiBlockCount;

				while(uiCurrentDataBlockIndex < uiDataBlockTerminator && uiCurrentBlockEntrySize < this->lpBlockEntries[uiCurrentBlockEntryIndex].uiFileDataSize)
				{
					hlUInt uiNextDataBlockIndex = this->lpFragmentationMap[uiCurrentDataBlockIndex].uiNextDataBlockIndex;

					// If this data block is not ordered sequentially, swap it with the sequential data block.
					if(uiCurrentDataBlockIndex != uiIncrement)
					{
						// Make sure we can map the two data blocks before we alter any tables.
						if(this->pMapping->Map(pCurrentView, static_cast<hlULongLong>(this->pDataBlockHeader->uiFirstBlockOffset) + static_cast<hlULongLong>(uiCurrentDataBlockIndex) * static_cast<hlULongLong>(this->pDataBlockHeader->uiBlockSize), static_cast<hlULongLong>(this->pDataBlockHeader->uiBlockSize)) &&
							this->pMapping->Map(pIncrementedView, static_cast<hlULongLong>(this->pDataBlockHeader->uiFirstBlockOffset) + static_cast<hlULongLong>(uiIncrement) * static_cast<hlULongLong>(this->pDataBlockHeader->uiBlockSize), static_cast<hlULongLong>(this->pDataBlockHeader->uiBlockSize)))
						{
							// Search to see if the sequential data block is in use, we only need to check
							// files after ours in the directory because everything before is sequential.
							hlBool bFound = hlFalse;
							for(hlUInt j = i; j < this->pDirectoryHeader->uiItemCount && !bFound; j++)
							{
								if(this->lpDirectoryEntries[j].uiDirectoryFlags & HL_GCF_FLAG_FILE)
								{
									hlUInt uiIncrementedBlockEntryIndex = this->lpDirectoryMapEntries[j].uiFirstBlockIndex;

									while(uiIncrementedBlockEntryIndex != this->pDataBlockHeader->uiBlockCount && !bFound)
									{
										hlUInt uiIncrementedDataBlockIndex = this->lpBlockEntries[uiIncrementedBlockEntryIndex].uiFirstDataBlockIndex;

										if(uiIncrementedDataBlockIndex == uiIncrement)
										{
											// The sequential data block is the first data block in a block entry,
											// update the tables preserving the sequence for the file we are fragmenting.

											this->lpBlockEntries[uiIncrementedBlockEntryIndex].uiFirstDataBlockIndex = uiCurrentDataBlockIndex;

											this->lpFragmentationMap[uiCurrentDataBlockIndex].uiNextDataBlockIndex = this->lpFragmentationMap[uiIncrementedDataBlockIndex].uiNextDataBlockIndex;

											// We found it.
											bFound = hlTrue;
											break;
										}
										else
										{
											// The sequential data block is in the middle of a block entry,
											// update the tables preserving the sequence for the file we are fragmenting.

											hlUInt uiIncrementedBlockEntrySize = 0;
											hlUInt uiIncrementedLastDataBlockIndex = this->pDataBlockHeader->uiBlockCount;

											while(uiIncrementedDataBlockIndex < uiDataBlockTerminator && uiIncrementedBlockEntrySize < this->lpBlockEntries[uiIncrementedBlockEntryIndex].uiFileDataSize)
											{
												if(uiIncrementedDataBlockIndex == uiIncrement)
												{
													// If the data blocks are side by side, prevent circular maps.
													if(uiIncrement != uiNextDataBlockIndex)
													{
														this->lpFragmentationMap[uiIncrementedLastDataBlockIndex].uiNextDataBlockIndex = uiCurrentDataBlockIndex;
													}

													this->lpFragmentationMap[uiCurrentDataBlockIndex].uiNextDataBlockIndex = this->lpFragmentationMap[uiIncrementedDataBlockIndex].uiNextDataBlockIndex;

													// We found it.
													bFound = hlTrue;
													break;
												}

												uiIncrementedLastDataBlockIndex = uiIncrementedDataBlockIndex;

												uiIncrementedDataBlockIndex = this->lpFragmentationMap[uiIncrementedDataBlockIndex].uiNextDataBlockIndex;
												uiIncrementedBlockEntrySize += this->pDataBlockHeader->uiBlockSize;
											}
										}

										uiIncrementedBlockEntryIndex = this->lpBlockEntries[uiIncrementedBlockEntryIndex].uiNextBlockEntryIndex;
									}
								}
							}

							// Swap the data blocks if necessary.
							if(bFound)
							{
								memcpy(lpDataBlock, pIncrementedView->GetView(), this->pDataBlockHeader->uiBlockSize);
							}
							memcpy(const_cast<hlVoid *>(pIncrementedView->GetView()), pCurrentView->GetView(), this->pDataBlockHeader->uiBlockSize);
							if(!this->pMapping->Commit(*pIncrementedView))
							{
								// At this point the GCF will be corrupt and require validating by Steam for repair.
								bError = hlTrue;
							}
							if(bFound)
							{
								memcpy(const_cast<hlVoid *>(pCurrentView->GetView()), lpDataBlock, this->pDataBlockHeader->uiBlockSize);
								if(!this->pMapping->Commit(*pCurrentView))
								{
									// At this point the GCF will be corrupt and require validating by Steam for repair.
									bError = hlTrue;
								}
							}

							// Update the tables preserving the sequence of the file we are defragmenting.
							if(uiLastDataBlockIndex == this->pDataBlockHeader->uiBlockCount)
							{
								this->lpBlockEntries[uiCurrentBlockEntryIndex].uiFirstDataBlockIndex = uiIncrement;
							}
							else
							{
								this->lpFragmentationMap[uiLastDataBlockIndex].uiNextDataBlockIndex = uiIncrement;
							}

							if(uiIncrement != this->pDataBlockHeader->uiBlockCount)
							{
								// If the data blocks are side by side, prevent circular maps.
								if(uiIncrement != uiNextDataBlockIndex)
								{
									this->lpFragmentationMap[uiIncrement].uiNextDataBlockIndex = uiNextDataBlockIndex;
								}
								else
								{
									this->lpFragmentationMap[uiIncrement].uiNextDataBlockIndex = uiCurrentDataBlockIndex;
								}
							}
						}
						else
						{
							bError = hlTrue;
							break;
						}
					}

					// Move to the next data block.
					uiLastDataBlockIndex = uiIncrement;
					uiIncrement++;

					uiCurrentDataBlockIndex = this->lpFragmentationMap[uiLastDataBlockIndex].uiNextDataBlockIndex;
					uiCurrentBlockEntrySize += this->pDataBlockHeader->uiBlockSize;

					// Update the progress.
					uiBytesDefragmented += static_cast<hlULongLong>(this->pDataBlockHeader->uiBlockSize);
					hlDefragmentProgress(this->lpDirectoryItems != 0 ? static_cast<CDirectoryFile *>(this->lpDirectoryItems[i]) : 0, uiFilesDefragmented, uiFilesTotal, uiBytesDefragmented, uiBytesTotal, &bCancel);
					if(bCancel)
					{
						break;
					}
				}

				if(bError || bCancel)
				{
					break;
				}

				// Update the tables to make sure the last data block points to nothing.
				// It would seem that this is only necessary if there is an eror in the GCF
				// in which case we should just let it be (Steam knows better)...
				/*if(uiLastDataBlockIndex != this->pDataBlockHeader->uiBlockCount)
				{
					this->lpFragmentationMap[uiLastDataBlockIndex].uiNextDataBlockIndex = uiDataBlockTerminator;
				}*/

				uiCurrentBlockEntryIndex = this->lpBlockEntries[uiCurrentBlockEntryIndex].uiNextBlockEntryIndex;
			}

			uiFilesDefragmented++;
		}
	}

	if(!bError && !bCancel)
	{
		// Store the first unused fragmentation map entry.
		if(uiIncrement < this->pFragmentationMapHeader->uiBlockCount)
		{
			// Note: usually if there are no unused data blocks, uiFirstUnusedEntry is 0, but
			// sometimes it isn't (and I don't know why) so I'm unsure if I should set it to
			// 0 in the else case.
			this->pFragmentationMapHeader->uiFirstUnusedEntry = uiIncrement;
			this->pFragmentationMapHeader->uiChecksum = this->pFragmentationMapHeader->uiBlockCount +
														this->pFragmentationMapHeader->uiFirstUnusedEntry +
														this->pFragmentationMapHeader->uiTerminator;
		}

		// Fill in the unused fragmentation map entries with uiBlockCount.
		for(hlUInt i = uiIncrement; i < this->pFragmentationMapHeader->uiBlockCount; i++)
		{
			this->lpFragmentationMap[i].uiNextDataBlockIndex = this->pFragmentationMapHeader->uiBlockCount;
		}
	}
	else
	{
		hlByte *lpTouched = new hlByte[this->pFragmentationMapHeader->uiBlockCount];
		memset(lpTouched, 0, sizeof(hlByte) * this->pFragmentationMapHeader->uiBlockCount);

		// Figure out which fragmentation map entries are used.
		for(hlUInt i = 0; i < this->pDirectoryHeader->uiItemCount; i++)
		{
			if(this->lpDirectoryEntries[i].uiDirectoryFlags & HL_GCF_FLAG_FILE)
			{
				hlUInt uiBlockEntryIndex = this->lpDirectoryMapEntries[i].uiFirstBlockIndex;

				while(uiBlockEntryIndex != this->pDataBlockHeader->uiBlockCount)
				{
					hlUInt uiBlockEntrySize = 0;
					hlUInt uiDataBlockIndex = this->lpBlockEntries[uiBlockEntryIndex].uiFirstDataBlockIndex;

					while(uiDataBlockIndex < uiDataBlockTerminator && uiBlockEntrySize < this->lpBlockEntries[uiBlockEntryIndex].uiFileDataSize)
					{
						lpTouched[uiDataBlockIndex] = hlTrue;

						uiDataBlockIndex = this->lpFragmentationMap[uiDataBlockIndex].uiNextDataBlockIndex;

						uiBlockEntrySize += this->pDataBlockHeader->uiBlockSize;
					}

					uiBlockEntryIndex = this->lpBlockEntries[uiBlockEntryIndex].uiNextBlockEntryIndex;
				}
			}
		}

		// Fill in the unused fragmentation map entries with uiBlockCount.
		hlBool bFirst = hlFalse;
		for(hlUInt i = 0; i < this->pFragmentationMapHeader->uiBlockCount; i++)
		{
			if(!lpTouched[i])
			{
				if(!bFirst)
				{
					// Store the first unused fragmentation map entry.
					this->pFragmentationMapHeader->uiFirstUnusedEntry = i;
					this->pFragmentationMapHeader->uiChecksum = this->pFragmentationMapHeader->uiBlockCount +
																this->pFragmentationMapHeader->uiFirstUnusedEntry +
																this->pFragmentationMapHeader->uiTerminator;

					bFirst = hlTrue;
				}
				this->lpFragmentationMap[i].uiNextDataBlockIndex = this->pFragmentationMapHeader->uiBlockCount;
			}
		}

		delete []lpTouched;
	}

	delete []lpDataBlock;

	this->pMapping->Unmap(pCurrentView);
	this->pMapping->Unmap(pIncrementedView);

	// Commit header changes to mapping.
	this->pMapping->Commit(*this->pHeaderView, (hlUInt)((const hlByte *)this->lpBlockEntries - (const hlByte *)this->pHeaderView->GetView()), sizeof(GCFBlockEntry) * this->pBlockEntryHeader->uiBlockCount);
	this->pMapping->Commit(*this->pHeaderView, (hlUInt)((const hlByte *)this->pFragmentationMapHeader - (const hlByte *)this->pHeaderView->GetView()), sizeof(GCFFragmentationMapHeader));
	this->pMapping->Commit(*this->pHeaderView, (hlUInt)((const hlByte *)this->lpFragmentationMap - (const hlByte *)this->pHeaderView->GetView()), sizeof(GCFFragmentationMap) * this->pFragmentationMapHeader->uiBlockCount);

	return !bError;
}

CDirectoryFolder *CGCFFile::CreateRoot()
{
	this->lpDirectoryItems = new CDirectoryItem *[this->pDirectoryHeader->uiItemCount];

	this->lpDirectoryItems[0] = new CDirectoryFolder("root", 0, 0, this, 0);

	this->CreateRoot(static_cast<CDirectoryFolder *>(this->lpDirectoryItems[0]));

	return static_cast<CDirectoryFolder *>(this->lpDirectoryItems[0]);
}

hlVoid CGCFFile::CreateRoot(CDirectoryFolder *pFolder)
{
	// Get the first directory item.
	hlUInt uiIndex = this->lpDirectoryEntries[pFolder->GetID()].uiFirstIndex;

	// Loop through directory items.
	while(uiIndex && uiIndex != 0xffffffff)
	{
		// Check if the item is a folder.
		if((this->lpDirectoryEntries[uiIndex].uiDirectoryFlags & HL_GCF_FLAG_FILE) == 0)
		{
			// Add the directory item to the current folder.
			this->lpDirectoryItems[uiIndex] = pFolder->AddFolder(this->lpDirectoryNames + this->lpDirectoryEntries[uiIndex].uiNameOffset, uiIndex);

			// Build the new folder.
			this->CreateRoot(static_cast<CDirectoryFolder *>(this->lpDirectoryItems[uiIndex]));
		}
		else
		{
			// Add the directory item to the current folder.
			this->lpDirectoryItems[uiIndex] = pFolder->AddFile(this->lpDirectoryNames + this->lpDirectoryEntries[uiIndex].uiNameOffset, uiIndex);
		}

		// Get the next directory item.
		uiIndex = this->lpDirectoryEntries[uiIndex].uiNextIndex;
	}
}

hlUInt CGCFFile::GetAttributeCountInternal() const
{
	return HL_GCF_PACKAGE_COUNT;
}

const hlChar *CGCFFile::GetAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_GCF_PACKAGE_COUNT)
	{
		return this->lpAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CGCFFile::GetAttributeInternal(HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(eAttribute)
	{
	case HL_GCF_PACKAGE_VERSION:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiMinorVersion, hlFalse);
		return hlTrue;
	case HL_GCF_PACKAGE_ID:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiCacheID, hlFalse);
		return hlTrue;
	case HL_GCF_PACKAGE_ALLOCATED_BLOCKS:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pDataBlockHeader->uiBlockCount, hlFalse);
		return hlTrue;
	case HL_GCF_PACKAGE_USED_BLOCKS:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pDataBlockHeader->uiBlocksUsed, hlFalse);
		return hlTrue;
	case HL_GCF_PACKAGE_BLOCK_LENGTH:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pDataBlockHeader->uiBlockSize, hlFalse);
		return hlTrue;
	case HL_GCF_PACKAGE_LAST_VERSION_PLAYED:
		hlAttributeSetUnsignedInteger(&Attribute, this->lpAttributeNames[eAttribute], this->pHeader->uiLastVersionPlayed, hlFalse);
		return hlTrue;
	default:
		return hlFalse;
	}
}

hlUInt CGCFFile::GetItemAttributeCountInternal() const
{
	return HL_GCF_ITEM_COUNT;
}

const hlChar *CGCFFile::GetItemAttributeNameInternal(HLPackageAttribute eAttribute) const
{
	if(eAttribute < HL_GCF_ITEM_COUNT)
	{
		return this->lpItemAttributeNames[eAttribute];
	}

	return 0;
}

hlBool CGCFFile::GetItemAttributeInternal(const CDirectoryItem *pItem, HLPackageAttribute eAttribute, HLAttribute &Attribute) const
{
	switch(pItem->GetType())
	{
		case HL_ITEM_FILE:
		{
			const CDirectoryFile *pFile = static_cast<const CDirectoryFile *>(pItem);
			switch(eAttribute)
			{
				case HL_GCF_ITEM_ENCRYPTED:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_ENCRYPTED) != 0);
					return hlTrue;
				}
				case HL_GCF_ITEM_COPY_LOCAL:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_COPY_LOCAL) != 0);
					return hlTrue;
				}
				case HL_GCF_ITEM_OVERWRITE_LOCAL:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_COPY_LOCAL_NO_OVERWRITE) == 0);
					return hlTrue;
				}
				case HL_GCF_ITEM_BACKUP_LOCAL:
				{
					hlAttributeSetBoolean(&Attribute, this->lpItemAttributeNames[eAttribute], (this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_BACKUP_LOCAL) != 0);
					return hlTrue;
				}
				case HL_GCF_ITEM_FLAGS:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags, hlTrue);
					return hlTrue;
				}
				case HL_GCF_ITEM_FRAGMENTATION:
				{
					hlUInt uiBlocksFragmented = 0;
					hlUInt uiBlocksUsed = 0;

					this->GetItemFragmentation(pFile->GetID(), uiBlocksFragmented, uiBlocksUsed);

					if(uiBlocksUsed == 0)
					{
						hlAttributeSetFloat(&Attribute, this->lpItemAttributeNames[eAttribute], 0.0f);
					}
					else
					{
						hlAttributeSetFloat(&Attribute, this->lpItemAttributeNames[eAttribute], ((hlFloat)uiBlocksFragmented / (hlFloat)uiBlocksUsed) * 100.0f);
					}
					return hlTrue;
				}
			}
			break;
		}
		case HL_ITEM_FOLDER:
		{
			const CDirectoryFolder *pFolder = static_cast<const CDirectoryFolder *>(pItem);
			switch(eAttribute)
			{
				case HL_GCF_ITEM_FLAGS:
				{
					hlAttributeSetUnsignedInteger(&Attribute, this->lpItemAttributeNames[eAttribute], this->lpDirectoryEntries[pFolder->GetID()].uiDirectoryFlags, hlTrue);
					return hlTrue;
				}
				case HL_GCF_ITEM_FRAGMENTATION:
				{
					hlUInt uiBlocksFragmented = 0;
					hlUInt uiBlocksUsed = 0;

					this->GetItemFragmentation(pFolder->GetID(), uiBlocksFragmented, uiBlocksUsed);

					if(uiBlocksUsed == 0)
					{
						hlAttributeSetFloat(&Attribute, this->lpItemAttributeNames[eAttribute], 0.0f);
					}
					else
					{
						hlAttributeSetFloat(&Attribute, this->lpItemAttributeNames[eAttribute], ((hlFloat)uiBlocksFragmented / (hlFloat)uiBlocksUsed) * 100.0f);
					}
					return hlTrue;
				}
			}
			break;
		}
	}

	return hlFalse;
}

hlBool CGCFFile::GetFileExtractableInternal(const CDirectoryFile *pFile, hlBool &bExtractable) const
{
	if(this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_ENCRYPTED)
	{
		bExtractable = hlFalse;
	}
	else
	{
		// Do we have enough data to extract?
		hlUInt uiSize = 0;

		// Get the first data block.
		hlUInt uiBlockEntryIndex = this->lpDirectoryMapEntries[pFile->GetID()].uiFirstBlockIndex;

		// Loop through each data block.
		while(uiBlockEntryIndex != this->pDataBlockHeader->uiBlockCount)
		{
			uiSize += this->lpBlockEntries[uiBlockEntryIndex].uiFileDataSize;

			// Get the next data block.
			uiBlockEntryIndex = this->lpBlockEntries[uiBlockEntryIndex].uiNextBlockEntryIndex;
		}

		bExtractable = uiSize >= this->lpDirectoryEntries[pFile->GetID()].uiItemSize;
	}

	return hlTrue;
}

hlBool CGCFFile::GetFileValidationInternal(const CDirectoryFile *pFile, HLValidation &eValidation) const
{
	// Do we have enough data to validate?
	{
		hlUInt uiSize = 0;

		// Get the first data block.
		hlUInt uiBlockEntryIndex = this->lpDirectoryMapEntries[pFile->GetID()].uiFirstBlockIndex;

		// Loop through each data block.
		while(uiBlockEntryIndex != this->pDataBlockHeader->uiBlockCount)
		{
			uiSize += this->lpBlockEntries[uiBlockEntryIndex].uiFileDataSize;

			// Get the next data block.
			uiBlockEntryIndex = this->lpBlockEntries[uiBlockEntryIndex].uiNextBlockEntryIndex;
		}

		if(uiSize != this->lpDirectoryEntries[pFile->GetID()].uiItemSize)
		{
			// File is incomplete.
			eValidation = HL_VALIDATES_INCOMPLETE;
			return hlTrue;
		}
	}

	if((this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_ENCRYPTED) != 0)
	{
		// No way of checking, assume it's ok.
		eValidation = HL_VALIDATES_ASSUMED_OK;
		return hlTrue;
	}

	// File has no checksum.
	if(this->lpDirectoryEntries[pFile->GetID()].uiChecksumIndex == 0xffffffff)
	{
		eValidation = HL_VALIDATES_ASSUMED_OK;
		return hlTrue;
	}

	Streams::IStream *pStream = 0;
	if(this->CreateStreamInternal(pFile, pStream))
	{
		if(pStream->Open(HL_MODE_READ))
		{
			eValidation = HL_VALIDATES_OK;

			hlULongLong uiTotalBytes = 0, uiFileBytes = pStream->GetStreamSize();
			hlUInt uiBufferSize;
			hlByte lpBuffer[HL_GCF_CHECKSUM_LENGTH];

			const GCFChecksumMapEntry *pChecksumMapEntry = this->lpChecksumMapEntries + this->lpDirectoryEntries[pFile->GetID()].uiChecksumIndex;

			hlBool bCancel = hlFalse;
			hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);

			hlUInt i = 0;
			while((uiBufferSize = pStream->Read(lpBuffer, HL_GCF_CHECKSUM_LENGTH)) != 0)
			{
				if(bCancel)
				{
					// User canceled.
					eValidation = HL_VALIDATES_CANCELED;
					break;
				}

				if(i >= pChecksumMapEntry->uiChecksumCount)
				{
					// Something bad happened.
					eValidation = HL_VALIDATES_ERROR;
					break;
				}

				hlULong uiChecksum = Adler32(lpBuffer, uiBufferSize) ^ CRC32(lpBuffer, uiBufferSize);
				if(uiChecksum != this->lpChecksumEntries[pChecksumMapEntry->uiFirstChecksumIndex + i].uiChecksum)
				{
					eValidation = HL_VALIDATES_CORRUPT;
					break;
				}

				uiTotalBytes += static_cast<hlULongLong>(uiBufferSize);

				hlValidateFileProgress(const_cast<CDirectoryFile *>(pFile), uiTotalBytes, uiFileBytes, &bCancel);

				i++;
			}

			pStream->Close();
		}
		else
		{
			eValidation = HL_VALIDATES_ERROR;
		}

		this->ReleaseStreamInternal(*pStream);
		delete pStream;
	}
	else
	{
		eValidation = HL_VALIDATES_ERROR;
	}

	return hlTrue;
}

hlBool CGCFFile::GetFileSizeInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	uiSize = this->lpDirectoryEntries[pFile->GetID()].uiItemSize;

	return hlTrue;
}

hlBool CGCFFile::GetFileSizeOnDiskInternal(const CDirectoryFile *pFile, hlUInt &uiSize) const
{
	// Get the first data block.
	hlUInt uiBlockEntryIndex = this->lpDirectoryMapEntries[pFile->GetID()].uiFirstBlockIndex;

	// Loop through each data block.
	while(uiBlockEntryIndex != this->pDataBlockHeader->uiBlockCount)
	{
		uiSize += ((this->lpBlockEntries[uiBlockEntryIndex].uiFileDataSize + this->pDataBlockHeader->uiBlockSize - 1) / this->pDataBlockHeader->uiBlockSize) * this->pDataBlockHeader->uiBlockSize;

		// Get the next data block.
		uiBlockEntryIndex = this->lpBlockEntries[uiBlockEntryIndex].uiNextBlockEntryIndex;
	}

	return hlTrue;
}

hlBool CGCFFile::CreateStreamInternal(const CDirectoryFile *pFile, Streams::IStream *&pStream) const
{
	if(!bReadEncrypted && this->lpDirectoryEntries[pFile->GetID()].uiDirectoryFlags & HL_GCF_FLAG_ENCRYPTED)
	{
		LastError.SetErrorMessage("File is encrypted.");
		return hlFalse;
	}

	pStream = new Streams::CGCFStream(*this, pFile->GetID());

	return hlTrue;
}

hlVoid CGCFFile::GetItemFragmentation(hlUInt uiDirectoryItemIndex, hlUInt &uiBlocksFragmented, hlUInt &uiBlocksUsed) const
{
	if((this->lpDirectoryEntries[uiDirectoryItemIndex].uiDirectoryFlags & HL_GCF_FLAG_FILE) == 0)
	{
		uiDirectoryItemIndex = this->lpDirectoryEntries[uiDirectoryItemIndex].uiFirstIndex;

		while(uiDirectoryItemIndex && uiDirectoryItemIndex != 0xffffffff)
		{
			this->GetItemFragmentation(uiDirectoryItemIndex, uiBlocksFragmented, uiBlocksUsed);

			uiDirectoryItemIndex = this->lpDirectoryEntries[uiDirectoryItemIndex].uiNextIndex;
		}
	}
	else
	{
		hlUInt uiDataBlockTerminator = this->pFragmentationMapHeader->uiTerminator == 0 ? 0x0000ffff : 0xffffffff;

		hlUInt uiLastDataBlockIndex = this->pDataBlockHeader->uiBlockCount;
		hlUInt uiBlockEntryIndex = this->lpDirectoryMapEntries[uiDirectoryItemIndex].uiFirstBlockIndex;

		while(uiBlockEntryIndex != this->pDataBlockHeader->uiBlockCount)
		{
			hlUInt uiBlockEntrySize = 0;
			hlUInt uiDataBlockIndex = this->lpBlockEntries[uiBlockEntryIndex].uiFirstDataBlockIndex;

			while(uiDataBlockIndex < uiDataBlockTerminator && uiBlockEntrySize < this->lpBlockEntries[uiBlockEntryIndex].uiFileDataSize)
			{
				if(uiLastDataBlockIndex != this->pDataBlockHeader->uiBlockCount && uiLastDataBlockIndex + 1 != uiDataBlockIndex)
				{
					uiBlocksFragmented++;
				}
				uiBlocksUsed++;
				uiLastDataBlockIndex = uiDataBlockIndex;

				uiDataBlockIndex = this->lpFragmentationMap[uiDataBlockIndex].uiNextDataBlockIndex;

				uiBlockEntrySize += this->pDataBlockHeader->uiBlockSize;
			}

			uiBlockEntryIndex = this->lpBlockEntries[uiBlockEntryIndex].uiNextBlockEntryIndex;
		}
	}
}
