@RD /S /Q "mod_MapReplacer"
if not exist "modding_Tools\VPK CREATOR\dota" mkdir "modding_Tools\VPK CREATOR\dota"
"%~dp0\modding_Tools\hllib245\bin\x64\HLExtract.exe" -p "dota\maps\dota.vpk" -d "modding_Tools\VPK CREATOR\dota" -e "maps"
"%~dp0\modding_Tools\hllib245\bin\x64\HLExtract.exe" -p "dota\maps\dota_autumn.vpk" -d "modding_Tools\VPK CREATOR\dota" -e "maps"
"%~dp0\modding_Tools\VPK CREATOR\vpk.exe" "modding_Tools\VPK CREATOR\dota"
@RD /S /Q "modding_Tools\VPK CREATOR\dota"
if not exist "mod_MapReplacer\maps" mkdir "mod_MapReplacer\maps"
move /Y "modding_Tools\VPK CREATOR\dota.vpk" "mod_MapReplacer\maps\dota.vpk"

@RD /S /Q "modding_Tools\VPK CREATOR\pak01_dir"
if not exist "modding_Tools\VPK CREATOR\pak01_dir\materials\overviews" mkdir "modding_Tools\VPK CREATOR\pak01_dir\materials\overviews"
"%~dp0\modding_Tools\hllib245\bin\x64\HLExtract.exe" -p "dota\pak01_dir.vpk" -d "modding_Tools\VPK CREATOR\pak01_dir\materials\overviews" -e "materials\overviews\dota_autumn.vmat_c"
ren "%~dp0\modding_Tools\VPK CREATOR\pak01_dir\materials\overviews\dota_autumn.vmat_c" dota.vmat_c

"%~dp0\modding_Tools\VPK CREATOR\vpk.exe" "modding_Tools\VPK CREATOR\pak01_dir"
move /Y "modding_Tools\VPK CREATOR\pak01_dir.vpk" "mod_MapReplacer\pak01_dir.vpk"
START steam://rungameid/570