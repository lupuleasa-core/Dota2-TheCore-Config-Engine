XCOPY "..\sln\%1\HLExtract\Win32\Release\HLExtract.exe" "..\bin\x86\" /Y
XCOPY "..\sln\%1\HLLib\Win32\Release\HLLib.dll" "..\bin\x86\" /Y
XCOPY "..\sln\%1\HLLib\Win32\Release\HLLib.lib" "..\lib\x86\" /Y

XCOPY "..\sln\%1\HLExtract\x64\Release\HLExtract.exe" "..\bin\x64\" /Y
XCOPY "..\sln\%1\HLLib\x64\Release\HLLib.dll" "..\bin\x64\" /Y
XCOPY "..\sln\%1\HLLib\x64\Release\HLLib.lib" "..\lib\x64\" /Y