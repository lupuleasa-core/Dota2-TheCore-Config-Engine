#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

#CommentFlag //

//The script is only active when dota2.exe is active.
#IfWinActive ahk_exe dota2.exe

//Unlocks the Second Modifier
LAlt::
ControlSend, , {LWin down}, Dota 2
Keywait, LAlt
ControlSend, , {LWin up}, Dota 2
Return

//Control + Tab is now Alt + Tab
<^Tab::AltTab

//Turn off Capslock
SetCapsLockState, off

//Keep Capslock off at all times
*Capslock::
Send {Capslock down}
SetCapsLockState, off
Keywait, Capslock
Send {Capslock up}
Return

//Remap the left WinKey to left Ctrl
LWin::LCtrl