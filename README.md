# A Minimalist Dual Modifier DotA 2 Keyboard Layout

### What is this?

The engine that "unlocks" the WINDOWS modifier key in DotA 2 and then binds it to Space. This gives you 2 modifiers for your keyboard layout: Alt and Space.
This is all achieved with only four lines of AHK code:

`#SingleInstance force`

`#IfWinActive ahk_exe dota2.exe`

`~*Space::ControlSend, , {LWin Down}, Dota 2`

`*Space Up::Send {LWin Up}`

This ultra short code blocks the Windows OS from seeing the Windows key presses being sent to dota2.exe.
Spacebar retains full functionality.

### How to set it up?

##To Install:

####Only Supported on Windows Operating Systems

1. Place D2SpaceModifier.exe in your StartUp folder (I have provided a shortcut).
   You can use the .ahk in the source code folder instead if you have installed AutoHotKey. https://autohotkey.com/

2. Pick a cool layout inside the keyboard layouts folder. I recommend my own.

3. Place the "Game" folder inside of "Steam\steamapps\common\dota 2 beta" and overwrite.

4. Place the "570" folder inside of "Steam\userdata\ [Your 9 digit Steam ID here] " and overwrite.

5. Restart your computer. D2SpaceModifier.exe will run in the background but will not be active unless
   you have DotA 2 running and are "alt-tabbed" into it.

6. launch DotA 2 and customize to your liking

####Note: The SPACE modifier reads "WINDOWS" in-game.

### How to uninstall?

1. Just delete the files you pasted in the `Game`, `570`, and `StartUp` folder.

2. Set your keybinds back to the way you want them

3. Enable double-tap for self-cast in the dota 2 options by raising the timeout to something around 0.5

*Note: Double tap selfcast is intentionally disabled to prevent miscasts*


### Known Valve Bugs:

1. Self-cast abilities which are auto-castable are self-casted when the auto-cast hotkey is pressed.
Only affects Ogre Magi, Lich, and neutral troll priest. This is a Valve bug and out of my control.

### Frequently Asked Questions

##### Why can't I double-tap to self-cast or change wards with double-tap?

That is intended behaviour in my super compact layout, to prevent miscasts. <kbd>Alt</kbd>+<kbd>Key</kbd> should do self-cast or switch wards. Furthermore, double tap self cast is impossible if you are using quick casts.

This was done by disabling the `dota_ability_self_cast_timeout` tolerance. The way "double-taping" works is by using the "smart-double tap" in-game option (hold space and press an item or ability's hotkey).

##### Can I rebind stuff in-game?

You can change the binds of EVERYTHING you see using the default in-game UI.

##### Why do some of my commands not trigger?

That must be because of your non-standard keyboard. You should consider changing your keyboard layout to standard English in Windows options.

##### Does <kbd>CAPSLOCK</kbd> still do what it usually does, even if it's a keybind?

Yes.


### Alternatives

1. ARCANA HOTKEYS II for Dota 2 - AVEYO's D-OPTIMIZER
   https://github.com/AveYo/D-OPTIMIZER.
   This is a great keyboard layout and one you'll probably prefer if you are missing the extra features that came with Loopuleasa's        TheCore. You do have to modify dota 2's .vpk files. So don't use it to create cheats, or you'll get VAC banned. If you want the   extra features, use Arcana Hotkeys. If all you want is two modifiers with minimal features then you use this project.


### Made a cool keyboard layout and want to share it?

Contact me and I might add it to the main repository for people to see, and I'll give you credit.
If you know how to use git, just fork this repo and request a merge using a pull request.
