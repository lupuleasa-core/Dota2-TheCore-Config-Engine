#SingleInstance force
#IfWinActive ahk_exe dota2.exe
~*Space::ControlSend, , {LWin Down}, Dota 2
*Space Up::Send {LWin Up}
~*CapsLock Up::SetCapsLockState Off
