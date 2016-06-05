#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

#CommentFlag //
#IfWinActive ahk_exe dota2.exe

a=0 //ToggleRuneShuffle
b=0 //ToggleAutoAttack
c=0 //ToggleOrbEffects
d=0 //ToggleSelectSummons
e=0 //ToggleOpenMic
f=0 //ToggleDeveloperMode
g=1 //ToggleRClickRepeat
i=0 //TrackSpace
j=1 //ToggleRClickDeny
k=0 //TrackRClickDenySpace

ProcessExist(Name)
{
	Process,Exist,%Name%
	return Errorlevel
}
Loop,
{
	Sleep, 1000
	If ProcessExist("dota2.exe")
	{
		z:="dota2.exe"
		Process, WaitClose, %z%	
		Reload			
	}
}
Return

//Make Space still work as a modifier
~Space::
	If k=0
	{
		i=1
		Keywait, Space
		i=0
	}
	Else
	{
		Send {F15}
		Send {F16}
		i=1
		Keywait, Space
		Send {F16}
		Send {F15}
		i=0
	}
Return

//Remap Alt to Space
~Alt::
Send {Space down}
Keywait, Alt
Send {Space up}
Return

//Remap ` to Space
`::
If i=0
{
	Send {Space down}
	Keywait, ``
	Send {Space up}
}
Else
{
	Send {`` down}
	Keywait, ``
	Send {`` up}
}

LWin::LCtrl //Make the Left Windows Key function as the Left Control Key
#A::Return //Disable Windows Key Shortcuts

////////////////////////////////////////////////////////////
//  Function Keys                                         //
////////////////////////////////////////////////////////////
F1::
	If a=0
	{
		Send {F1}
		Keywait, F1
		Send {F1}
		a=1
	}
	Else
	{
		Send {F2}
		Keywait, F1
		Send {F2}
		a=0
	}
Return
Space & F1::
Send {F1 down}
Keywait, F1
Send {F1 up}
Return
!F1::
Send {F6 down}
Keywait, F1
Send {F6 up}
Return
	
F2::
Send {F4 down}
Keywait, F2
Send {F4 up}
Return
Space & F2::
Send {F4}
Send {F4 down}
Keywait, F2
Send {F4 up}
Return
!F2::
Send {F7 down}
Keywait, F2
Send {F7 up}
Return

!F3::
Send {F4}
Send {Y}
Send {F3}
Send {G}
Send {Numpad0}
Keywait, Alt
Send {1}
Return

F4::
Send {F5 down}
Keywait, F4
Send {F5 up}
Return
Space & F4::
Keywait, Space
Send {B}
Send {0}
Send {B}
Return
!F4::
Send {F2 Down}
Keywait, F4
Send {F2 Up}
Return

F5::
Send {F8 down}
Keywait, F5
Send {F8 up}
Return
Space & F5::
	If d=0
	{
		Send {vk29} //Select
		Send {ScrollLock}
		Send {= down}
		d=1
		Keywait, F5
		Send {= up}
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {Pause}
		Send {= down}
		d=0
		Keywait, F5
		Send {= up}
	}
Return
!F5::
Keywait, Alt
Send {F8}
Return

Space & F6::
	If g=0
	{
		Send {vk29} //Select
		Send {F13}
		Send {BS down}
		g=1
		Keywait, F6
		Send {BS up}
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {F14}
		Send {BS down}
		g=0
		Keywait, F6
		Send {BS up}
	}
Return

Space & F7::Return

F8::Return
Space & F8::Return
!F8::
Send {NumpadDiv}
Sleep, 200
Reload
Return

F11::
	If f=0
	{
		Send {vk29} //Select
		Send {NumpadMult} 
		Send {F11 down}
		f=1
		Keywait, F11
		Send {F11 up}
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {NumpadSub}
		Send {F11 down}
		f=0
		Keywait, F11
		Send {F11 up}
	}
!F11::
	If f=0
	{
		Send {vk29} //Select
		Send {NumpadMult} 
		Send {F11 down}
		f=1
		Keywait, F11
		Send {F11 up}
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {NumpadSub}
		Send {F11 down}
		f=0
		Keywait, F11
		Send {F11 up}
	}

!F12::
Send {F12 down}
Keywait, F12
Send {F12 up}
Return

Space & 1::
Keywait, Space
Send {1}
Send {1}
Return

Space & 2::
Keywait, Space
Send {2}
Send {2}
Return

Space & 3::
Send {3}
Send {3 down}
Keywait, 3
Send {3 up}
Return

Space & 4::
Send {4}
Send {4 down}
Keywait, 4
Send {4 up}
Return

Space & 5::
Send {5}
Send {5 down}
Keywait, 5
Send {5 up}
Return

Space & 6::
Send {6}
Send {6 down}
Keywait, 6
Send {6 up}
Return

Space & 7::
Send {7}
Send {7 down}
Keywait, 7
Send {7 up}
Return

Space & 8::
Send {8}
Send {8 down}
Keywait, 8
Send {8 up}
Return

Space & 9::
Send {9}
Send {9 down}
Keywait, 9
Send {9 up}
Return

Space & 0::
Send {0}
Send {0 down}
Keywait, 0
Send {0 up}
Return

Space & -::
Send {-}
Send {- down}
Keywait, -
Send {- up}
Return

=::
	If d=0
	{
		Send {vk29} //Select
		Send {ScrollLock}
		Send {= down}
		d=1
		Keywait, =
		Send {= up}
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {Pause}
		Send {= down}
		d=0
		Keywait, =
		Send {= up}
	}
Return
Space & =::
	If d=0
	{
		Send {vk29} //Select
		Send {ScrollLock}
		Send {= down}
		d=1
		Keywait, =
		Send {= up}
	}	
	Else
	{
		Send {vkF7} //CrSel
		Send {Pause}
		Send {= down}
		d=0
		Keywait, =
		Send {= up}
	}
Return
!=::
	If d=0
	{
		Send {vk29} //Select
		Send {ScrollLock}
		Send {= down}
		d=1
		Keywait, =
		Send {= up}
	}	
	Else
	{
		Send {vkF7} //CrSel
		Send {Pause}
		Send {= down}
		d=0
		Keywait, =
		Send {= up}
	}
Return

BS::
	If g=0
	{
		Send {vk29} //Select
		Send {F13}
		Send {BS}
		g=1
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {F14}
		Send {BS}
		g=0
	}
Return
Space & BS::
	If g=0
	{
		Send {vk29} //Select
		Send {F13}
		Send {BS}
		g=1
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {F14}
		Send {BS}
		g=0
	}
Return
!BS::
	If g=0
	{
		Send {vk29} //Select
		Send {F13}
		Send {BS}
		g=1
	}
	Else
	{
		Send {vkF7} //CrSel
		Send {F14}
		Send {BS}
		g=0
	}
Return

Space & Tab::
Send {Tab}
Send {XButton1 down}
Keywait, TAB
Send {XButton1 up}
Return

Space & q::
Send {Numpad7 down}
Keywait, q
Send {Numpad7 up}
Return

Space & w::
Send {Numpad8 down}
Keywait, w
Send {Numpad8 up}
Return

Space & e::
Send {Numpad9 down}
Keywait, e
Send {Numpad9 up}
Return

Space & r::
Send {Numpad6 down}
Keywait, r
Send {Numpad6 up}
Return

Space & t::
Send {Numpad4 down}
Keywait, t
Send {Numpad4 up}
Return

u::
	If j=0
	{
		Send {vk29} //Select
		Send {F17}
		Send {u down}
		j=1
		k=0
		Keywait,  u
		Send {u up}
	}
	Else If j=1
	{
		Send {vkF7} //CrSel
		Send {F18}
		Send {u down}
		j=2
		k=1
		Keywait,  u
		Send {u up}
	}
	Else
	{
		Send {vkF8} //ExSel
		Send {F19}
		Send {F16}
		Send {u down}
		j=0
		k=0
		Keywait,  u
		Send {u up}
	}
Return
Space & u::
Send {u down}
Keywait, u
Send {u up}
Return
!u::
Send {u down}
Keywait, u
Send {u up}
Return

o::
Send {vk29} //Select
Send {F17}
Send {o down}
j=1
k=0
Keywait,  o
Send {o up}
Return
Space & o::
Send {o down}
Keywait, o
Send {o up}
Return
!o::
Send {o down}
Keywait, o
Send {o up}
Return

p::
Send {vk29} //Select
Send {vkF6} //Sysreq
Send {p down}
b=1
Keywait, p
Send {p up}
Return

!Capslock::
If e=0
{
	Send {vk29} //Select
	Send {NumpadAdd down}
	e=1
	Keywait, Capslock
	Send {NumpadAdd up}
}
Else
{
	Send {vkF7} //CrSel
	Send {NumpadAdd down}
	e=0
	Keywait, Capslock
	Send {NumpadAdd up}
}
Return

!a::
Keywait, Alt
Send {``}
Return

!s::
	If b=0
	{
		Send {vk29} //Select
		Send {vkF6} //Sysreq
		Send {p down}
		b=1
		Keywait, s
		Send {p up}
	}
	Else If b=1
	{
		Send {vkF7} //CrSel
		Send {vkFE} //Clear
		Send {; down}
		b=2
		Keywait, s
		Send {; up}
	}
	Else If b=2
	{
		Send {vkF8} //ExSel
		Send {vk2B} //Execute
		Send {. down}
		b=0
		Keywait, s
		Send {. up}
	}
Return

Space & d::
Send {Home down}
Keywait, d
Send {Home up}
Return

Space & f::
Send {PgUp down}
Keywait, f
Send {PgUp up}
Return

Space & g::
Send {Numpad5 down}
Keywait, g
Send {Numpad5 up}
Return

;::
Send {vkF7} //CrSel
Send {vkFE} //Clear
Send {; down}
b=2
Keywait, ;
Send {; up}
Return

Space & z::
Send {vkF8} //ExSel
Send {Numpad1}
Send {Numpad2}
Send {Numpad3}
Send {NumpadDot down}
Keywait, z
Send {NumpadDot up}
Return
!z::
	If c=0
	{
		Send {vkF8} //ExSel
		Send {Up down}
		c=1
		Keywait, z
		Send {Up up}
	}
	Else If c=1
	{
		Send {vkF8} //ExSel
		Send {Left down}
		c=2
		Keywait, z
		Send {Left up}
	}
	Else If c=2
	{
		Send {vkF8} //ExSel
		Send {Down down}
		c=3
		Keywait, z
		Send {Down up}
	}
	Else
	{
		Send {vkF8} //ExSel
		Send {Right down}
		c=0
		Keywait, z
		Send {Right up}
	}
Return

Space & x::
Send {Delete down}
Keywait, x
Send {Delete up}
Return

Space & c::
Send {End down}
Keywait, c
Send {End up}
Return

Space & v::
Send {PgDn down}
Keywait, v
Send {PgDn up}
Return

.::
Send {vkF8} //ExSel
Send {vk2B} //Execute
Send {. down}
b=0
Keywait, s
Send {. up}
Return

Space & XButton2::
Send {Ins down}
Keywait, XButton2
Send {Ins up}
Return

!WheelDown::
	If c=0
	{
		Send {vkF8} //ExSel
		Send {Up}
		c=1
	}
	Else If c=1
	{
		Send {vkF8} //ExSel
		Send {Left}
		c=2
	}
	Else If c=2
	{
		Send {vkF8} //ExSel
		Send {Down}
		c=3
	}
Return

!WheelUp::
	If c=1
	{
		Send {vkF8} //ExSel
		Send {Right}
		c=0
	}
	Else If c=2
	{
		Send {vkF8} //ExSel
		Send {Up}
		c=1
	}
	Else If c=3
	{
		Send {vkF8} //ExSel
		Send {Left}
		c=2
	}
Return
