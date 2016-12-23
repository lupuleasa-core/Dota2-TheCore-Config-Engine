@echo off

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %*:"=""
    echo UAC.ShellExecute "cmd.exe", "/c %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------
cd /d %~dp0
@RD /S /Q "dota\guides"
if not exist "dota_cache\guides" mkdir "dota_cache\guides"
mklink /j dota\guides dota_cache\guides
@RD /S /Q "dota\replays"0
if not exist "dota_cache\replays" mkdir "dota_cache\replays"
mklink /j dota\replays dota_cache\replays
del "dota\demoheader.tmp"
copy NUL dota_cache\demoheader.tmp
mklink /h dota\demoheader.tmp dota_cache\demoheader.tmp
del "dota\rich_presence_persistent_data.txt"
copy NUL dota_cache\rich_presence_persistent_data.txt
mklink /h dota\rich_presence_persistent_data.txt dota_cache\rich_presence_persistent_data.txt
del  "dota\server_log.txt"
copy NUL dota_cache\server_log.txt
mklink /h dota\server_log.txt dota_cache\server_log.txt
del "dota\steamdatagram_tickets.bin"
copy NUL dota_cache\steamdatagram_tickets.bin
mklink /h dota\steamdatagram_tickets.bin dota_cache\steamdatagram_tickets.bin