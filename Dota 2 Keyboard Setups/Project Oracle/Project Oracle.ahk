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

//Keep Capslock off when typing chat
*Enter::
Send {Enter down}
SetCapsLockState, off
Keywait, Enter
Send {Enter up}
Return

//Remap the left WinKey to left Ctrl
LWin::LCtrl
