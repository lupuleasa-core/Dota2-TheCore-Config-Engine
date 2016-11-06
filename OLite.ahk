#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

////////////////////////////////////////////////////////////////////////////////////////
//  General                                                                           //
////////////////////////////////////////////////////////////////////////////////////////
#CommentFlag //
// The script is only active when dota2.exe is active.
#IfWinActive ahk_exe dota2.exe

////////////////////////////////////////////////////////////////////////////////////////
//  Keybinds                                                                          //
////////////////////////////////////////////////////////////////////////////////////////
// Unlocks the Second Modifier
*LAlt::
ControlSend, , {LWin down}, Dota 2
Keywait, LAlt
ControlSend, , {LWin up}, Dota 2
Return

// Binds Mouse5 as a Unified Orders Modifier
XButton2::LCtrl

////////////////////////////////////////////////////////////////////////////////////////
//  Workarounds and Fixes                                                             //
////////////////////////////////////////////////////////////////////////////////////////
// Fixes Alt + Tab
#If GetKeyState("LAlt", "p")
Tab::AltTabAndMenu
#IfWinActive ahk_exe dota2.exe

// Keep CapsLock off
*CapsLock::
Send {CapsLock down}
Keywait, CapsLock
SetCapsLockState, off
Send {CapsLock up}
Return

// Disables Left Winkey by making it a Unified Orders Modifier
*LWin::LCtrl

////////////////////////////////////////////////////////////////////////////////////////
//  Colored Chat Hotstrings                                                           //
////////////////////////////////////////////////////////////////////////////////////////
#Hotstring EndChars :
//red = "r:"
::r::
Clipboard = 
SendEvent ^v^v
Return

//blue = "b:"
::b::
Clipboard = 
SendEvent ^v^v
Return

//green = "g:"
::g::
Clipboard = 
SendEvent ^v^v
Return

//yellow = "y:"
::y::
Clipboard = 
SendEvent ^v^v
Return

//magenta = "m:"
::m::
Clipboard = 
SendEvent ^v^v
Return

//violet = "v:"
::v::
Clipboard = 
SendEvent ^v^v
Return

//orange = "o:"
::o::
Clipboard = 
SendEvent ^v^v
Return

//indigo = "i:"
::i::
Clipboard = 
SendEvent ^v^v
Return

//pink = "p:"
::p::
Clipboard = 
SendEvent ^v^v
Return

//dark green = "dg:"
::dg::
Clipboard = 
SendEvent ^v^v
Return

//yellow-green = "yg:"
::yg::
Clipboard = 
SendEvent ^v^v
Return

//olive = "ol:"
::ol::
Clipboard = 
SendEvent ^v^v
Return

//red-pink = "rp:"
::rp::
Clipboard = 
SendEvent ^v^v
Return

//silver = "s:"
::s::
Clipboard = 
SendEvent ^v^v
Return