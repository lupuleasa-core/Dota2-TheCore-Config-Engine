It really is just a simple four lines.

#*	

Wildcard: Fire the hotkey even if extra modifiers are being held down. This is often used in conjunction with remapping keys or buttons. For example:

`*#c::Run Calc.exe  ; Win+C, Shift+Win+C, Ctrl+Win+C, etc. will all trigger this hotkey.`

`*ScrollLock::Run Notepad  ; Pressing ScrollLock will trigger this hotkey even when modifier key(s) are down.`

Wildcard hotkeys always use the keyboard hook, as do any hotkeys eclipsed by a wildcard hotkey. For example, the presence of *a:: would cause ^a:: to always use the hook.
