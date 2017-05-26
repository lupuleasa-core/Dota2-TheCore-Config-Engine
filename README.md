# DotA 2 Space Modifier:
## The Holy Grail of Triple Keyboard Layouts

### What is this?
The engine that "unlocks" the <kbd>WINDOWS</kbd> modifier key in DotA 2 and then binds it to <kbd>SPACE</kbd>. This gives you 2 modifiers for keyboard layouts: <kbd>ALT</kbd> and <kbd>SPACE</kbd>. Some of you will recall Loopuleasa's work which was broken by Valve's limits to the autoexec in the spring of 2016. This project is a fork from the former and restores most of its functionality with the addition of five lines of AHK code:

    #SingleInstance force
    #IfWinActive ahk_exe dota2.exe
    ~*Space::ControlSend, , {LWin Down}, Dota 2
    *Space Up::Send {LWin Up}
    ~*CapsLock Up::SetCapsLockState Off

**Simply put, this code hides all down key presses of the left windows key from the operating system and ALL applications EXCEPT dota2.exe.** <kbd>SPACE</kbd> retains full functionality. Believe it or not, this is all we need to make <kbd>SPACE</kbd> into a modifier just like <kbd>ALT</kbd>. This is because Valve already had the <kbd>WINDOWS</kbd> keys setup to work as modifiers. The only reason we can't use them is because of windows shortcuts "Alt-tabbing" us out of DotA.

### How to set it up?

## To Install:

#### Only Supported on Windows Operating Systems

1. Place D2SpaceModifier.exe in your StartUp folder, "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup".
   You can use the .ahk in the source code folder instead if you have AutoHotKey installed. https://autohotkey.com/

2. Place the autoexec.cfg inside of "Steam\steamapps\common\dota 2 beta\game\dota\cfg" and overwrite if necessary.

3. Pick a cool layout from the numbered folders or create your own. I recommend number 1 (my personal layout).

* [No Modifier](https://cloud.githubusercontent.com/assets/19603023/26277989/6f11ca28-3d57-11e7-8472-4e4ff262df8d.png)

* [Space Pressed](https://cloud.githubusercontent.com/assets/19603023/26277992/7bbc7a02-3d57-11e7-9e77-dd6ce634c991.png)

* [Alt Pressed](https://cloud.githubusercontent.com/assets/19603023/26277996/84c0ccf2-3d57-11e7-96bc-418a29a09293.png)

4. Place the "570" folder inside of "Steam\userdata\ [Your 9 digit Steam ID here] " and overwrite.

5. Restart your computer. D2SpaceModifier.exe will run in the background but will be inactive unless
   you have DotA 2 running and are "alt-tabbed" into it.

6. Launch DotA 2 and customize to your liking.

#### Note: The <kbd>SPACE</kbd> modifier reads "WINDOWS" in-game.

Behold the <kbd>SPACE</kbd> modifier in action!
![image](https://cloud.githubusercontent.com/assets/19603023/26280395/13386eb0-3d97-11e7-8611-253684316744.png)

### How to uninstall?

1. Just delete the files you pasted in the `Game`, `570`, and `StartUp` folder.

2. Set your keybinds back to the way you want them.

3. Enable double-tap for self-cast in the DotA 2 options by raising the timeout to something around 0.5.

*Note: Double tap selfcast is intentionally disabled to prevent miscasts*

### Known Valve Bugs:

1. Self-cast abilities which are auto-castable are self-casted when the auto-cast hotkey is pressed.
Only affects Ogre Magi, Lich, and neutral troll priest. This is a Valve bug and out of my control.

### Frequently Asked Questions

**Why can't I double-tap to self-cast or change wards with double-tap?**

That is intended behaviour in my super compact layout, to prevent miscasts. <kbd>Alt</kbd>+<kbd>Key</kbd> should do self-cast or switch wards. Furthermore, double tap self cast is impossible if you are using quick casts.

This was done by reducing the self cast timeout tolerance to 0. The way "double-taping" works is by using the "smart-double tap" in-game option (hold space and press an item or ability's hotkey).

**Can I rebind stuff in-game?**

You can change the binds of almost everything using the default in-game UI.

**Why do some of my commands not trigger?**

That must be because of your non-standard keyboard. You should consider changing your keyboard layout to standard English in Windows options.

**Does <kbd>CAPSLOCK</kbd> still do what it usually does, even if it's a keybind?**

No. Line 5 of the script disables <kbd>CAPSLOCK</kbd>. You can still bind stuff to it.

**How do I pick a modifier other than <kbd>SPACE</kbd>?**

You will need AHK installed (https://autohotkey.com/). Edit D2SpaceModifier.ahk in the "Source Code" folder by replacing the word "Space" on lines 3 and 4 with what ever key you want such as "x" for example. Here is a list of some AHK keys with unique names if you wish to bind to one of those (https://autohotkey.com/docs/KeyList.htm). Place your edited script in your StartUp folder instead of the exe (or compile a new exe yourself). Restart your computer and finish installation and tweaking as usual.

**Will this get me banned?**

Remember, all the script does is allow dota2.exe to see the windows key press while blocking it from the OS and every other application (only active while "Dota 2" is the active window). I haven't been banned and have played 1000+ games.

**Can't you just disable the windows key using registry edits?**

dota2.exe won't be able to see the key press then.

### Made a cool keyboard layout and want to share it?

Contact me on reddit at [u/karrandalf](https://www.reddit.com/message/compose/?to=Karrandalf) and I might add it to the main repository for people to see, and I'll give you credit.
If you know how to use git, just fork this repo and request a merge using a pull request.
