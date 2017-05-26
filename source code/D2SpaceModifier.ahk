; **Highly Recommended**
; Prevents more than one instance of this script from running by closing the old instance when a new one is ran.
#SingleInstance force 

; **Highly Recommended**
; Makes the script only active while you are "Alt-tabbed" into DotA 2.
#IfWinActive ahk_exe dota2.exe 

; **Required**
; Hides all down key presses of the left windows key from the operating system and ALL applications EXCEPT dota2.exe.
~*Space::ControlSend, , {LWin Down}, Dota 2 

; **Required**
; Allows all releases of the left windows key to be seen by the operating system and ALL applications INCLUDING dota2.exe.
*Space Up::Send {LWin Up}

; **Highly Recommended**
; Upon releasing the CapsLock key, the CapsLock State is set to off. 
; This method allows you to still bind keys to it using the in-game UI.
; It also prevents conflict due to the windows key and CapsLock interaction.
~*CapsLock Up::SetCapsLockState Off

; **Optional**
; Binds the Unified Orders Modifier to Mouse5 inorder to improve accessibility.
*XButton2::Ctrl
