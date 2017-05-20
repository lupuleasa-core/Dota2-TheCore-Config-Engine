# DotA 2 Space Modifier:
## The Holy Grail of Triple Keyboard Layouts

### What is this?
The engine that "unlocks" the WINDOWS modifier key in DotA 2 and then binds it to the spacebar. This gives you 2 modifiers for your keyboard layouts: Alt and Space.
This is all achieved with only four lines of AHK code:

> #SingleInstance force

> #IfWinActive ahk_exe dota2.exe

> ~*Space::ControlSend, , {LWin Down}, Dota 2

> *Space Up::Send {LWin Up}

**Simply put, this code hides all down key presses of the left windows key from ALL applications EXCEPT dota2.exe.** Spacebar retains full functionality.

### How to set it up?

## To Install:

#### Only Supported on Windows Operating Systems

1. Place D2SpaceModifier.exe in your StartUp folder, "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup".
   You can use the .ahk in the source code folder instead if you have installed AutoHotKey. https://autohotkey.com/

2. Place the autoexec.cfg inside of "Steam\steamapps\common\dota 2 beta\game\dota\cfg" and overwrite if necessary.

3. Pick a cool layout from the numbered folders. I recommend number 1 (my own).

4. Place the "570" folder inside of "Steam\userdata\ [Your 9 digit Steam ID here] " and overwrite.

5. Restart your computer. D2SpaceModifier.exe will run in the background but will be inactive unless
   you have DotA 2 running and are "alt-tabbed" into it.

6. Launch DotA 2 and customize to your liking.

#### Note: The SPACE modifier reads "WINDOWS" in-game.

### How to uninstall?

1. Just delete the files you pasted in the `Game`, `570`, and `StartUp` folder.

2. Set your keybinds back to the way you want them.

3. Enable double-tap for self-cast in the dota 2 options by raising the timeout to something around 0.5.

*Note: Double tap selfcast is intentionally disabled to prevent miscasts*

### Known Valve Bugs:

1. Self-cast abilities which are auto-castable are self-casted when the auto-cast hotkey is pressed.
Only affects Ogre Magi, Lich, and neutral troll priest. This is a Valve bug and out of my control.

### Frequently Asked Questions

##### Why can't I double-tap to self-cast or change wards with double-tap?

That is intended behaviour in my super compact layout, to prevent miscasts. <kbd>Alt</kbd>+<kbd>Key</kbd> should do self-cast or switch wards. Furthermore, double tap self cast is impossible if you are using quick casts.

This was done by reducing the self cast timeout tolerance to 0. The way "double-taping" works is by using the "smart-double tap" in-game option (hold space and press an item or ability's hotkey).

##### Can I rebind stuff in-game?

You can change the binds of almost everything using the default in-game UI.

##### Why do some of my commands not trigger?

That must be because of your non-standard keyboard. You should consider changing your keyboard layout to standard English in Windows options.

##### Does <kbd>CAPSLOCK</kbd> still do what it usually does, even if it's a keybind?

Yes.

### Made a cool keyboard layout and want to share it?

Contact me and I might add it to the main repository for people to see, and I'll give you credit.
If you know how to use git, just fork this repo and request a merge using a pull request.
