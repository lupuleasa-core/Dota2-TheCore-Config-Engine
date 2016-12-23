/*
 * HLExtract.Net
 * Copyright (C) 2008-2012 Ryan Gregg

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

using System;
using System.Runtime.InteropServices;

public sealed class HLLib
{
    #region Constants
    public const int HL_VERSION_NUMBER = ((2 << 24) | (4 << 16) | (5 << 8) | 0);
    public const string HL_VERSION_STRING = "2.4.5";

    public const uint HL_ID_INVALID = 0xffffffff;

    public const uint HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE = 8;
    public const uint HL_DEFAULT_VIEW_SIZE = 131072;
    public const uint HL_DEFAULT_COPY_BUFFER_SIZE = 131072;
    #endregion

    #region Enumerations
    public enum HLOption : uint
    {
        HL_VERSION = 0,
        HL_ERROR,
        HL_ERROR_SYSTEM,
        HL_ERROR_SHORT_FORMATED,
        HL_ERROR_LONG_FORMATED,
        HL_PROC_OPEN,
        HL_PROC_CLOSE,
        HL_PROC_READ,
        HL_PROC_WRITE,
        HL_PROC_SEEK,
        HL_PROC_TELL,
        HL_PROC_SIZE,
        HL_PROC_EXTRACT_ITEM_START,
        HL_PROC_EXTRACT_ITEM_END,
        HL_PROC_EXTRACT_FILE_PROGRESS,
        HL_PROC_VALIDATE_FILE_PROGRESS,
        HL_OVERWRITE_FILES,
        HL_PACKAGE_BOUND,
        HL_PACKAGE_ID,
        HL_PACKAGE_SIZE,
        HL_PACKAGE_TOTAL_ALLOCATIONS,
        HL_PACKAGE_TOTAL_MEMORY_ALLOCATED,
        HL_PACKAGE_TOTAL_MEMORY_USED,
        HL_READ_ENCRYPTED,
        HL_FORCE_DEFRAGMENT,
        HL_PROC_DEFRAGMENT_PROGRESS,
        HL_PROC_DEFRAGMENT_PROGRESS_EX,
        HL_PROC_SEEK_EX,
        HL_PROC_TELL_EX,
        HL_PROC_SIZE_EX
    }

    public enum HLFileMode : uint
	{
        HL_MODE_INVALID = 0x00,
        HL_MODE_READ = 0x01,
        HL_MODE_WRITE = 0x02,
        HL_MODE_CREATE = 0x04,
        HL_MODE_VOLATILE = 0x08,
        HL_MODE_NO_FILEMAPPING = 0x10,
        HL_MODE_QUICK_FILEMAPPING = 0x20
	}

    public enum HLSeekMode : uint
    {
        HL_SEEK_BEGINNING = 0,
        HL_SEEK_CURRENT,
        HL_SEEK_END
    }

    public enum HLDirectoryItemType : uint
    {
        HL_ITEM_NONE = 0,
        HL_ITEM_FOLDER,
        HL_ITEM_FILE
    }

    public enum HLSortOrder : uint
    {
        HL_ORDER_ASCENDING = 0,
        HL_ORDER_DESCENDING
    }

    public enum HLSortField : uint
    {
        HL_FIELD_NAME = 0,
        HL_FIELD_SIZE
    }

    public enum HLFindType : uint
    {
        HL_FIND_FILES = 0x01,
        HL_FIND_FOLDERS = 0x02,
        HL_FIND_NO_RECURSE = 0x04,
        HL_FIND_CASE_SENSITIVE = 0x08,
        HL_FIND_MODE_STRING = 0x10,
        HL_FIND_MODE_SUBSTRING = 0x20,
        HL_FIND_MODE_WILDCARD = 0x00,
        HL_FIND_ALL = HL_FIND_FILES | HL_FIND_FOLDERS
    }

    public enum HLStreamType : uint
    {
        HL_STREAM_NONE = 0,
        HL_STREAM_FILE,
        HL_STREAM_GCF,
        HL_STREAM_MAPPING,
        HL_STREAM_MEMORY,
        HL_STREAM_PROC,
        HL_STREAM_NULL
    }

    public enum HLMappingType : uint
    {
        HL_MAPPING_NONE = 0,
        HL_MAPPING_FILE,
        HL_MAPPING_MEMORY,
        HL_MAPPING_STREAM
    }

    public enum HLPackageType : uint
    {
	    HL_PACKAGE_NONE = 0,
	    HL_PACKAGE_BSP,
	    HL_PACKAGE_GCF,
	    HL_PACKAGE_PAK,
	    HL_PACKAGE_VBSP,
	    HL_PACKAGE_WAD,
	    HL_PACKAGE_XZP,
	    HL_PACKAGE_ZIP,
	    HL_PACKAGE_NCF,
        HL_PACKAGE_VPK,
        HL_PACKAGE_SGA
    }

    public enum HLAttributeType : uint
    {
        HL_ATTRIBUTE_INVALID = 0,
        HL_ATTRIBUTE_BOOLEAN,
        HL_ATTRIBUTE_INTEGER,
        HL_ATTRIBUTE_UNSIGNED_INTEGER,
        HL_ATTRIBUTE_FLOAT,
        HL_ATTRIBUTE_STRING
    }

    public enum HLPackageAttribute : uint
    {
        HL_BSP_PACKAGE_VERSION = 0,
        HL_BSP_PACKAGE_COUNT,
        HL_BSP_ITEM_WIDTH = 0,
        HL_BSP_ITEM_HEIGHT,
        HL_BSP_ITEM_PALETTE_ENTRIES,
        HL_BSP_ITEM_COUNT,

        HL_GCF_PACKAGE_VERSION = 0,
        HL_GCF_PACKAGE_ID,
        HL_GCF_PACKAGE_ALLOCATED_BLOCKS,
        HL_GCF_PACKAGE_USED_BLOCKS,
        HL_GCF_PACKAGE_BLOCK_LENGTH,
        HL_GCF_PACKAGE_LAST_VERSION_PLAYED,
        HL_GCF_PACKAGE_COUNT,
        HL_GCF_ITEM_ENCRYPTED = 0,
        HL_GCF_ITEM_COPY_LOCAL,
        HL_GCF_ITEM_OVERWRITE_LOCAL,
        HL_GCF_ITEM_BACKUP_LOCAL,
        HL_GCF_ITEM_FLAGS,
        HL_GCF_ITEM_FRAGMENTATION,
        HL_GCF_ITEM_COUNT,

        HL_NCF_PACKAGE_VERSION = 0,
        HL_NCF_PACKAGE_ID,
        HL_NCF_PACKAGE_LAST_VERSION_PLAYED,
        HL_NCF_PACKAGE_COUNT,
        HL_NCF_ITEM_ENCRYPTED = 0,
        HL_NCF_ITEM_COPY_LOCAL,
        HL_NCF_ITEM_OVERWRITE_LOCAL,
        HL_NCF_ITEM_BACKUP_LOCAL,
        HL_NCF_ITEM_FLAGS,
        HL_NCF_ITEM_COUNT,

        HL_PAK_PACKAGE_COUNT = 0,
        HL_PAK_ITEM_COUNT = 0,

        HL_SGA_PACKAGE_VERSION_MAJOR = 0,
        HL_SGA_PACKAGE_VERSION_MINOR,
        HL_SGA_PACKAGE_MD5_FILE,
        HL_SGA_PACKAGE_NAME,
        HL_SGA_PACKAGE_MD5_HEADER,
        HL_SGA_PACKAGE_COUNT,
        HL_SGA_ITEM_SECTION_ALIAS = 0,
        HL_SGA_ITEM_SECTION_NAME,
        HL_SGA_ITEM_MODIFIED,
        HL_SGA_ITEM_TYPE,
        HL_SGA_ITEM_CRC,
        HL_SGA_ITEM_VERIFICATION,
        HL_SGA_ITEM_COUNT,

        HL_VBSP_PACKAGE_VERSION = 0,
        HL_VBSP_PACKAGE_MAP_REVISION,
        HL_VBSP_PACKAGE_COUNT,
        HL_VBSP_ITEM_VERSION = 0,
        HL_VBSP_ITEM_FOUR_CC,
        HL_VBSP_ZIP_PACKAGE_DISK,
        HL_VBSP_ZIP_PACKAGE_COMMENT,
        HL_VBSP_ZIP_ITEM_CREATE_VERSION,
        HL_VBSP_ZIP_ITEM_EXTRACT_VERSION,
        HL_VBSP_ZIP_ITEM_FLAGS,
        HL_VBSP_ZIP_ITEM_COMPRESSION_METHOD,
        HL_VBSP_ZIP_ITEM_CRC,
        HL_VBSP_ZIP_ITEM_DISK,
        HL_VBSP_ZIP_ITEM_COMMENT,
        HL_VBSP_ITEM_COUNT,

	    HL_VPK_PACKAGE_COUNT = 0,
	    HL_VPK_ITEM_PRELOAD_BYTES = 0,
	    HL_VPK_ITEM_ARCHIVE,
        HL_VPK_ITEM_CRC,
	    HL_VPK_ITEM_COUNT,

        HL_WAD_PACKAGE_VERSION = 0,
        HL_WAD_PACKAGE_COUNT,
        HL_WAD_ITEM_WIDTH = 0,
        HL_WAD_ITEM_HEIGHT,
        HL_WAD_ITEM_PALETTE_ENTRIES,
        HL_WAD_ITEM_MIPMAPS,
        HL_WAD_ITEM_COMPRESSED,
        HL_WAD_ITEM_TYPE,
        HL_WAD_ITEM_COUNT,

        HL_XZP_PACKAGE_VERSION = 0,
        HL_XZP_PACKAGE_PRELOAD_BYTES,
        HL_XZP_PACKAGE_COUNT,
        HL_XZP_ITEM_CREATED = 0,
        HL_XZP_ITEM_PRELOAD_BYTES,
        HL_XZP_ITEM_COUNT,

        HL_ZIP_PACKAGE_DISK = 0,
        HL_ZIP_PACKAGE_COMMENT,
        HL_ZIP_PACKAGE_COUNT,
        HL_ZIP_ITEM_CREATE_VERSION = 0,
        HL_ZIP_ITEM_EXTRACT_VERSION,
        HL_ZIP_ITEM_FLAGS,
        HL_ZIP_ITEM_COMPRESSION_METHOD,
        HL_ZIP_ITEM_CRC,
        HL_ZIP_ITEM_DISK,
        HL_ZIP_ITEM_COMMENT,
        HL_ZIP_ITEM_COUNT
    }

    public enum HLValidation
    {
        HL_VALIDATES_OK = 0,
        HL_VALIDATES_ASSUMED_OK,
        HL_VALIDATES_INCOMPLETE,
        HL_VALIDATES_CORRUPT,
        HL_VALIDATES_CANCELED,
        HL_VALIDATES_ERROR
    }
    #endregion

    #region Structures
    [StructLayout(LayoutKind.Sequential,Pack=1,Size=512)]
    public struct HLAttribute
    {
        public HLAttributeType eAttributeType;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst=252)]
        public byte[] lpName;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst=256)]
        public byte[] lpValue;

        public string GetName()
        {
            int iLength = 0;
            while(lpName[iLength] != 0)
            {
                iLength++;
            }
            try
            {
                return System.Text.Encoding.ASCII.GetString(lpName, 0, iLength);
            }
            catch
            {
                return string.Empty;
            }
        }

        public object GetData()
        {
            switch (eAttributeType)
            {
                case HLAttributeType.HL_ATTRIBUTE_BOOLEAN:
                    return hlAttributeGetBoolean(ref this);
                case HLAttributeType.HL_ATTRIBUTE_INTEGER:
                    return hlAttributeGetInteger(ref this);
                case HLAttributeType.HL_ATTRIBUTE_UNSIGNED_INTEGER:
                    return hlAttributeGetUnsignedInteger(ref this);
                case HLAttributeType.HL_ATTRIBUTE_FLOAT:
                    return hlAttributeGetFloat(ref this);
                case HLAttributeType.HL_ATTRIBUTE_STRING:
                    return hlAttributeGetString(ref this);
                default:
                    return null;
            }
        }

        public override string ToString()
        {
            switch (eAttributeType)
            {
                case HLAttributeType.HL_ATTRIBUTE_BOOLEAN:
                    return hlAttributeGetBoolean(ref this).ToString();
                case HLAttributeType.HL_ATTRIBUTE_INTEGER:
                    return hlAttributeGetInteger(ref this).ToString("#,##0");
                case HLAttributeType.HL_ATTRIBUTE_UNSIGNED_INTEGER:
                    if (lpValue[4] == 0)
                    {
                        return hlAttributeGetUnsignedInteger(ref this).ToString("#,##0");
                    }
                    else // Display as hexadecimal.
                    {
                        return "0x" + hlAttributeGetUnsignedInteger(ref this).ToString("x8");
                    }
                case HLAttributeType.HL_ATTRIBUTE_FLOAT:
                    return hlAttributeGetFloat(ref this).ToString("#,##0.00000000");
                case HLAttributeType.HL_ATTRIBUTE_STRING:
                    return hlAttributeGetString(ref this);
                default:
                    return string.Empty;
            }
        }
    }
    #endregion

    #region Callback Functions
    //
    // Important: Callback functions cannot use IntPtr.  Instead, I use [MarshalAs(UnmanagedType.I4)]int.
    // Convert IntPtr objects using IntPtr.ToInt32().  Convert int objects to IntPtr using new IntPtr(int).
    // This obviously only works on 32 bit builds of HLLib.
    //

    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    [return:MarshalAs(UnmanagedType.U1)]
    public delegate bool HLOpenProc(uint uiMode, IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    [return:MarshalAs(UnmanagedType.U1)]
    public delegate bool HLCloseProc(IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate uint HLReadProc(IntPtr lpData, uint uiBytes, IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate uint HLWriteProc(IntPtr lpData, uint uiBytes, IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate uint HLSeekProc(long iOffset, HLSeekMode eSeekMode, IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate ulong HLSeekExProc(Int64 iOffset, HLSeekMode eSeekMode, IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate uint HLTellProc(IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate ulong HLTellExProc(IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate uint HLSizeProc(IntPtr pUserData);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate ulong HLSizeExProc(IntPtr pUserData);

    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate void HLExtractItemStartProc(IntPtr pItem);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate void HLExtractItemEndProc(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bSuccess);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate void HLExtractFileProgressProc(IntPtr pFile, uint uiBytesExtracted, uint uiBytesTotal, [MarshalAs(UnmanagedType.U1)]ref bool bCancel);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate void HLValidateFileProgressProc(IntPtr pFile, uint uiBytesValidated, uint uiBytesTotal, [MarshalAs(UnmanagedType.U1)]ref bool pCancel);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate void HLDefragmentFileProgressProc(IntPtr pFile, uint uiFilesDefragmented, uint uiFilesTotal, uint uiBytesDefragmented, uint uiBytesTotal, [MarshalAs(UnmanagedType.U1)]ref bool pCancel);
    [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
    public delegate void HLDefragmentFileProgressExProc(IntPtr pFile, uint uiFilesDefragmented, uint uiFilesTotal, UInt64 uiBytesDefragmented, UInt64 uiBytesTotal, [MarshalAs(UnmanagedType.U1)]ref bool pCancel);
    #endregion

    #region Functions
    public static bool IsWow64()
    {
        return IntPtr.Size == 8;
    }

    //
    // VTFLib
    //

    public static void hlInitialize()
    {
        if (IsWow64()) x64.hlInitialize(); else x86.hlInitialize();
    }
    public static void hlShutdown()
    {
        if (IsWow64()) x64.hlShutdown(); else x86.hlShutdown();
    }

    //
    // Get/Set
    //

    public static bool hlGetBoolean(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetBoolean(eOption); else return x86.hlGetBoolean(eOption);
    }
    public static bool hlGetBooleanValidate(HLOption eOption, out bool pValue)
    {
        if (IsWow64()) return x64.hlGetBooleanValidate(eOption, out pValue); else return x86.hlGetBooleanValidate(eOption, out pValue);
    }
    public static void hlSetBoolean(HLOption eOption, bool bValue)
    {
        if (IsWow64()) x64.hlSetBoolean(eOption, bValue); else x86.hlSetBoolean(eOption, bValue);
    }

    public static int hlGetInteger(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetInteger(eOption); else return x86.hlGetInteger(eOption);
    }
    public static bool hlGetIntegerValidate(HLOption eOption, out int pValue)
    {
        if (IsWow64()) return x64.hlGetIntegerValidate(eOption, out pValue); else return x86.hlGetIntegerValidate(eOption, out pValue);
    }
    public static void hlSetInteger(HLOption eOption, int iValue)
    {
        if (IsWow64()) x64.hlSetInteger(eOption, iValue); else x86.hlSetInteger(eOption, iValue);
    }

    public static uint hlGetUnsignedInteger(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetUnsignedInteger(eOption); else return x86.hlGetUnsignedInteger(eOption);
    }
    public static bool hlGetUnsignedIntegerValidate(HLOption eOption, out uint pValue)
    {
        if (IsWow64()) return x64.hlGetUnsignedIntegerValidate(eOption, out pValue); else return x86.hlGetUnsignedIntegerValidate(eOption, out pValue);
    }
    public static void hlSetUnsignedInteger(HLOption eOption, uint uiValue)
    {
        if (IsWow64()) x64.hlSetUnsignedInteger(eOption, uiValue); else x86.hlSetUnsignedInteger(eOption, uiValue);
    }

    public static long hlGetLong(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetLong(eOption); else return x86.hlGetLong(eOption);
    }
    public static bool hlGetLongValidate(HLOption eOption, out long pValue)
    {
        if (IsWow64()) return x64.hlGetLongValidate(eOption, out pValue); else return x86.hlGetLongValidate(eOption, out pValue);
    }
    public static void hlSetLong(HLOption eOption, long iValue)
    {
        if (IsWow64()) x64.hlSetLong(eOption, iValue); else x86.hlSetLong(eOption, iValue);
    }

    public static ulong hlGetUnsignedLong(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetUnsignedLong(eOption); else return x86.hlGetUnsignedLong(eOption);
    }
    public static bool hlGetUnsignedLongValidate(HLOption eOption, out ulong pValue)
    {
        if (IsWow64()) return x64.hlGetUnsignedLongValidate(eOption, out pValue); else return x86.hlGetUnsignedLongValidate(eOption, out pValue);
    }
    public static void hlSetUnsignedLong(HLOption eOption, ulong uiValue)
    {
        if (IsWow64()) x64.hlSetUnsignedLong(eOption, uiValue); else x86.hlSetUnsignedLong(eOption, uiValue);
    }

    public static float hlGetFloat(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetFloat(eOption); else return x86.hlGetFloat(eOption);
    }
    public static bool hlGetFloatValidate(HLOption eOption, out float pValue)
    {
        if (IsWow64()) return x64.hlGetFloatValidate(eOption, out pValue); else return x86.hlGetFloatValidate(eOption, out pValue);
    }
    public static void hlSetFloat(HLOption eOption, float pValue)
    {
        if (IsWow64()) x64.hlSetFloat(eOption, pValue); else x86.hlSetFloat(eOption, pValue);
    }

    public static string hlGetString(HLOption eOption)
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlGetString(eOption); else lpString = x86.hlGetString(eOption);
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static bool hlGetStringValidate(HLOption eOption, out string pValue)
    {
        if (IsWow64()) return x64.hlGetStringValidate(eOption, out pValue); else return x86.hlGetStringValidate(eOption, out pValue);
    }
    public static void hlSetString(HLOption eOption, string lpValue)
    {
        if (IsWow64()) x64.hlSetString(eOption, lpValue); else x86.hlSetString(eOption, lpValue);
    }

    public static IntPtr hlGetVoid(HLOption eOption)
    {
        if (IsWow64()) return x64.hlGetVoid(eOption); else return x86.hlGetVoid(eOption);
    }
    public static bool hlGetVoidValidate(HLOption eOption, out IntPtr pValue)
    {
        if (IsWow64()) return x64.hlGetVoidValidate(eOption, out pValue); else return x86.hlGetVoidValidate(eOption, out pValue);
    }
    public static void hlSetVoid(HLOption eOption, IntPtr lpValue)
    {
        if (IsWow64()) x64.hlSetVoid(eOption, lpValue); else x86.hlSetVoid(eOption, lpValue);
    }

    //
    // Attributes
    //

    public static bool hlAttributeGetBoolean(ref HLAttribute pAttribute)
    {
        if (IsWow64()) return x64.hlAttributeGetBoolean(ref pAttribute); else return x86.hlAttributeGetBoolean(ref pAttribute);
    }
    public static void hlAttributeSetBoolean(ref HLAttribute pAttribute, string lpName, bool bValue)
    {
        if (IsWow64()) x64.hlAttributeSetBoolean(ref pAttribute, lpName, bValue); else x86.hlAttributeSetBoolean(ref pAttribute, lpName, bValue);
    }

    public static int hlAttributeGetInteger(ref HLAttribute pAttribute)
    {
        if (IsWow64()) return x64.hlAttributeGetInteger(ref pAttribute); else return x86.hlAttributeGetInteger(ref pAttribute);
    }
    public static void hlAttributeSetInteger(ref HLAttribute pAttribute, string lpName, int iValue)
    {
        if (IsWow64()) x64.hlAttributeSetInteger(ref pAttribute, lpName, iValue); else x86.hlAttributeSetInteger(ref pAttribute, lpName, iValue);
    }

    public static uint hlAttributeGetUnsignedInteger(ref HLAttribute pAttribute)
    {
        if (IsWow64()) return x64.hlAttributeGetUnsignedInteger(ref pAttribute); else return x86.hlAttributeGetUnsignedInteger(ref pAttribute);
    }
    public static void hlAttributeSetUnsignedInteger(ref HLAttribute pAttribute, string lpName, uint uiValue, bool bHexadecimal)
    {
        if (IsWow64()) x64.hlAttributeSetUnsignedInteger(ref pAttribute, lpName, uiValue, bHexadecimal); else x86.hlAttributeSetUnsignedInteger(ref pAttribute, lpName, uiValue, bHexadecimal);
    }

    public static float hlAttributeGetFloat(ref HLAttribute pAttribute)
    {
        if (IsWow64()) return x64.hlAttributeGetFloat(ref pAttribute); else return x86.hlAttributeGetFloat(ref pAttribute);
    }
    public static void hlAttributeSetFloat(ref HLAttribute pAttribute, string lpName, float fValue)
    {
        if (IsWow64()) x64.hlAttributeSetFloat(ref pAttribute, lpName, fValue); else x86.hlAttributeSetFloat(ref pAttribute, lpName, fValue);
    }

    public static string hlAttributeGetString(ref HLAttribute pAttribute)
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlAttributeGetString(ref pAttribute); else lpString = x86.hlAttributeGetString(ref pAttribute);
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static void hlAttributeSetString(ref HLAttribute pAttribute, string lpName, string lpValue)
    {
        if (IsWow64()) x64.hlAttributeSetString(ref pAttribute, lpName, lpValue); else x86.hlAttributeSetString(ref pAttribute, lpName, lpValue);
    }

    //
    // Directory Item
    //

    public static HLDirectoryItemType hlItemGetType(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlItemGetType(pItem); else return x86.hlItemGetType(pItem);
    }

    public static string hlItemGetName(IntPtr pItem)
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlItemGetName(pItem); else lpString = x86.hlItemGetName(pItem);
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static uint hlItemGetID(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlItemGetID(pItem); else return x86.hlItemGetID(pItem);
    }
    public static IntPtr hlItemGetData(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlItemGetData(pItem); else return x86.hlItemGetData(pItem);
    }

    public static uint hlItemGetPackage(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlItemGetPackage(pItem); else return x86.hlItemGetPackage(pItem);
    }
    public static IntPtr hlItemGetParent(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlItemGetParent(pItem); else return x86.hlItemGetParent(pItem);
    }

    public static bool hlItemGetSize(IntPtr pItem, out uint pSize)
    {
        if (IsWow64()) return x64.hlItemGetSize(pItem, out pSize); else return x86.hlItemGetSize(pItem, out pSize);
    }
    public static bool hlItemGetSizeEx(IntPtr pItem, out UInt64 pSize)
    {
        if (IsWow64()) return x64.hlItemGetSizeEx(pItem, out pSize); else return x86.hlItemGetSizeEx(pItem, out pSize);
    }
    public static bool hlItemGetSizeOnDisk(IntPtr pItem, out uint pSize)
    {
        if (IsWow64()) return x64.hlItemGetSizeOnDisk(pItem, out pSize); else return x86.hlItemGetSizeOnDisk(pItem, out pSize);
    }
    public static bool hlItemGetSizeOnDiskEx(IntPtr pItem, out UInt64 pSize)
    {
        if (IsWow64()) return x64.hlItemGetSizeOnDiskEx(pItem, out pSize); else return x86.hlItemGetSizeOnDiskEx(pItem, out pSize);
    }

    public static bool hlItemGetPath(IntPtr pItem, IntPtr lpPath, uint uiPathSize)
    {
        if (IsWow64()) return x64.hlItemGetPath(pItem, lpPath, uiPathSize); else return x86.hlItemGetPath(pItem, lpPath, uiPathSize);
    }
    public static bool hlItemExtract(IntPtr pItem, string lpPath)
    {
        if (IsWow64()) return x64.hlItemExtract(pItem, lpPath); else return x86.hlItemExtract(pItem, lpPath);
    }

    //
    // Directory Folder
    //

    public static uint hlFolderGetCount(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlFolderGetCount(pItem); else return x86.hlFolderGetCount(pItem);
    }

    public static IntPtr hlFolderGetItem(IntPtr pItem, uint uiIndex)
    {
        if (IsWow64()) return x64.hlFolderGetItem(pItem, uiIndex); else return x86.hlFolderGetItem(pItem, uiIndex);
    }
    public static IntPtr hlFolderGetItemByName(IntPtr pItem, string lpName, HLFindType eFind)
    {
        if (IsWow64()) return x64.hlFolderGetItemByName(pItem, lpName, eFind); else return x86.hlFolderGetItemByName(pItem, lpName, eFind);
    }
    public static IntPtr hlFolderGetItemByPath(IntPtr pItem, string lpPath, HLFindType eFind)
    {
        if (IsWow64()) return x64.hlFolderGetItemByPath(pItem, lpPath, eFind); else return x86.hlFolderGetItemByPath(pItem, lpPath, eFind);
    }

    public static void hlFolderSort(IntPtr pItem, HLSortField eField, HLSortOrder eOrder, bool bRecurse)
    {
        if (IsWow64()) x64.hlFolderSort(pItem, eField, eOrder, bRecurse); else x86.hlFolderSort(pItem, eField, eOrder, bRecurse);
    }

    public static IntPtr hlFolderFindFirst(IntPtr pFolder, string lpSearch, HLFindType eFind)
    {
        if (IsWow64()) return x64.hlFolderFindFirst(pFolder, lpSearch, eFind); else return x86.hlFolderFindFirst(pFolder, lpSearch, eFind);
    }
    public static IntPtr hlFolderFindNext(IntPtr pFolder, IntPtr pItem, string lpSearch, HLFindType eFind)
    {
        if (IsWow64()) return x64.hlFolderFindNext(pFolder, pItem, lpSearch, eFind); else return x86.hlFolderFindNext(pFolder, pItem, lpSearch, eFind);
    }

    public static uint hlFolderGetSize(IntPtr pItem, bool bRecurse)
    {
        if (IsWow64()) return x64.hlFolderGetSize(pItem, bRecurse); else return x86.hlFolderGetSize(pItem, bRecurse);
    }
    public static UInt64 hlFolderGetSizeEx(IntPtr pItem, bool bRecurse)
    {
        if (IsWow64()) return x64.hlFolderGetSizeEx(pItem, bRecurse); else return x86.hlFolderGetSizeEx(pItem, bRecurse);
    }
    public static uint hlFolderGetSizeOnDisk(IntPtr pItem, bool bRecurse)
    {
        if (IsWow64()) return x64.hlFolderGetSizeOnDisk(pItem, bRecurse); else return x86.hlFolderGetSizeOnDisk(pItem, bRecurse);
    }
    public static UInt64 hlFolderGetSizeOnDiskEx(IntPtr pItem, bool bRecurse)
    {
        if (IsWow64()) return x64.hlFolderGetSizeOnDiskEx(pItem, bRecurse); else return x86.hlFolderGetSizeOnDiskEx(pItem, bRecurse);
    }
    public static uint hlFolderGetFolderCount(IntPtr pItem, bool bRecurse)
    {
        if (IsWow64()) return x64.hlFolderGetFolderCount(pItem, bRecurse); else return x86.hlFolderGetFolderCount(pItem, bRecurse);
    }
    public static uint hlFolderGetFileCount(IntPtr pItem, bool bRecurse)
    {
        if (IsWow64()) return x64.hlFolderGetFileCount(pItem, bRecurse); else return x86.hlFolderGetFileCount(pItem, bRecurse);
    }

    //
    // Directory File
    //

    public static uint hlFileGetExtractable(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlFileGetExtractable(pItem); else return x86.hlFileGetExtractable(pItem);
    }
    public static HLValidation hlFileGetValidation(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlFileGetValidation(pItem); else return x86.hlFileGetValidation(pItem);
    }
    public static uint hlFileGetSize(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlFileGetSize(pItem); else return x86.hlFileGetSize(pItem);
    }
    public static uint hlFileGetSizeOnDisk(IntPtr pItem)
    {
        if (IsWow64()) return x64.hlFileGetSizeOnDisk(pItem); else return x86.hlFileGetSizeOnDisk(pItem);
    }

    public static bool hlFileCreateStream(IntPtr pItem, out IntPtr pStream)
    {
        if (IsWow64()) return x64.hlFileCreateStream(pItem, out pStream); else return x86.hlFileCreateStream(pItem, out pStream);
    }
    public static void hlFileReleaseStream(IntPtr pItem, IntPtr pStream)
    {
        if (IsWow64()) x64.hlFileReleaseStream(pItem, pStream); else x86.hlFileReleaseStream(pItem, pStream);
    }

    //
    // Stream
    //

    public static HLStreamType hlStreamGetType(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetType(pStream); else return x86.hlStreamGetType(pStream);
    }

    public static bool hlStreamGetOpened(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetOpened(pStream); else return x86.hlStreamGetOpened(pStream);
    }
    public static uint hlStreamGetMode(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetMode(pStream); else return x86.hlStreamGetMode(pStream);
    }

    public static bool hlStreamOpen(IntPtr pStream, uint uiMode)
    {
        if (IsWow64()) return x64.hlStreamOpen(pStream, uiMode); else return x86.hlStreamOpen(pStream, uiMode);
    }
    public static void hlStreamClose(IntPtr pStream)
    {
        if (IsWow64()) x64.hlStreamClose(pStream); else x86.hlStreamClose(pStream);
    }

    public static uint hlStreamGetStreamSize(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetStreamSize(pStream); else return x86.hlStreamGetStreamSize(pStream);
    }
    public static ulong hlStreamGetStreamSizeEx(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetStreamSizeEx(pStream); else return x86.hlStreamGetStreamSizeEx(pStream);
    }
    public static uint hlStreamGetStreamPointer(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetStreamPointer(pStream); else return x86.hlStreamGetStreamPointer(pStream);
    }
    public static ulong hlStreamGetStreamPointerEx(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlStreamGetStreamPointerEx(pStream); else return x86.hlStreamGetStreamPointerEx(pStream);
    }

    public static uint hlStreamSeek(IntPtr pStream, long iOffset, HLSeekMode eSeekMode)
    {
        if (IsWow64()) return x64.hlStreamSeek(pStream, iOffset, eSeekMode); else return x86.hlStreamSeek(pStream, iOffset, eSeekMode);
    }
    public static ulong hlStreamSeekEx(IntPtr pStream, long iOffset, HLSeekMode eSeekMode)
    {
        if (IsWow64()) return x64.hlStreamSeekEx(pStream, iOffset, eSeekMode); else return x86.hlStreamSeekEx(pStream, iOffset, eSeekMode);
    }

    public static bool hlStreamReadChar(IntPtr pStream, out char pChar)
    {
        if (IsWow64()) return x64.hlStreamReadChar(pStream, out pChar); else return x86.hlStreamReadChar(pStream, out pChar);
    }
    public static uint hlStreamRead(IntPtr pStream, IntPtr lpData, uint uiBytes)
    {
        if (IsWow64()) return x64.hlStreamRead(pStream, lpData, uiBytes); else return x86.hlStreamRead(pStream, lpData, uiBytes);
    }

    public static bool hlStreamWriteChar(IntPtr pStream, char iChar)
    {
        if (IsWow64()) return x64.hlStreamWriteChar(pStream, iChar); else return x86.hlStreamWriteChar(pStream, iChar);
    }
    public static uint hlStreamWrite(IntPtr pStream, IntPtr lpData, uint uiBytes)
    {
        if (IsWow64()) return x64.hlStreamWrite(pStream, lpData, uiBytes); else return x86.hlStreamWrite(pStream, lpData, uiBytes);
    }

    //
    // Package
    //

    public static bool hlBindPackage(uint uiPackage)
    {
        if (IsWow64()) return x64.hlBindPackage(uiPackage); else return x86.hlBindPackage(uiPackage);
    }

    public static HLPackageType hlGetPackageTypeFromName(string lpName)
    {
        if (IsWow64()) return x64.hlGetPackageTypeFromName(lpName); else return x86.hlGetPackageTypeFromName(lpName);
    }
    public static HLPackageType hlGetPackageTypeFromMemory(IntPtr lpBuffer, uint uiBufferSize)
    {
        if (IsWow64()) return x64.hlGetPackageTypeFromMemory(lpBuffer, uiBufferSize); else return x86.hlGetPackageTypeFromMemory(lpBuffer, uiBufferSize);
    }
    public static HLPackageType hlGetPackageTypeFromStream(IntPtr pStream)
    {
        if (IsWow64()) return x64.hlGetPackageTypeFromStream(pStream); else return x86.hlGetPackageTypeFromStream(pStream);
    }

    public static bool hlCreatePackage(HLPackageType ePackageType, out uint uiPackage)
    {
        if (IsWow64()) return x64.hlCreatePackage(ePackageType, out uiPackage); else return x86.hlCreatePackage(ePackageType, out uiPackage);
    }
    public static void hlDeletePackage(uint uiPackage)
    {
        if (IsWow64()) x64.hlDeletePackage(uiPackage); else x86.hlDeletePackage(uiPackage);
    }

    public static HLPackageType hlPackageGetType()
    {
        if (IsWow64()) return x64.hlPackageGetType(); else return x86.hlPackageGetType();
    }
    public static string hlPackageGetExtension()
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlPackageGetExtension(); else lpString = x86.hlPackageGetExtension();
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static string hlPackageGetDescription()
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlPackageGetDescription(); else lpString = x86.hlPackageGetDescription();
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }

    public static bool hlPackageGetOpened()
    {
        if (IsWow64()) return x64.hlPackageGetOpened(); else return x86.hlPackageGetOpened();
    }

    public static bool hlPackageOpenFile(string lpFileName, uint uiMode)
    {
        if (IsWow64()) return x64.hlPackageOpenFile(lpFileName, uiMode); else return x86.hlPackageOpenFile(lpFileName, uiMode);
    }
    public static bool hlPackageOpenMemory(IntPtr lpData, uint uiBufferSize, uint uiMode)
    {
        if (IsWow64()) return x64.hlPackageOpenMemory(lpData, uiBufferSize, uiMode); else return x86.hlPackageOpenMemory(lpData, uiBufferSize, uiMode);
    }
    public static bool hlPackageOpenProc(IntPtr pUserData, uint uiMode)
    {
        if (IsWow64()) return x64.hlPackageOpenProc(pUserData, uiMode); else return x86.hlPackageOpenProc(pUserData, uiMode);
    }
    public static bool hlPackageOpenStream(IntPtr pStream, uint uiMode)
    {
        if (IsWow64()) return x64.hlPackageOpenStream(pStream, uiMode); else return x86.hlPackageOpenStream(pStream, uiMode);
    }
    public static void hlPackageClose()
    {
        if (IsWow64()) x64.hlPackageClose(); else x86.hlPackageClose();
    }

    public static bool hlPackageDefragment()
    {
        if (IsWow64()) return x64.hlPackageDefragment(); else return x86.hlPackageDefragment();
    }

    public static IntPtr hlPackageGetRoot()
    {
        if (IsWow64()) return x64.hlPackageGetRoot(); else return x86.hlPackageGetRoot();
    }

    public static uint hlPackageGetAttributeCount()
    {
        if (IsWow64()) return x64.hlPackageGetAttributeCount(); else return x86.hlPackageGetAttributeCount();
    }
    public static string hlPackageGetAttributeName(HLPackageAttribute eAttribute)
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlPackageGetAttributeName(eAttribute); else lpString = x86.hlPackageGetAttributeName(eAttribute);
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static bool hlPackageGetAttribute(HLPackageAttribute eAttribute, out HLAttribute pAttribute)
    {
        if (IsWow64()) return x64.hlPackageGetAttribute(eAttribute, out pAttribute); else return x86.hlPackageGetAttribute(eAttribute, out pAttribute);
    }

    public static uint hlPackageGetItemAttributeCount()
    {
        if (IsWow64()) return x64.hlPackageGetItemAttributeCount(); else return x86.hlPackageGetItemAttributeCount();
    }
    public static string hlPackageGetItemAttributeName(HLPackageAttribute eAttribute)
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlPackageGetItemAttributeName(eAttribute); else lpString = x86.hlPackageGetItemAttributeName(eAttribute);
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static bool hlPackageGetItemAttribute(IntPtr pItem, HLPackageAttribute eAttribute, out HLAttribute pAttribute)
    {
        if (IsWow64()) return x64.hlPackageGetItemAttribute(pItem, eAttribute, out pAttribute); else return x86.hlPackageGetItemAttribute(pItem, eAttribute, out pAttribute);
    }

    public static bool hlPackageGetExtractable(IntPtr pItem, out bool pExtractable)
    {
        if (IsWow64()) return x64.hlPackageGetExtractable(pItem, out pExtractable); else return x86.hlPackageGetExtractable(pItem, out pExtractable);
    }
    public static bool hlPackageGetFileSize(IntPtr pItem, out uint pSize)
    {
        if (IsWow64()) return x64.hlPackageGetFileSize(pItem, out pSize); else return x86.hlPackageGetFileSize(pItem, out pSize);
    }
    public static bool hlPackageGetFileSizeOnDisk(IntPtr pItem, out uint pSize)
    {
        if (IsWow64()) return x64.hlPackageGetFileSizeOnDisk(pItem, out pSize); else return x86.hlPackageGetFileSizeOnDisk(pItem, out pSize);
    }
    public static bool hlPackageCreateStream(IntPtr pItem, out IntPtr pStream)
    {
        if (IsWow64()) return x64.hlPackageCreateStream(pItem, out pStream); else return x86.hlPackageCreateStream(pItem, out pStream);
    }
    public static void hlPackageReleaseStream(IntPtr pStream)
    {
        if (IsWow64()) x64.hlPackageReleaseStream(pStream); else x86.hlPackageReleaseStream(pStream);
    }

    public static string hlNCFFileGetRootPath()
    {
        IntPtr lpString;
        if (IsWow64()) lpString = x64.hlNCFFileGetRootPath(); else lpString = x86.hlNCFFileGetRootPath();
        return lpString == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(lpString);
    }
    public static void hlNCFFileSetRootPath(string lpRootPath)
    {
        if (IsWow64()) x64.hlNCFFileSetRootPath(lpRootPath); else x86.hlNCFFileSetRootPath(lpRootPath);
    }

    public static bool hlWADFileGetImageSizePaletted(IntPtr pItem, out uint uiPaletteDataSize, out uint uiPixelDataSize)
    {
        if (IsWow64()) return x64.hlWADFileGetImageSizePaletted(pItem, out uiPaletteDataSize, out uiPixelDataSize); else return x86.hlWADFileGetImageSizePaletted(pItem, out uiPaletteDataSize, out uiPixelDataSize);
    }
    public static bool hlWADFileGetImageDataPaletted(IntPtr pItem, out uint uiWidth, out uint uiHeight, out IntPtr lpPaletteData, out IntPtr lpPixelData)
    {
        if (IsWow64()) return x64.hlWADFileGetImageDataPaletted(pItem, out uiWidth, out uiHeight, out lpPaletteData, out lpPixelData); else return x86.hlWADFileGetImageDataPaletted(pItem, out uiWidth, out uiHeight, out lpPaletteData, out lpPixelData);
    }
    public static bool hlWADFileGetImageSize(IntPtr pItem, out uint uiPixelDataSize)
    {
        if (IsWow64()) return x64.hlWADFileGetImageSize(pItem, out uiPixelDataSize); else return x86.hlWADFileGetImageSize(pItem, out uiPixelDataSize);
    }
    public static bool hlWADFileGetImageData(IntPtr pItem, out uint uiWidth, out uint uiHeight, out IntPtr lpPixelData)
    {
        if (IsWow64()) return x64.hlWADFileGetImageData(pItem, out uiWidth, out uiHeight, out lpPixelData); else return x86.hlWADFileGetImageData(pItem, out uiWidth, out uiHeight, out lpPixelData);
    }

    private static class x86
    {
        //
        // VTFLib
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlInitialize();
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlShutdown();

        //
        // Get/Set
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetBoolean(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetBooleanValidate(HLOption eOption, [MarshalAs(UnmanagedType.U1)]out bool pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetBoolean(HLOption eOption, [MarshalAs(UnmanagedType.U1)]bool bValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int hlGetInteger(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetIntegerValidate(HLOption eOption, out int pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetInteger(HLOption eOption, int iValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlGetUnsignedInteger(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetUnsignedIntegerValidate(HLOption eOption, out uint pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetUnsignedInteger(HLOption eOption, uint uiValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetLongLong")]
        public static extern long hlGetLong(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetLongLongValidate")]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetLongValidate(HLOption eOption, out long pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlSetLongLong")]
        public static extern void hlSetLong(HLOption eOption, long iValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetUnsignedLongLong")]
        public static extern ulong hlGetUnsignedLong(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetUnsignedLongLongValidate")]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetUnsignedLongValidate(HLOption eOption, out ulong pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlSetUnsignedLongLong")]
        public static extern void hlSetUnsignedLong(HLOption eOption, ulong uiValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern float hlGetFloat(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetFloatValidate(HLOption eOption, out float pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetFloat(HLOption eOption, float pValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlGetString(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetStringValidate(HLOption eOption, out string pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetString(HLOption eOption, [MarshalAs(UnmanagedType.LPStr)]string lpValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlGetVoid(HLOption eOption);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetVoidValidate(HLOption eOption, out IntPtr pValue);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetVoid(HLOption eOption, IntPtr lpValue);

        //
        // Attributes
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlAttributeGetBoolean(ref HLAttribute pAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetBoolean(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, [MarshalAs(UnmanagedType.U1)]bool bValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int hlAttributeGetInteger(ref HLAttribute pAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetInteger(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, int iValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlAttributeGetUnsignedInteger(ref HLAttribute pAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetUnsignedInteger(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, uint uiValue, [MarshalAs(UnmanagedType.U1)]bool bHexadecimal);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern float hlAttributeGetFloat(ref HLAttribute pAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetFloat(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, float fValue);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlAttributeGetString(ref HLAttribute pAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetString(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, [MarshalAs(UnmanagedType.LPStr)]string lpValue);

        //
        // Directory Item
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLDirectoryItemType hlItemGetType(IntPtr pItem);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        //[return: MarshalAs(UnmanagedType.LPStr)]
        public static extern IntPtr hlItemGetName(IntPtr pItem);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlItemGetID(IntPtr pItem);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlItemGetData(IntPtr pItem);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlItemGetPackage(IntPtr pItem);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlItemGetParent(IntPtr pItem);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSize(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSizeEx(IntPtr pItem, out UInt64 pSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSizeOnDisk(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSizeOnDiskEx(IntPtr pItem, out UInt64 pSize);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetPath(IntPtr pItem, IntPtr lpPath, uint uiPathSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemExtract(IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpPath);

        //
        // Directory Folder
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetCount(IntPtr pItem);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderGetItem(IntPtr pItem, uint uiIndex);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderGetItemByName(IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpName, HLFindType eFind);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderGetItemByPath(IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpPath, HLFindType eFind);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlFolderSort(IntPtr pItem, HLSortField eField, HLSortOrder eOrder, [MarshalAs(UnmanagedType.U1)]bool bRecurse);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderFindFirst(IntPtr pFolder, [MarshalAs(UnmanagedType.LPStr)]string lpSearch, HLFindType eFind);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderFindNext(IntPtr pFolder, IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpSearch, HLFindType eFind);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetSize(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern UInt64 hlFolderGetSizeEx(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetSizeOnDisk(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern UInt64 hlFolderGetSizeOnDiskEx(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetFolderCount(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetFileCount(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);

        //
        // Directory File
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFileGetExtractable(IntPtr pItem);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLValidation hlFileGetValidation(IntPtr pItem);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFileGetSize(IntPtr pItem);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFileGetSizeOnDisk(IntPtr pItem);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlFileCreateStream(IntPtr pItem, out IntPtr pStream);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlFileReleaseStream(IntPtr pItem, IntPtr pStream);

        //
        // Stream
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLStreamType hlStreamGetType(IntPtr pStream);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamGetOpened(IntPtr pStream);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamGetMode(IntPtr pStream);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamOpen(IntPtr pStream, uint uiMode);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlStreamClose(IntPtr pStream);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamGetStreamSize(IntPtr pStream);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern ulong hlStreamGetStreamSizeEx(IntPtr pStream);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamGetStreamPointer(IntPtr pStream);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern ulong hlStreamGetStreamPointerEx(IntPtr pStream);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamSeek(IntPtr pStream, long iOffset, HLSeekMode eSeekMode);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern ulong hlStreamSeekEx(IntPtr pStream, long iOffset, HLSeekMode eSeekMode);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamReadChar(IntPtr pStream, out char pChar);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamRead(IntPtr pStream, IntPtr lpData, uint uiBytes);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamWriteChar(IntPtr pStream, char iChar);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamWrite(IntPtr pStream, IntPtr lpData, uint uiBytes);

        //
        // Package
        //

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlBindPackage(uint uiPackage);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlGetPackageTypeFromName([MarshalAs(UnmanagedType.LPStr)]string lpName);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlGetPackageTypeFromMemory(IntPtr lpBuffer, uint uiBufferSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlGetPackageTypeFromStream(IntPtr pStream);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlCreatePackage(HLPackageType ePackageType, out uint uiPackage);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlDeletePackage(uint uiPackage);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlPackageGetType();
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetExtension();
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetDescription();

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetOpened();

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenFile([MarshalAs(UnmanagedType.LPStr)]string lpFileName, uint uiMode);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenMemory(IntPtr lpData, uint uiBufferSize, uint uiMode);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenProc(IntPtr pUserData, uint uiMode);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenStream(IntPtr pStream, uint uiMode);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlPackageClose();

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageDefragment();

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetRoot();

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlPackageGetAttributeCount();
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetAttributeName(HLPackageAttribute eAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetAttribute(HLPackageAttribute eAttribute, out HLAttribute pAttribute);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlPackageGetItemAttributeCount();
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetItemAttributeName(HLPackageAttribute eAttribute);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetItemAttribute(IntPtr pItem, HLPackageAttribute eAttribute, out HLAttribute pAttribute);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetExtractable(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]out bool pExtractable);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetFileSize(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetFileSizeOnDisk(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageCreateStream(IntPtr pItem, out IntPtr pStream);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlPackageReleaseStream(IntPtr pStream);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlNCFFileGetRootPath();
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlNCFFileSetRootPath([MarshalAs(UnmanagedType.LPStr)]string lpRootPath);

        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageSizePaletted(IntPtr pItem, out uint uiPaletteDataSize, out uint uiPixelDataSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageDataPaletted(IntPtr pItemm, out uint uiWidth, out uint uiHeight, out IntPtr lpPaletteData, out IntPtr lpPixelData);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageSize(IntPtr pItem, out uint uiPixelDataSize);
        [DllImport("HLLib.x86.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageData(IntPtr pItem, out uint uiWidth, out uint uiHeight, out IntPtr lpPixelData);
    }

    private static class x64
    {
        //
        // VTFLib
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlInitialize();
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlShutdown();

        //
        // Get/Set
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetBoolean(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetBooleanValidate(HLOption eOption, [MarshalAs(UnmanagedType.U1)]out bool pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetBoolean(HLOption eOption, [MarshalAs(UnmanagedType.U1)]bool bValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int hlGetInteger(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetIntegerValidate(HLOption eOption, out int pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetInteger(HLOption eOption, int iValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlGetUnsignedInteger(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetUnsignedIntegerValidate(HLOption eOption, out uint pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetUnsignedInteger(HLOption eOption, uint uiValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetLongLong")]
        public static extern long hlGetLong(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetLongLongValidate")]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetLongValidate(HLOption eOption, out long pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlSetLongLong")]
        public static extern void hlSetLong(HLOption eOption, long iValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetUnsignedLongLong")]
        public static extern ulong hlGetUnsignedLong(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlGetUnsignedLongLongValidate")]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetUnsignedLongValidate(HLOption eOption, out ulong pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "hlSetUnsignedLongLong")]
        public static extern void hlSetUnsignedLong(HLOption eOption, ulong uiValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern float hlGetFloat(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetFloatValidate(HLOption eOption, out float pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetFloat(HLOption eOption, float pValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlGetString(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetStringValidate(HLOption eOption, out string pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetString(HLOption eOption, [MarshalAs(UnmanagedType.LPStr)]string lpValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlGetVoid(HLOption eOption);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlGetVoidValidate(HLOption eOption, out IntPtr pValue);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlSetVoid(HLOption eOption, IntPtr lpValue);

        //
        // Attributes
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlAttributeGetBoolean(ref HLAttribute pAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetBoolean(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, [MarshalAs(UnmanagedType.U1)]bool bValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int hlAttributeGetInteger(ref HLAttribute pAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetInteger(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, int iValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlAttributeGetUnsignedInteger(ref HLAttribute pAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetUnsignedInteger(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, uint uiValue, [MarshalAs(UnmanagedType.U1)]bool bHexadecimal);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern float hlAttributeGetFloat(ref HLAttribute pAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetFloat(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, float fValue);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlAttributeGetString(ref HLAttribute pAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlAttributeSetString(ref HLAttribute pAttribute, [MarshalAs(UnmanagedType.LPStr)]string lpName, [MarshalAs(UnmanagedType.LPStr)]string lpValue);

        //
        // Directory Item
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLDirectoryItemType hlItemGetType(IntPtr pItem);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        //[return: MarshalAs(UnmanagedType.LPStr)]
        public static extern IntPtr hlItemGetName(IntPtr pItem);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlItemGetID(IntPtr pItem);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlItemGetData(IntPtr pItem);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlItemGetPackage(IntPtr pItem);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlItemGetParent(IntPtr pItem);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSize(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSizeEx(IntPtr pItem, out UInt64 pSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSizeOnDisk(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetSizeOnDiskEx(IntPtr pItem, out UInt64 pSize);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemGetPath(IntPtr pItem, IntPtr lpPath, uint uiPathSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlItemExtract(IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpPath);

        //
        // Directory Folder
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetCount(IntPtr pItem);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderGetItem(IntPtr pItem, uint uiIndex);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderGetItemByName(IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpName, HLFindType eFind);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderGetItemByPath(IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpPath, HLFindType eFind);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlFolderSort(IntPtr pItem, HLSortField eField, HLSortOrder eOrder, [MarshalAs(UnmanagedType.U1)]bool bRecurse);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderFindFirst(IntPtr pFolder, [MarshalAs(UnmanagedType.LPStr)]string lpSearch, HLFindType eFind);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlFolderFindNext(IntPtr pFolder, IntPtr pItem, [MarshalAs(UnmanagedType.LPStr)]string lpSearch, HLFindType eFind);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetSize(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern UInt64 hlFolderGetSizeEx(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetSizeOnDisk(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern UInt64 hlFolderGetSizeOnDiskEx(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetFolderCount(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFolderGetFileCount(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]bool bRecurse);

        //
        // Directory File
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFileGetExtractable(IntPtr pItem);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLValidation hlFileGetValidation(IntPtr pItem);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFileGetSize(IntPtr pItem);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlFileGetSizeOnDisk(IntPtr pItem);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlFileCreateStream(IntPtr pItem, out IntPtr pStream);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlFileReleaseStream(IntPtr pItem, IntPtr pStream);

        //
        // Stream
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLStreamType hlStreamGetType(IntPtr pStream);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamGetOpened(IntPtr pStream);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamGetMode(IntPtr pStream);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamOpen(IntPtr pStream, uint uiMode);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlStreamClose(IntPtr pStream);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamGetStreamSize(IntPtr pStream);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern ulong hlStreamGetStreamSizeEx(IntPtr pStream);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamGetStreamPointer(IntPtr pStream);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern ulong hlStreamGetStreamPointerEx(IntPtr pStream);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamSeek(IntPtr pStream, long iOffset, HLSeekMode eSeekMode);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern ulong hlStreamSeekEx(IntPtr pStream, long iOffset, HLSeekMode eSeekMode);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamReadChar(IntPtr pStream, out char pChar);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamRead(IntPtr pStream, IntPtr lpData, uint uiBytes);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlStreamWriteChar(IntPtr pStream, char iChar);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlStreamWrite(IntPtr pStream, IntPtr lpData, uint uiBytes);

        //
        // Package
        //

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlBindPackage(uint uiPackage);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlGetPackageTypeFromName([MarshalAs(UnmanagedType.LPStr)]string lpName);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlGetPackageTypeFromMemory(IntPtr lpBuffer, uint uiBufferSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlGetPackageTypeFromStream(IntPtr pStream);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlCreatePackage(HLPackageType ePackageType, out uint uiPackage);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlDeletePackage(uint uiPackage);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern HLPackageType hlPackageGetType();
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetExtension();
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetDescription();

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetOpened();

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenFile([MarshalAs(UnmanagedType.LPStr)]string lpFileName, uint uiMode);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenMemory(IntPtr lpData, uint uiBufferSize, uint uiMode);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenProc(IntPtr pUserData, uint uiMode);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageOpenStream(IntPtr pStream, uint uiMode);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlPackageClose();

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageDefragment();

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetRoot();

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlPackageGetAttributeCount();
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetAttributeName(HLPackageAttribute eAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetAttribute(HLPackageAttribute eAttribute, out HLAttribute pAttribute);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern uint hlPackageGetItemAttributeCount();
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlPackageGetItemAttributeName(HLPackageAttribute eAttribute);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetItemAttribute(IntPtr pItem, HLPackageAttribute eAttribute, out HLAttribute pAttribute);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetExtractable(IntPtr pItem, [MarshalAs(UnmanagedType.U1)]out bool pExtractable);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetFileSize(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageGetFileSizeOnDisk(IntPtr pItem, out uint pSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlPackageCreateStream(IntPtr pItem, out IntPtr pStream);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlPackageReleaseStream(IntPtr pStream);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr hlNCFFileGetRootPath();
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void hlNCFFileSetRootPath([MarshalAs(UnmanagedType.LPStr)]string lpRootPath);

        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageSizePaletted(IntPtr pItem, out uint uiPaletteDataSize, out uint uiPixelDataSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageDataPaletted(IntPtr pItemm, out uint uiWidth, out uint uiHeight, out IntPtr lpPaletteData, out IntPtr lpPixelData);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageSize(IntPtr pItem, out uint uiPixelDataSize);
        [DllImport("HLLib.x64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool hlWADFileGetImageData(IntPtr pItem, out uint uiWidth, out uint uiHeight, out IntPtr lpPixelData);
    }
    #endregion
}
