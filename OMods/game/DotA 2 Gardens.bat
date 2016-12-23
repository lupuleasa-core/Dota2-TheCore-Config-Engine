@RD /S /Q "mod_MapReplacer"
if not exist "modding_Tools\VPK CREATOR\dota" mkdir "modding_Tools\VPK CREATOR\dota"
"%~dp0\modding_Tools\hllib245\bin\x64\HLExtract.exe" -p "dota\maps\dota.vpk" -d "modding_Tools\VPK CREATOR\dota" -e "maps"
"%~dp0\modding_Tools\hllib245\bin\x64\HLExtract.exe" -p "dota\maps\dota_coloseum.vpk" -d "modding_Tools\VPK CREATOR\dota" -e "maps"
"%~dp0\modding_Tools\VPK CREATOR\vpk.exe" "modding_Tools\VPK CREATOR\dota"
@RD /S /Q "modding_Tools\VPK CREATOR\dota"
if not exist "mod_MapReplacer\maps" mkdir "mod_MapReplacer\maps"
move /Y "modding_Tools\VPK CREATOR\dota.vpk" "mod_MapReplacer\maps\dota.vpk"
START steam://rungameid/570