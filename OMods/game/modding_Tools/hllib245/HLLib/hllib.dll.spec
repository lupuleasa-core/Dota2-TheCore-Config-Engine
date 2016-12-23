1   stdcall hlAttributeGetBoolean( ptr ) hlAttributeGetBoolean
2   stdcall hlAttributeGetFloat( ptr ) hlAttributeGetFloat
3   stdcall hlAttributeGetInteger( ptr ) hlAttributeGetInteger
4   stdcall hlAttributeGetString( ptr ) hlAttributeGetString
5   stdcall hlAttributeGetUnsignedInteger( ptr ) hlAttributeGetUnsignedInteger
6   stdcall hlAttributeSetBoolean( ptr ptr long ) hlAttributeSetBoolean
7   stdcall hlAttributeSetFloat( ptr ptr long ) hlAttributeSetFloat
8   stdcall hlAttributeSetInteger( ptr ptr long ) hlAttributeSetInteger
9   stdcall hlAttributeSetString( ptr ptr ptr ) hlAttributeSetString
10  stdcall hlAttributeSetUnsignedInteger( ptr ptr long long ) hlAttributeSetUnsignedInteger
11  stdcall hlBindPackage( long ) hlBindPackage
12  stdcall hlCreatePackage( ptr ptr ) hlCreatePackage
13  stdcall hlDeletePackage( long ) hlDeletePackage
14  stdcall hlFileCreateStream( ptr ptr ) hlFileCreateStream
15  stdcall hlFileGetExtractable( ptr ) hlFileGetExtractable
16  stdcall hlFileGetSize( ptr ) hlFileGetSize
17  stdcall hlFileGetSizeOnDisk( ptr ) hlFileGetSizeOnDisk
18  stdcall hlFileGetValidation( ptr ) hlFileGetValidation
19  stdcall hlFileReleaseStream( ptr ptr ) hlFileReleaseStream
20  stdcall hlFolderFindFirst( ptr ptr ptr ) hlFolderFindFirst
21  stdcall hlFolderFindNext( ptr ptr ptr ptr ) hlFolderFindNext
22  stdcall hlFolderGetCount( ptr ) hlFolderGetCount
23  stdcall hlFolderGetFileCount( ptr long ) hlFolderGetFileCount
24  stdcall hlFolderGetFolderCount( ptr long ) hlFolderGetFolderCount
25  stdcall hlFolderGetItem( ptr long ) hlFolderGetItem
26  stdcall hlFolderGetItemByName( ptr ptr ptr ) hlFolderGetItemByName
27  stdcall hlFolderGetItemByPath( ptr ptr ptr ) hlFolderGetItemByPath
28  stdcall hlFolderGetSize( ptr long ) hlFolderGetSize
29  stdcall hlFolderGetSizeOnDisk( ptr long ) hlFolderGetSizeOnDisk
30  stdcall hlFolderSort( ptr ptr ptr long ) hlFolderSort
31  stdcall hlGetBoolean( ptr ) hlGetBoolean
32  stdcall hlGetBooleanValidate( ptr ptr ) hlGetBooleanValidate
33  stdcall hlGetFloat( ptr ) hlGetFloat
34  stdcall hlGetFloatValidate( ptr ptr ) hlGetFloatValidate
35  stdcall hlGetInteger( ptr ) hlGetInteger
36  stdcall hlGetIntegerValidate( ptr ptr ) hlGetIntegerValidate
37  stdcall hlGetPackageTypeFromMemory( ptr long ) hlGetPackageTypeFromMemory
38  stdcall hlGetPackageTypeFromName( ptr ) hlGetPackageTypeFromName
39  stdcall hlGetPackageTypeFromStream( ptr ) hlGetPackageTypeFromStream
40  stdcall hlGetString( ptr ) hlGetString
41  stdcall hlGetStringValidate( ptr ptr ) hlGetStringValidate
42  stdcall hlGetVoid( ptr ) hlGetVoid
43  stdcall hlGetVoidValidate( ptr ptr ) hlGetVoidValidate
44  stdcall hlInitialize() hlInitialize
45  stdcall hlItemExtract( ptr ptr ) hlItemExtract
46  stdcall hlItemGetData( ptr ) hlItemGetData
47  stdcall hlItemGetID( ptr ) hlItemGetID
48  stdcall hlItemGetName( ptr ) hlItemGetName
49  stdcall hlItemGetPackage( ptr ) hlItemGetPackage
50  stdcall hlItemGetParent( ptr ) hlItemGetParent
51  stdcall hlItemGetPath( ptr ptr long ) hlItemGetPath
52  stdcall hlItemGetSize( ptr ptr ) hlItemGetSize
53  stdcall hlItemGetSizeOnDisk( ptr ptr ) hlItemGetSizeOnDisk
54  stdcall hlItemGetType( ptr ) hlItemGetType
55  stdcall hlNCFFileGetRootPath() hlNCFFileGetRootPath
56  stdcall hlNCFFileSetRootPath( ptr ) hlNCFFileSetRootPath
57  stdcall hlPackageClose() hlPackageClose
58  stdcall hlPackageCreateStream( ptr ptr ) hlPackageCreateStream
59  stdcall hlPackageDefragment() hlPackageDefragment
60  stdcall hlPackageGetAttribute( ptr ptr ) hlPackageGetAttribute
61  stdcall hlPackageGetAttributeCount() hlPackageGetAttributeCount
61  stdcall hlPackageGetAttributeName( ptr ) hlPackageGetAttributeName
62  stdcall hlPackageGetDescription() hlPackageGetDescription
63  stdcall hlPackageGetExtension() hlPackageGetExtension
64  stdcall hlPackageGetExtractable( ptr ptr ) hlPackageGetExtractable
65  stdcall hlPackageGetFileSize( ptr ptr ) hlPackageGetFileSize
66  stdcall hlPackageGetFileSizeOnDisk( ptr ptr ) hlPackageGetFileSizeOnDisk
67  stdcall hlPackageGetItemAttribute( ptr ptr ptr ) hlPackageGetItemAttribute
68  stdcall hlPackageGetItemAttributeCount() hlPackageGetItemAttributeCount
68  stdcall hlPackageGetItemAttributeName( ptr ) hlPackageGetItemAttributeName
69  stdcall hlPackageGetOpened() hlPackageGetOpened
70  stdcall hlPackageGetRoot() hlPackageGetRoot
71  stdcall hlPackageGetType() hlPackageGetType
72  stdcall hlPackageOpenFile( ptr long ) hlPackageOpenFile
73  stdcall hlPackageOpenMemory( ptr long long ) hlPackageOpenMemory
74  stdcall hlPackageOpenProc( ptr long ) hlPackageOpenProc
75  stdcall hlPackageOpenStream( ptr long ) hlPackageOpenStream
76  stdcall hlPackageReleaseStream( ptr ) hlPackageReleaseStream
77  stdcall hlSetBoolean( ptr long ) hlSetBoolean
78  stdcall hlSetFloat( ptr long ) hlSetFloat
79  stdcall hlSetInteger( ptr long ) hlSetInteger
80  stdcall hlSetString( ptr ptr ) hlSetString
81  stdcall hlSetVoid( ptr ptr ) hlSetVoid
82  stdcall hlShutdown() hlShutdown
83  stdcall hlStreamClose( ptr ) hlStreamClose
84  stdcall hlStreamGetMode( ptr ) hlStreamGetMode
85  stdcall hlStreamGetOpened( ptr ) hlStreamGetOpened
86  stdcall hlStreamGetStreamPointer( ptr ) hlStreamGetStreamPointer
87  stdcall hlStreamGetStreamSize( ptr ) hlStreamGetStreamSize
88  stdcall hlStreamGetType( ptr ) hlStreamGetType
89  stdcall hlStreamOpen( ptr long ) hlStreamOpen
90  stdcall hlStreamRead( ptr ptr long ) hlStreamRead
91  stdcall hlStreamReadChar( ptr ptr ) hlStreamReadChar
92  stdcall hlStreamSeek( ptr long ptr ) hlStreamSeek
93  stdcall hlStreamWrite( ptr ptr long ) hlStreamWrite
94  stdcall hlStreamWriteChar( ptr long ) hlStreamWriteChar
95  stdcall hlWADFileGetImageData( ptr ptr ptr ptr ) hlWADFileGetImageData
96  stdcall hlWADFileGetImageDataPaletted( ptr ptr ptr ptr ptr ) hlWADFileGetImageDataPaletted
97  stdcall hlWADFileGetImageSize( ptr ptr ) hlWADFileGetImageSize
98  stdcall hlWADFileGetImageSizePaletted( ptr ptr ptr ) hlWADFileGetImageSizePaletted
