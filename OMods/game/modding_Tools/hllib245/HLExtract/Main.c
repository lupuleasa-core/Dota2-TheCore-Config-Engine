/*
 * HLExtract
 * Copyright (C) 2006-2010 Ryan Gregg

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifdef _WIN32
#	include "..\lib\HLLib.h"
#	ifdef _MSC_VER
#		ifdef _DEBUG
#			ifdef _WIN64
#				pragma comment(lib, "../HLLib/x64/Debug/HLLib.lib")
#			else
#				pragma comment(lib, "../HLLib/Win32/Debug/HLLib.lib")
#			endif
#		else
#			ifdef _WIN64
#				pragma comment(lib, "../HLLib/x64/Release/HLLib.lib")
#			else
#				pragma comment(lib, "../HLLib/Win32/Release/HLLib.lib")
#			endif
#		endif
#	endif
#else
#	include <hl.h>
#endif

#if _MSC_VER
#	define _CRT_SECURE_NO_WARNINGS
#	define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <assert.h>
#include <ctype.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#else
#	include <linux/limits.h>
#	define MAX_PATH PATH_MAX

#	define FOREGROUND_BLUE      0x0001
#	define FOREGROUND_GREEN     0x0002
#	define FOREGROUND_RED       0x0004
#	define FOREGROUND_INTENSITY 0x0008

#	define stricmp strcasecmp
#	define strnicmp strncasecmp
#endif

hlUInt16 GetColor();
hlVoid SetColor(hlUInt16 uiColor);
hlVoid Print(hlUInt16 uiColor, const hlChar *lpFormat, ...);
hlVoid PrintUsage();
hlVoid List(FILE *pFile, HLDirectoryItem *pItem, hlBool bListFolders, hlBool bListFiles);
hlVoid ProgressStart();
hlVoid ProgressUpdate(hlULongLong uiBytesDone, hlULongLong uiBytesTotal);
hlVoid ExtractItemStartCallback(HLDirectoryItem *pItem);
hlVoid FileProgressCallback(HLDirectoryItem *pFile, hlUInt uiBytesExtracted, hlUInt uiBytesTotal, hlBool *pCancel);
hlVoid ExtractItemEndCallback(HLDirectoryItem *pItem, hlBool bSuccess);
hlVoid DefragmentProgressCallback(HLDirectoryItem *pFile, hlUInt uiFilesDefragmented, hlUInt uiFilesTotal, hlULongLong uiBytesDefragmented, hlULongLong uiBytesTotal, hlBool *pCancel);
HLValidation Validate(HLDirectoryItem *pItem);
hlVoid PrintAttribute(hlChar *lpPrefix, HLAttribute *pAttribute, hlChar *lpPostfix);
hlVoid PrintValidation(HLValidation eValidation);
hlVoid EnterConsole(hlUInt uiPackage, hlUInt uiConsoleCommands, hlChar *lpConsoleCommands[]);

#define MAX_ITEMS 1024
#define BUFFER_SIZE 1024

static hlChar lpDestination[MAX_PATH] = "";
static hlBool bSilent = hlFalse;
#ifndef _WIN32
	static hlUInt uiProgressLast = 0;
#endif

int main(hlInt argc, hlChar* argv[])
{
	hlUInt i;

	// Arguments.
	hlUInt uiArgumentCount = (hlUInt)argc;
	hlChar *lpPackage = 0;
	hlUInt uiExtractItems = 0;
	hlChar *lpExtractItems[MAX_ITEMS];
	hlUInt uiValidateItems = 0;
	hlChar *lpValidateItems[MAX_ITEMS];
	hlChar *lpList = 0;
	hlBool bDefragment = hlFalse;
	hlChar *lpNCFRootPath = 0;

	hlBool bList = hlFalse;
	hlBool bListFolders = hlFalse;
	hlBool bListFiles = hlFalse;
	FILE *pFile = 0;

	hlBool bConsoleMode = hlFalse;
	hlUInt uiConsoleCommands = 0;
	hlChar *lpConsoleCommands[MAX_ITEMS];
	hlBool bFileMapping = hlFalse;
	hlBool bQuickFileMapping = hlFalse;
	hlBool bVolatileAccess = hlFalse;
	hlBool bOverwriteFiles = hlTrue;
	hlBool bForceDefragment = hlFalse;

	// Package stuff.
	HLPackageType ePackageType = HL_PACKAGE_NONE;
	hlUInt uiPackage = HL_ID_INVALID, uiMode = HL_MODE_INVALID;
	HLDirectoryItem *pItem = 0;

	if(hlGetUnsignedInteger(HL_VERSION) < HL_VERSION_NUMBER)
	{
		printf("Wrong HLLib version: v%s.\n", hlGetString(HL_VERSION));
		return 1;
	}

	// Process switches.
	if(uiArgumentCount == 2)
	{
		// The user just specified a file, drop into console mode.
		lpPackage = argv[1];
		bConsoleMode = hlTrue;
		bVolatileAccess = hlTrue;
	}
	else
	{
		for(i = 1; i < uiArgumentCount; i++)
		{
			if(stricmp(argv[i], "-p") == 0 || stricmp(argv[i], "--package") == 0)
			{
				if(lpPackage == 0 && i + 1 < uiArgumentCount)
				{
					lpPackage = argv[++i];
				}
				else
				{
					PrintUsage();
					return 2;
				}
			}
			else if(stricmp(argv[i], "-d") == 0 || stricmp(argv[i], "--dest") == 0)
			{
				if(*lpDestination == 0 && i + 1 < uiArgumentCount)
				{
					strcpy(lpDestination, argv[++i]);
				}
				else
				{
					PrintUsage();
					return 2;
				}
			}
			else if(stricmp(argv[i], "-e") == 0 || stricmp(argv[i], "--extract") == 0)
			{
				if(i + 1 < uiArgumentCount)
				{
					if(uiExtractItems == MAX_ITEMS)
					{
						Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error loading package:\nMAX_ITEMS\n");
						return 2;
					}
					lpExtractItems[uiExtractItems++] = argv[++i];
				}
				else
				{
					PrintUsage();
					return 2;
				}
			}
			else if(stricmp(argv[i], "-t") == 0 || stricmp(argv[i], "--validate") == 0)
			{
				if(i + 1 < uiArgumentCount)
				{
					if(uiValidateItems == MAX_ITEMS)
					{
						Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error loading package:\nMAX_ITEMS\n");
						return 2;
					}
					lpValidateItems[uiValidateItems++] = argv[++i];
				}
				else
				{
					PrintUsage();
					return 2;
				}
			}
			else if(strnicmp(argv[i], "-l", 2) == 0 || stricmp(argv[i], "--list") == 0)
			{
				if(bList)
				{
					PrintUsage();
					return 2;
				}

				bList = hlTrue;

				if(stricmp(argv[i], "-l") == 0 || stricmp(argv[i], "--list") == 0)
				{
					// By default list everything.
					bListFolders = hlTrue;
					bListFiles = hlTrue;
				}
				else
				{
					// List folders and files if specified.
					bListFolders = strcspn(argv[i], "dD") != strlen(argv[i]);
					bListFiles = strcspn(argv[i], "fF") != strlen(argv[i]);
				}

				// Check to see if we need to dump our list to a file.
				if(i + 1 < uiArgumentCount && *argv[i + 1] != '-')
				{
					lpList = argv[++i];
				}
			}
			else if(stricmp(argv[i], "-f") == 0 || stricmp(argv[i], "--defragment") == 0)
			{
				bDefragment = hlTrue;
			}
			else if(stricmp(argv[i], "-n") == 0 || stricmp(argv[i], "--ncfroot") == 0)
			{
				if(lpNCFRootPath == 0 && i + 1 < uiArgumentCount)
				{
					lpNCFRootPath = argv[++i];
				}
				else
				{
					PrintUsage();
					return 2;
				}
			}
			else if(stricmp(argv[i], "-s") == 0 || stricmp(argv[i], "--silent") == 0)
			{
				bSilent = hlTrue;
			}
			else if(stricmp(argv[i], "-c") == 0 || stricmp(argv[i], "--console") == 0)
			{
				bConsoleMode = hlTrue;
			}
			else if(stricmp(argv[i], "-x") == 0 || stricmp(argv[i], "--execute") == 0)
			{
				if(i + 1 < uiArgumentCount)
				{
					if(uiConsoleCommands == MAX_ITEMS)
					{
						Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error loading package:\nMAX_ITEMS\n");
						return 2;
					}
					lpConsoleCommands[uiConsoleCommands++] = argv[++i];
				}
				else
				{
					PrintUsage();
					return 2;
				}
			}
			else if(stricmp(argv[i], "-m") == 0 || stricmp(argv[i], "--filemapping") == 0)
			{
				bFileMapping = hlTrue;
			}
			else if(stricmp(argv[i], "-q") == 0 || stricmp(argv[i], "--quick-filemapping") == 0)
			{
				bFileMapping = hlTrue;
				bQuickFileMapping = hlTrue;
			}
			else if(stricmp(argv[i], "-v") == 0 || stricmp(argv[i], "--volatile") == 0)
			{
				bVolatileAccess = hlTrue;
			}
			else if(stricmp(argv[i], "-o") == 0 || stricmp(argv[i], "--overwrite") == 0)
			{
				bOverwriteFiles = hlFalse;
			}
			else if(stricmp(argv[i], "-r") == 0 || stricmp(argv[i], "--force-defragment") == 0)
			{
				bDefragment = hlTrue;
				bForceDefragment = hlTrue;
			}
			else
			{
				PrintUsage();
				return 2;
			}
		}
	}

	// Make sure we have something to do.
	if(lpPackage == 0 || (uiExtractItems == 0 && uiValidateItems == 0 && !bList && !bDefragment && !bConsoleMode))
	{
		PrintUsage();
		return 2;
	}

	// If the destination directory is not specified, make it the input directory.
	if(*lpDestination == 0)
	{
		const hlChar *pForward = strrchr(lpPackage, '\\');
		const hlChar *pBackward = strrchr(lpPackage, '/');
		const hlChar *pEnd = pForward > pBackward ? pForward : pBackward;
		if(pEnd != 0)
		{
			strncpy(lpDestination, lpPackage, pEnd - lpPackage);
			lpDestination[pEnd - lpPackage] = '\0';
		}
	}

	hlInitialize();

	hlSetBoolean(HL_OVERWRITE_FILES, bOverwriteFiles);
	hlSetBoolean(HL_FORCE_DEFRAGMENT, bForceDefragment);
	hlSetVoid(HL_PROC_EXTRACT_ITEM_START, ExtractItemStartCallback);
	hlSetVoid(HL_PROC_EXTRACT_ITEM_END, ExtractItemEndCallback);
	hlSetVoid(HL_PROC_EXTRACT_FILE_PROGRESS, FileProgressCallback);
	hlSetVoid(HL_PROC_VALIDATE_FILE_PROGRESS, FileProgressCallback);
	hlSetVoid(HL_PROC_DEFRAGMENT_PROGRESS_EX, DefragmentProgressCallback);

	// Get the package type from the filename extension.
	ePackageType = hlGetPackageTypeFromName(lpPackage);

	// If the above fails, try getting the package type from the data at the start of the file.
	if(ePackageType == HL_PACKAGE_NONE)
	{
		pFile = fopen(lpPackage, "rb");
		if(pFile != 0)
		{
			hlByte lpBuffer[HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE];

			hlUInt uiBufferSize = (hlUInt)fread(lpBuffer, 1, HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE, pFile);

			ePackageType = hlGetPackageTypeFromMemory(lpBuffer, uiBufferSize);

			fclose(pFile);
			pFile = 0;
		}
	}

	if(ePackageType == HL_PACKAGE_NONE)
	{
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error loading %s:\nUnsupported package type.\n", lpPackage);

		hlShutdown();
		return 3;
	}

	// Create a package element, the element is allocated by the library and cleaned
	// up by the library.  An ID is generated which must be bound to apply operations
	// to the package.
	if(!hlCreatePackage(ePackageType, &uiPackage))
	{
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error loading %s:\n%s\n", lpPackage, hlGetString(HL_ERROR_SHORT_FORMATED));

		hlShutdown();
		return 3;
	}

	hlBindPackage(uiPackage);

	uiMode = HL_MODE_READ | (bDefragment ? HL_MODE_WRITE : 0);
	uiMode |= !bFileMapping ? HL_MODE_NO_FILEMAPPING : 0;
	uiMode |= bQuickFileMapping ? HL_MODE_QUICK_FILEMAPPING : 0;
	uiMode |= bVolatileAccess ? HL_MODE_VOLATILE : 0;

	// Open the package.
	// Of the above modes, only HL_MODE_READ is required.  HL_MODE_WRITE is present
	// only for future use.  File mapping is recommended as an efficient way to load
	// packages.  Quick file mapping maps the entire file (instead of bits as they are
	// needed) and thus should only be used in Windows 2000 and up (older versions of
	// Windows have poor virtual memory management which means large files won't be able
	// to find a continues block and will fail to load).  Volatile access allows HLLib
	// to share files with other applications that have those file open for writing.
	// This is useful for, say, loading .gcf files while Steam is running.
	if(!hlPackageOpenFile(lpPackage, uiMode))
	{
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error loading %s:\n%s\n", lpPackage, hlGetString(HL_ERROR_SHORT_FORMATED));

		hlShutdown();
		return 3;
	}

	// If we have a .ncf file, the package file data is stored externally.  In order to
	// validate the file data etc., HLLib needs to know where to look.  Tell it where.
	if(ePackageType == HL_PACKAGE_NCF)
	{
		hlNCFFileSetRootPath(lpNCFRootPath);
	}

	if(!bSilent)
		Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "%s opened.\n", lpPackage);

	// Extract the requested items.
	for(i = 0; i < uiExtractItems; i++)
	{
		// Find the item.
		pItem = hlFolderGetItemByPath(hlPackageGetRoot(), lpExtractItems[i], HL_FIND_ALL);

		if(pItem == 0)
		{
			printf("%s not found in package.\n", lpExtractItems[i]);
			continue;
		}

		if(!bSilent)
		{
			Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Extracting %s...\n", hlItemGetName(pItem));
			printf("\n");
		}

		// Extract the item.
		// Item is extracted to cDestination\Item->GetName().
		hlItemExtract(pItem, lpDestination);

		if(!bSilent)
		{
			printf("\n");
			printf("Done.\n");
		}
	}

	// Validate the requested items.
	for(i = 0; i < uiValidateItems; i++)
	{
		// Find the item.
		pItem = hlFolderGetItemByPath(hlPackageGetRoot(), lpValidateItems[i], HL_FIND_ALL);

		if(pItem == 0)
		{
			printf("%s not found in package.\n", lpValidateItems[i]);
			continue;
		}

		if(!bSilent)
		{
			Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Validating %s...\n", hlItemGetName(pItem));
			printf("\n");
		}

		// Validate the item.
		Validate(pItem);

		if(!bSilent)
		{
			printf("\n");
			printf("Done.\n");
		}
	}

	// List items in package.
	if(bList)
	{
		if(!bSilent)
		{
			printf("Listing...\n");
			printf("\n");
		}

		pFile = stdout;

		if(lpList != 0)
		{
			pFile = fopen(lpList, "wt");

			if(pFile == 0)
			{
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error opening %s:\n%s\n", lpList, "fopen() failed.");
				pFile = stdout;
			}
		}

		List(pFile, hlPackageGetRoot(), bListFolders, bListFiles);

		if(lpList != 0)
		{
			fclose(pFile);
			pFile = 0;
		}

		if(!bSilent)
		{
			printf("\n");
			printf("Done.\n");
		}
	}

	if(bDefragment)
	{
		if(!bSilent)
		{
			printf("Defragmenting...\n");
			printf("\n");

			ProgressStart();
			printf("  Progress: ");
		}

		if(!hlPackageDefragment())
		{
			Print(FOREGROUND_RED | FOREGROUND_INTENSITY, " %s", hlGetString(HL_ERROR_SHORT_FORMATED));
		}

		if(!bSilent)
		{
			printf("\n");

			printf("\n");
			printf("Done.\n");
		}
	}

	// Interactive console mode.
	// Commands: dir, cd, root, info, extract, find, type, cls, help, exit.
	if(bConsoleMode)
	{
		EnterConsole(uiPackage, uiConsoleCommands, lpConsoleCommands);
	}

	// Close the package.
	hlPackageClose();

	if(!bSilent)
		Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "%s closed.\n", lpPackage);

	// Free up the allocated memory.
	hlDeletePackage(uiPackage);

	hlShutdown();

	return 0;
}

hlUInt16 GetColor()
{
#ifdef _WIN32
    HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Handle != INVALID_HANDLE_VALUE)
	{
		CONSOLE_SCREEN_BUFFER_INFO Info;
		if(GetConsoleScreenBufferInfo(Handle, &Info))
		{
			return Info.wAttributes;
		}
    }
#endif

	return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
}

hlVoid SetColor(hlUInt16 uiColor)
{
#ifdef _WIN32
    HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Handle == INVALID_HANDLE_VALUE)
	{
        return;
    }

	SetConsoleTextAttribute(Handle, uiColor);
#endif
}

hlVoid Print(hlUInt16 uiColor, const hlChar *lpFormat, ...)
{
	hlUInt16 uiDefaultColor;
	va_list List;
	
	uiDefaultColor = GetColor();
	if(uiDefaultColor != uiColor)
	{
		SetColor(uiColor);
	}

	va_start(List, lpFormat);

	vprintf(lpFormat, List);

	va_end(List);

	if(uiDefaultColor != uiColor)
	{
		SetColor(uiDefaultColor);
	}
}

hlVoid PrintUsage()
{
	printf("HLExtract using HLLib v%s\n", hlGetString(HL_VERSION));
	printf("\n");
	printf("Correct HLExtract usage:\n");
	printf(" -p <filepath>       (Package to load.)\n");
	printf(" -d <path>           (Destination extraction directory.)\n");
	printf(" -e <itempath>       (Item in package to extract.)\n");
	printf(" -t <itempath>       (Item in package to validate.)\n");
	printf(" -l[d][f] [filepath] (List the contents of the package.)\n");
	printf(" -f                  (Defragment package.)\n");
	printf(" -c                  (Console mode.)\n");
	printf(" -x <command>        (Execute console command.)\n");
	printf(" -s                  (Silent mode.)\n");
	printf(" -m                  (Use file mapping.)\n");
	printf(" -q                  (Use quick file mapping.)\n");
	printf(" -v                  (Allow volatile access.)\n");
	printf(" -o                  (Don't overwrite files.)\n");
	printf(" -r                  (Force defragmenting on all files.)\n");
	printf(" -n <path>           (NCF file's root path.)\n");
	printf("\n");
	printf("Example HLExtract usage:\n");
#ifdef _WIN32
	printf("HLExtract.exe -p \"C:\\half-life.gcf\" -d \"C:\\backup\" -e \"root\\valve\\models\" -e \"root\\valve\\config.cfg\"\n");
	printf("HLExtract.exe -p \"C:\\half-life.gcf\" -c -m -v\n");
	printf("HLExtract.exe -p \"C:\\half-life.gcf\" -lf \"C:\\half-life.txt\" -m -s\n");
	printf("HLExtract.exe -p \"C:\\half-life.gcf\" -m -f\n");
#else
	printf("HLExtract.exe -p \"~/half-life.gcf\" -d \"~/backup\" -e \"root/valve/models\" -e \"root/valve/config.cfg\"\n");
	printf("HLExtract.exe -p \"~/half-life.gcf\" -c -m -v\n");
	printf("HLExtract.exe -p \"~/half-life.gcf\" -lf \"~/half-life.txt\" -m -s\n");
	printf("HLExtract.exe -p \"~/half-life.gcf\" -m -f\n");
#endif
}

hlVoid List(FILE *pFile, HLDirectoryItem *pItem, hlBool bListFolders, hlBool bListFiles)
{
	hlUInt i, uiItemCount;
	hlChar lpPath[512] = "";

	switch(hlItemGetType(pItem))
	{
	case HL_ITEM_FOLDER:
		if(bListFolders)
		{
			hlItemGetPath(pItem, lpPath, sizeof(lpPath));
			fprintf(pFile, "%s\n", lpPath);
		}

		uiItemCount = hlFolderGetCount(pItem);
		for(i = 0; i < uiItemCount; i++)
		{
			List(pFile, hlFolderGetItem(pItem, i), bListFolders, bListFiles);
		}
		break;
	case HL_ITEM_FILE:
		if(bListFiles)
		{
			hlItemGetPath(pItem, lpPath, sizeof(lpPath));
			fprintf(pFile, "%s\n", lpPath);
		}
		break;
	}
}

hlVoid ProgressStart()
{
#ifndef _WIN32
	uiProgressLast = 0;
	printf("0%%");
#endif
}

hlVoid ProgressUpdate(hlULongLong uiBytesDone, hlULongLong uiBytesTotal)
{
	if(!bSilent)
	{
#ifdef _WIN32
		HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (Handle != INVALID_HANDLE_VALUE)
		{
			CONSOLE_SCREEN_BUFFER_INFO Info;
			if(GetConsoleScreenBufferInfo(Handle, &Info))
			{
				if(uiBytesTotal == 0)
				{
					printf("100.0%%");
				}
				else
				{
					printf("%0.0f%%", (hlSingle)((hlDouble)uiBytesDone / (hlDouble)uiBytesTotal * 100.0));
				}
				SetConsoleCursorPosition(Handle, Info.dwCursorPosition);
			}
		}
#else
		hlUInt uiProgress = uiBytesTotal == 0 ? 100 : (hlUInt)((hlUInt64)uiBytesDone * 100 / (hlUInt64)uiBytesTotal);
		while(uiProgress >= uiProgressLast + 10)
		{
			uiProgressLast += 10;
			if(uiProgressLast == 100)
			{
				printf("100%% ");
			}
			else if(uiProgressLast == 50)
			{
				printf("50%%");
			}
			else
			{
				printf(".");
			}
		}
#endif
	}
}

hlVoid ExtractItemStartCallback(HLDirectoryItem *pItem)
{
	if(!bSilent)
	{
		if(hlItemGetType(pItem) == HL_ITEM_FILE)
		{
			printf("  Extracting %s: ", hlItemGetName(pItem));
			ProgressStart();
		}
		else
		{
			Print(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "  Extracting %s:\n", hlItemGetName(pItem));
		}
	}
}

hlVoid FileProgressCallback(HLDirectoryItem *pFile, hlUInt uiBytesExtracted, hlUInt uiBytesTotal, hlBool *pCancel)
{
	ProgressUpdate((hlULongLong)uiBytesExtracted, (hlULongLong)uiBytesTotal);
}

hlVoid ExtractItemEndCallback(HLDirectoryItem *pItem, hlBool bSuccess)
{
	hlUInt uiSize = 0;
	hlChar lpPath[512] = "";

	if(bSuccess)
	{
		if(!bSilent)
		{
			hlItemGetSize(pItem, &uiSize);
			if(hlItemGetType(pItem) == HL_ITEM_FILE)
			{
				Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "OK");
				printf(" (%u B)\n", uiSize);
			}
			else
			{
				Print(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "  Done %s: ", hlItemGetName(pItem));
				Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "OK");
				Print(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, " (%u B)\n", uiSize);
			}
		}
	}
	else
	{
		if(!bSilent)
		{
			if(hlItemGetType(pItem) == HL_ITEM_FILE)
			{
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Errored\n");
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "    %s\n", hlGetString(HL_ERROR_SHORT_FORMATED));
			}
			else
			{
				Print(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "  Done %s: ", hlItemGetName(pItem));
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Errored\n");
			}
		}
		else
		{
			hlItemGetPath(pItem, lpPath, sizeof(lpPath));
			if(hlItemGetType(pItem) == HL_ITEM_FILE)
			{
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "  Error extracting %s:\n", lpPath);
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "    %s\n", hlGetString(HL_ERROR_SHORT_FORMATED));
			}
			else
			{
				Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "  Error extracting %s.\n", lpPath);
			}
		}
	}
}

hlVoid DefragmentProgressCallback(HLDirectoryItem *pFile, hlUInt uiFilesDefragmented, hlUInt uiFilesTotal, hlULongLong uiBytesDefragmented, hlULongLong uiBytesTotal, hlBool *pCancel)
{
	ProgressUpdate(uiBytesDefragmented, uiBytesTotal);
}

HLValidation Validate(HLDirectoryItem *pItem)
{
	hlUInt i, uiItemCount;
	hlChar lpPath[512] = "";
	HLValidation eValidation = HL_VALIDATES_OK, eTest;

	switch(hlItemGetType(pItem))
	{
	case HL_ITEM_FOLDER:
		if(!bSilent)
		{
			Print(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "  Validating %s:\n", hlItemGetName(pItem));
		}

		uiItemCount = hlFolderGetCount(pItem);
		for(i = 0; i < uiItemCount; i++)
		{
			eTest = Validate(hlFolderGetItem(pItem, i));
			if(eTest > eValidation)
			{
				eValidation = eTest;
			}
		}

		if(!bSilent)
		{
			Print(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "  Done %s: ", hlItemGetName(pItem));
			PrintValidation(eValidation);
			printf("\n");
		}
		break;
	case HL_ITEM_FILE:
		if(!bSilent)
		{
			printf("  Validating %s: ", hlItemGetName(pItem));
			ProgressStart();
		}

		eValidation = hlFileGetValidation(pItem);

		if(bSilent)
		{
			switch(eValidation)
			{
			case HL_VALIDATES_INCOMPLETE:
			case HL_VALIDATES_CORRUPT:
				hlItemGetPath(pItem, lpPath, sizeof(lpPath));
				printf("  Validating %s: ", lpPath);
				PrintValidation(eValidation);
				printf("\n");
				break;
			}
		}
		else
		{
			PrintValidation(eValidation);
			printf("  \n");
		}
		break;
	}

	return eValidation;
}

hlVoid PrintAttribute(hlChar *lpPrefix, HLAttribute *pAttribute, hlChar *lpPostfix)
{
	switch(pAttribute->eAttributeType)
	{
	case HL_ATTRIBUTE_BOOLEAN:
		printf("%s%s: %s%s\n", lpPrefix, pAttribute->lpName, pAttribute->Value.Boolean.bValue ? "True" : "False", lpPostfix);
		break;
	case HL_ATTRIBUTE_INTEGER:
		printf("%s%s: %i%s\n", lpPrefix, pAttribute->lpName, pAttribute->Value.Integer.iValue, lpPostfix);
		break;
	case HL_ATTRIBUTE_UNSIGNED_INTEGER:
		if(pAttribute->Value.UnsignedInteger.bHexadecimal)
		{
			printf("%s%s: %#.8x%s\n", lpPrefix, pAttribute->lpName, pAttribute->Value.UnsignedInteger.uiValue, lpPostfix);
		}
		else
		{
			printf("%s%s: %u%s\n", lpPrefix, pAttribute->lpName, pAttribute->Value.UnsignedInteger.uiValue, lpPostfix);
		}
		break;
	case HL_ATTRIBUTE_FLOAT:
		printf("%s%s: %f%s\n", lpPrefix, pAttribute->lpName, pAttribute->Value.Float.fValue, lpPostfix);
		break;
	case HL_ATTRIBUTE_STRING:
		printf("%s%s: %s%s\n", lpPrefix, pAttribute->lpName, pAttribute->Value.String.lpValue, lpPostfix);
		break;
	}
}

hlVoid PrintValidation(HLValidation eValidation)
{
	hlUInt16 uiColor = GetColor();

	switch(eValidation)
	{
	case HL_VALIDATES_ASSUMED_OK:
		Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Assumed OK");
		break;
	case HL_VALIDATES_OK:
		Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "OK");
		break;
	case HL_VALIDATES_INCOMPLETE:
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Incomplete");
		break;
	case HL_VALIDATES_CORRUPT:
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Corrupt");
		break;
	case HL_VALIDATES_CANCELED:
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Canceled");
		break;
	case HL_VALIDATES_ERROR:
		Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error");
		break;
	default:
		printf("Unknown");
		break;
	}
}

hlVoid EnterConsole(hlUInt uiPackage, hlUInt uiConsoleCommands, hlChar *lpConsoleCommands[])
{
	hlUInt i;

	hlChar lpBuffer[BUFFER_SIZE];	// Input string.
	hlChar lpCommand[BUFFER_SIZE];	// Input command  (i.e. first word in input string).
	hlChar lpArgument[BUFFER_SIZE];	// Input argument (i.e. rest of input string).
	hlChar *lpTemp;
	hlChar lpTempBuffer[BUFFER_SIZE];

	hlUInt16 uiColor;
	HLDirectoryItem *pItem = 0, *pSubItem = 0;

	hlBool bFound;
	hlUInt uiItemCount, uiFolderCount, uiFileCount;
	hlChar iChar;
	HLStream *pStream = 0;
	HLAttribute Attribute;
	HLPackageType ePackageType = HL_PACKAGE_NONE;
	hlUInt uiSubPackage = HL_ID_INVALID;

	pItem = hlPackageGetRoot();

	while(hlTrue)
	{
		uiColor = GetColor();
		SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		if(uiConsoleCommands > 0)
		{
			printf("%s>%s\n", hlItemGetName(pItem), *lpConsoleCommands);

			strncpy(lpBuffer, *lpConsoleCommands, sizeof(lpBuffer));
			lpBuffer[sizeof(lpBuffer) - 1] = '\0';

			uiConsoleCommands--;
			lpConsoleCommands++;
		}
		else
		{
			// Command prompt.
			printf("%s>", hlItemGetName(pItem));

			// Get and parse line.
			fgets(lpBuffer, sizeof(lpBuffer), stdin);
		}

		SetColor(uiColor);

		i = (hlUInt)strlen(lpBuffer);
		while(i > 0 && (lpBuffer[i - 1] == '\r' || lpBuffer[i - 1] == '\n'))
		{
			i--;
			lpBuffer[i] = '\0';
		}
		
		*lpCommand = *lpArgument = 0;

		strcpy(lpCommand, lpBuffer);
		lpTemp = strchr(lpCommand, ' ');
		if(lpTemp != 0)
		{
			strcpy(lpArgument, lpTemp + 1);
			*lpTemp = 0;
		}

		// Cycle through commands.

		//
		// Directory listing.
		// Good example of CDirectoryItem::GetType().
		//
#ifdef _WIN32
		if(stricmp(lpCommand, "dir") == 0)
#else
		if(stricmp(lpCommand, "ls") == 0)
#endif
		{
			uiItemCount = hlFolderGetCount(pItem);
			uiFolderCount = 0, uiFileCount = 0;

			*lpTempBuffer = 0;
			hlItemGetPath(pItem, lpTempBuffer, sizeof(lpTempBuffer));

			printf("Directory of %s:\n", lpTempBuffer);

			printf("\n");

			if(*lpArgument == 0)
			{
				// List all items in the current folder.
				for(i = 0; i < uiItemCount; i++)
				{
					pSubItem = hlFolderGetItem(pItem, i);
					if(hlItemGetType(pSubItem) == HL_ITEM_FOLDER)
					{
						uiFolderCount++;
						printf("  <%s>\n", hlItemGetName(pSubItem));
					}
					else if(hlItemGetType(pSubItem) == HL_ITEM_FILE)
					{
						uiFileCount++;
						printf("  %s\n", hlItemGetName(pSubItem));
					}
				}
			}
			else
			{
				pSubItem = hlFolderFindFirst(pItem, lpArgument, HL_FIND_ALL | HL_FIND_NO_RECURSE);
				while(pSubItem)
				{
					if(hlItemGetType(pSubItem) == HL_ITEM_FOLDER)
					{
						uiFolderCount++;
						printf("  <%s>\n", hlItemGetName(pSubItem));
					}
					else if(hlItemGetType(pSubItem) == HL_ITEM_FILE)
					{
						uiFileCount++;
						printf("  %s\n", hlItemGetName(pSubItem));
					}

					pSubItem = hlFolderFindNext(pItem, pSubItem, lpArgument, HL_FIND_ALL | HL_FIND_NO_RECURSE);
				}
			}

			printf("\n");

			// Could also have used hlFolderGetFolderCount() and
			// hlFolderGetFileCount().

			printf("Summary:\n");
			printf("\n");
			printf("  %u Folder%s.\n", uiFolderCount, uiFolderCount != 1 ? "s" : "");
			printf("  %u File%s.\n", uiFileCount, uiFileCount != 1 ? "s" : "");
			printf("\n");
		}
		//
		// Change directory.
		// Good example of CDirectoryFolder::GetParent() and item casting.
		//
		else if(stricmp(lpCommand, "cd") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command cd supplied.\n");
			}
			else
			{
				if(stricmp(lpArgument, ".") == 0)
				{

				}
				else if(stricmp(lpArgument, "..") == 0)
				{
					if(hlItemGetParent(pItem) != 0)
					{
						pItem = hlItemGetParent(pItem);
					}
					else
					{
						printf("Folder does not have a parent.\n");
					}
				}
				else
				{
					bFound = hlFalse;
					uiItemCount = hlFolderGetCount(pItem);
					for(i = 0; i < uiItemCount; i++)
					{
						pSubItem = hlFolderGetItem(pItem, i);
						if(hlItemGetType(pSubItem) == HL_ITEM_FOLDER && stricmp(lpArgument, hlItemGetName(pSubItem)) == 0)
						{
							bFound = hlTrue;
							pItem = pSubItem;
							break;
						}
					}

					if(!bFound)
					{
						printf("%s not found.\n", lpArgument);
					}
				}
			}
		}
		//
		// Go to the root folder.
		//
		else if(stricmp(lpCommand, "root") == 0)
		{
			pItem = hlPackageGetRoot();
		}
		//
		// Item information.
		// Good example of CPackageUtility helper functions.
		//
		else if(stricmp(lpCommand, "info") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command info supplied.\n");
			}
			else
			{
				pSubItem = hlFolderGetItemByPath(pItem, lpArgument, HL_FIND_ALL);

				if(pSubItem != 0)
				{
					*lpTempBuffer = 0;
					hlItemGetPath(pSubItem, lpTempBuffer, sizeof(lpTempBuffer));

					printf("Information for %s:\n", lpTempBuffer);
					printf("\n");

					switch(hlItemGetType(pSubItem))
					{
					case HL_ITEM_FOLDER:
						printf("  Type: Folder\n");
#ifdef _WIN32
						printf("  Size: %I64u B\n", hlFolderGetSizeEx(pSubItem, hlTrue));
						printf("  Size On Disk: %I64u B\n", hlFolderGetSizeOnDiskEx(pSubItem, hlTrue));
#else
						printf("  Size: %llu B\n", hlFolderGetSizeEx(pSubItem, hlTrue));
						printf("  Size On Disk: %llu B\n", hlFolderGetSizeOnDiskEx(pSubItem, hlTrue));
#endif
						printf("  Folders: %u\n", hlFolderGetFolderCount(pSubItem, hlTrue));
						printf("  Files: %u\n", hlFolderGetFileCount(pSubItem, hlTrue));
						break;
					case HL_ITEM_FILE:
						printf("  Type: File\n");
						printf("  Extractable: %s\n", hlFileGetExtractable(pSubItem) ? "True" : "False");
						//printf("  Validates: %s\n", hlFileGetValidates(pSubItem) ? "True" : "False");
						printf("  Size: %u B\n", hlFileGetSize(pSubItem));
						printf("  Size On Disk: %u B\n", hlFileGetSizeOnDisk(pSubItem));
						break;
					}

					uiItemCount = hlPackageGetItemAttributeCount();
					for(i = 0; i < uiItemCount; i++)
					{
						if(hlPackageGetItemAttribute(pSubItem, i, &Attribute))
						{
							PrintAttribute("  ", &Attribute, "");
						}
					}

					printf("\n");
				}
				else
				{
					printf("%s not found.\n", lpArgument);
				}
			}
		}
		//
		// Extract item.
		// Good example of CPackageUtility extract functions.
		//
		else if(stricmp(lpCommand, "extract") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command extract supplied.\n");
			}
			else
			{
				if(stricmp(lpArgument, ".") == 0)
				{
					pSubItem = pItem;
				}
				else
				{
					pSubItem = hlFolderGetItemByName(pItem, lpArgument, HL_FIND_ALL);
				}

				if(pSubItem)
				{
					// Extract the item.
					// Item is extracted to cDestination\Item->GetName().
					if(!bSilent)
					{
						printf("Extracting %s...\n", hlItemGetName(pSubItem));
						printf("\n");
					}

					hlItemExtract(pSubItem, lpDestination);

					if(!bSilent)
					{
						printf("\n");
						printf("Done.\n");
					}
				}
				else
				{
					printf("%s not found.\n", lpArgument);
				}
			}
		}
		//
		// Validate item.
		// Validates the checksums of each item.
		//
		else if(stricmp(lpCommand, "validate") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command extract supplied.\n");
			}
			else
			{
				if(stricmp(lpArgument, ".") == 0)
				{
					pSubItem = pItem;
				}
				else
				{
					pSubItem = hlFolderGetItemByName(pItem, lpArgument, HL_FIND_ALL);
				}

				if(pSubItem)
				{
					if(!bSilent)
					{
						printf("Validating %s...\n", hlItemGetName(pSubItem));
						printf("\n");
					}

					Validate(pSubItem);

					if(!bSilent)
					{
						printf("\n");
						printf("Done.\n");
					}
				}
				else
				{
					printf("%s not found.\n", lpArgument);
				}
			}
		}
		//
		// Find items.
		// Good example of recursive directory navigation (Search() function).
		//
		else if(stricmp(lpCommand, "find") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command find supplied.\n");
			}
			else
			{
				// Search for the requested items.
				if(!bSilent)
				{
					printf("Searching for %s...\n", lpArgument);
					printf("\n");
				}

				uiItemCount = 0;
				pSubItem = hlFolderFindFirst(pItem, lpArgument, HL_FIND_ALL);
				while(pSubItem)
				{
					hlItemGetPath(pSubItem, lpTempBuffer, sizeof(lpTempBuffer));

					// Print the path.
					uiItemCount++;
					Print(hlItemGetType(pSubItem) == HL_ITEM_FILE ? FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY : GetColor(), "  Found %s: %s\n", hlItemGetType(pSubItem) == HL_ITEM_FOLDER ? "folder" : "file", lpTempBuffer);

					pSubItem = hlFolderFindNext(pItem, pSubItem, lpArgument, HL_FIND_ALL);
				}

				if(!bSilent)
				{
					if(uiItemCount != 0)
					{
						printf("\n");
					}

					printf("  %u item%s found.\n", uiItemCount, uiItemCount != 1 ? "s" : "");
					printf("\n");
				}
			}
		}
		//
		// Type files.
		// Good example of reading files into memory.
		//
		else if(stricmp(lpCommand, "type") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command type supplied.\n");
			}
			else
			{
				pSubItem = hlFolderGetItemByName(pItem, lpArgument, HL_FIND_FILES);

				if(pSubItem)
				{
					*lpTempBuffer = 0;
					hlItemGetPath(pSubItem, lpTempBuffer, sizeof(lpTempBuffer));

					if(!bSilent)
					{
						printf("Type for %s:\n", lpTempBuffer);
						printf("\n");
					}

					if(hlFileCreateStream(pSubItem, &pStream))
					{
						if(hlStreamOpen(pStream, HL_MODE_READ))
						{
							uiColor = GetColor();
							SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

							while(hlStreamReadChar(pStream, &iChar))
							{
								if((iChar >= ' ' && iChar <= '~') || iChar == '\n' || iChar == '\t')
								{
									putc(iChar, stdout);
								}
							}

							SetColor(uiColor);

							hlStreamClose(pStream);
						}
						else
						{
							Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error typing %s:\n%s\n", hlItemGetName(pSubItem), hlGetString(HL_ERROR_SHORT_FORMATED));
						}

						hlFileReleaseStream(pSubItem, pStream);
						pStream = 0;
					}
					else
					{
						Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error typing %s:\n%s\n", hlItemGetName(pSubItem), hlGetString(HL_ERROR_SHORT_FORMATED));
					}

					if(!bSilent)
					{
						printf("\n");
						printf("Done.\n");
					}
				}
				else
				{
					printf("%s not found.\n", lpArgument);
				}
			}
		}
		//
		// Open item.
		// Good example of opening packages inside packages.
		//
		else if(stricmp(lpCommand, "open") == 0)
		{
			if(*lpArgument == 0)
			{
				printf("No argument for command open supplied.\n");
			}
			else
			{
				pSubItem = hlFolderGetItemByName(pItem, lpArgument, HL_FIND_FILES);

				if(pSubItem)
				{
					if(hlFileCreateStream(pSubItem, &pStream))
					{
						if(hlStreamOpen(pStream, HL_MODE_READ))
						{
							ePackageType = hlGetPackageTypeFromStream(pStream);

							if(hlCreatePackage(ePackageType, &uiSubPackage))
							{
								hlBindPackage(uiSubPackage);
								if(hlPackageOpenStream(pStream, HL_MODE_READ))
								{
									if(!bSilent)
										Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "%s opened.\n", hlItemGetName(pSubItem));

									EnterConsole(uiSubPackage, uiConsoleCommands, lpConsoleCommands);

									hlPackageClose();

									if(!bSilent)
										Print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "%s closed.\n", hlItemGetName(pSubItem));
								}
								else
								{
									Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error opening %s:\n%s\n", hlItemGetName(pSubItem), hlGetString(HL_ERROR_SHORT_FORMATED));
								}

								hlDeletePackage(uiSubPackage);

								hlBindPackage(uiPackage);
							}
							else
							{
								Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error opening %s:\n%s\n", hlItemGetName(pSubItem), hlGetString(HL_ERROR_SHORT_FORMATED));
							}

							hlStreamClose(pStream);
						}
						else
						{
							Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error opening %s:\n%s\n", hlItemGetName(pSubItem), hlGetString(HL_ERROR_SHORT_FORMATED));
						}

						hlFileReleaseStream(pSubItem, pStream);
						pStream = 0;
					}
					else
					{
						Print(FOREGROUND_RED | FOREGROUND_INTENSITY, "Error opening %s:\n%s\n", hlItemGetName(pSubItem), hlGetString(HL_ERROR_SHORT_FORMATED));
					}
				}
				else
				{
					printf("%s not found.\n", lpArgument);
				}
			}
		}
		//
		// Clear screen.
		//
		else if(stricmp(lpCommand, "status") == 0)
		{
#ifdef _WIN32
			printf("Total size: %I64u B\n", hlGetUnsignedLongLong(HL_PACKAGE_SIZE));
#else
			printf("Total size: %llu B\n", hlGetUnsignedLongLong(HL_PACKAGE_SIZE));
#endif
			printf("Total mapping allocations: %u\n", hlGetUnsignedInteger(HL_PACKAGE_TOTAL_ALLOCATIONS));
#ifdef _WIN32
			printf("Total mapping memory allocated: %I64u B\n", hlGetUnsignedLongLong(HL_PACKAGE_TOTAL_MEMORY_ALLOCATED));
			printf("Total mapping memory used: %I64u B\n", hlGetUnsignedLongLong(HL_PACKAGE_TOTAL_MEMORY_USED));
#else
			printf("Total mapping memory allocated: %llu B\n", hlGetUnsignedLongLong(HL_PACKAGE_TOTAL_MEMORY_ALLOCATED));
			printf("Total mapping memory used: %llu B\n", hlGetUnsignedLongLong(HL_PACKAGE_TOTAL_MEMORY_USED));
#endif

			uiItemCount = hlPackageGetAttributeCount();
			for(i = 0; i < uiItemCount; i++)
			{
				if(hlPackageGetAttribute(i, &Attribute))
				{
					PrintAttribute("", &Attribute, "");
				}
			}
		}
#ifdef _WIN32
		else if(stricmp(lpCommand, "cls") == 0)
		{
			system("cls");
		}
#endif
		else if(stricmp(lpCommand, "help") == 0)
		{
			printf("Valid commands:\n");
			printf("\n");
#ifdef _WIN32
			printf("dir <filter>    (Directory list.)\n");
#else
			printf("ls <filter>     (Directory list.)\n");
#endif
			printf("cd <folder>     (Change directroy.)\n");
			printf("info <item>     (Item information.)\n");
			printf("extract <item>  (Extract item.)\n");
			printf("validate <item> (Validate item.)\n");
			printf("find <filter>   (Find item.)\n");
			printf("type <file>     (Type a file.)\n");
			printf("open <file>     (Open a nested package.)\n");
			printf("root            (Go to the root folder.)\n");
			printf("status          (Package information.)\n");
#ifdef _WIN32
			printf("cls             (Clear the screen.)\n");
#endif
			printf("help            (Program help.)\n");
			printf("exit            (Quit program.)\n");
			printf("\n");
		}
		else if(stricmp(lpCommand, "exit") == 0)
		{
			break;
		}
		else
		{
			printf("Unkown command: %s\n", lpCommand);
		}
	}
}
