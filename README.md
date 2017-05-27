# DotA 2 Space Modifier: The Sacred Five Lines

### What is this?
The engine that "unlocks" the **hidden** <kbd>WINDOWS</kbd> modifier key in DotA 2 and then binds it to <kbd>SPACE</kbd>. This gives you two modifiers for keyboard layouts: <kbd>ALT</kbd> and <kbd>SPACE</kbd>. Some of you will recall [Loopuleasa's work](https://github.com/loopuleasa/Dota2-TheCore-Config-Engine) which was broken by Valve's limits to the autoexec in the spring of 2016. This project is a fork of the former and restores most of its functionality by utilizing five lines of AHK code (and two optional lines):

    ; 1. **Highly Recommended**
    ; *Documentation: https://autohotkey.com/docs/commands/_SingleInstance.htm *
    ; Prevents more than one instance of this script from running by closing the old instance when a new one is ran.
    #SingleInstance force 
    
    ; 2. **Highly Recommended**
    ; *Documentation: https://autohotkey.com/docs/commands/_IfWinActive.htm *
    ; Makes the script only active while you are "Alt-tabbed" into DotA 2.
    #IfWinActive ahk_exe dota2.exe 

    ; 3. **Required**
    ; *Documentation: https://autohotkey.com/docs/Hotkeys.htm *
    ; *Documentation: https://autohotkey.com/docs/commands/ControlSend.htm *
    ; Hides all down key presses of the left Windows key from the operating system
    ; and ALL applications EXCEPT dota2.exe.
    ~*Space::ControlSend, , {LWin Down}, Dota 2 
    
    ; 4. **Required**
    ; *Documentation: https://autohotkey.com/docs/commands/Send.htm *
    ; Allows all releases of the left Windows key to be seen by the operating system
    ; and ALL applications INCLUDING dota2.exe.
    *Space Up::Send {LWin Up}
    
    ; 5. **Highly Recommended**
    ; *Documentation: https://autohotkey.com/docs/commands/SetNumScrollCapsLockState.htm *
    ; Upon releasing the CapsLock key, the CapsLock State is set to off. 
    ; It prevents a conflict due to the Windows key and CapsLock interaction.
    ; This method allows you to still use the CapsLock key as a hotkey in the in-game UI.
    ~*CapsLock Up::SetCapsLockState Off
    
    ; 6. **Optional**
    ; Binds the Unified Command Modifier to Mouse5 while inside DotA 2 inorder to improve accessibility.
    ; Overrides other mouse binding software.
    *XButton2::Ctrl

    ; 7. **Optional**
    ; Disables the left Windows key while inside of DotA 2.
    *LWin::Return
    
**Simply put, this code hides all down key presses of the left Windows key from the operating system and ALL applications EXCEPT** `dota2.exe`**.** <kbd>SPACE</kbd> retains full functionality. These five lines are all we need to make <kbd>SPACE</kbd> into a modifier just like <kbd>ALT</kbd>. This is because Valve already had the <kbd>WINDOWS</kbd> keys setup to work as modifiers. The only reason we couldn't use them before is because of Windows shortcuts "Alt-tabbing" us out of DotA. **The Sacred Five Lines** fix this issue.

## To Install

*Only Supported on Windows Operating Systems*

1. Place `D2SpaceModifier.exe` (the one with the SPACE icon) in your StartUp folder, "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup". You can use the .ahk instead if you have AutoHotKey installed. https://autohotkey.com/

*Note: The compiled `.exe` is only included in releases. If you don't see the executable, then you have downloaded the lastest unreleased version and have to use the `.ahk` script instead (or compile an `.exe` yourself).*

2. Place the `autoexec.cfg` inside of "Steam\steamapps\common\dota 2 beta\game\dota\cfg" and overwrite if necessary.

3. Select a layout from the numbered folders or create your own. I recommend number one ([my personal layout](https://github.com/Karrandalf/D2SpaceModifier/tree/master/1.%20Karrandalf's%20Keyboard%20Layout)).

4. Place the "570" folder inside of "Steam\userdata\[Your 9 digit Steam ID here]" and overwrite.

5. Restart your computer. `D2SpaceModifier.exe` will run in the background but will be inactive unless
   you have DotA 2 running and are "Alt-tabbed" into it.

6. Launch DotA 2 and customize to your liking.

*Note: The <kbd>SPACE</kbd> modifier reads <kbd>WINDOWS</kbd> in-game.*

Behold our free <kbd>SPACE</kbd> modifier!
![image](https://cloud.githubusercontent.com/assets/19603023/26280395/13386eb0-3d97-11e7-8611-253684316744.png)

## To Uninstall

1. Delete the files you pasted in the "Game", "570", and "StartUp" folders.

2. Set your keybinds back to the way you want them.

## Known Bugs

1. Self-cast abilities which are auto-castable are self-casted when the auto-cast hotkey is pressed.
Only affects Ogre Magi, Lich, and neutral troll priest. This is a **Valve bug** and out of my control.

## Frequently Asked Questions

**Why can't I double-tap to self-cast or change wards with double-tap?**

That is intended behaviour in my super compact layout, to prevent miscasts. <kbd>Alt</kbd>+<kbd>Key</kbd> should do self-cast or switch wards. Furthermore, double tap self cast is impossible if you are using quick casts.

This was done by reducing the self cast timeout tolerance to zero. The way "double-taping" works is by using the "smart-double tap" in-game option (hold <kbd>Alt</kbd> and press an item or ability's hotkey).

**Can I rebind stuff in-game?**

You can change the binds of almost everything using the default in-game UI.

A few keys require you to edit `user_keys.vcfg` in the "Steam\userdata\[Your 9 digit Steam ID here]\570" folder.

If you want to change the bind of <kbd>MOUSE5</kbd>, you have to edit the sixth line of `D2SpaceModifier.ahk`. 

**Why is Roshan quiet?**

It is easier to hear enemy units while you are killing Rosh. You can change this by editing the first line of `autoexec.cfg` to

    dota_silent_roshan	0

**Why do some of my commands not trigger?**

That must be because of your non-standard keyboard. You should consider changing your keyboard layout to standard English in Windows options.

**Does <kbd>CAPSLOCK</kbd> still do what it usually does, even if it's a keybind?**

No, the fifth line of the script disables <kbd>CAPSLOCK</kbd>. You can still use it as a hotkey in the in-game UI.

**How do I pick a modifier other than <kbd>SPACE</kbd>?**

You will need AHK installed (https://autohotkey.com/). Edit `D2SpaceModifier.ahk` by replacing the word "Space" in the third and fourth lines with what ever key you want such as "x" for example (here is a list of some AHK keys with unique names: https://autohotkey.com/docs/KeyList.htm). Place your edited script in your StartUp folder instead of the `.exe` (or compile a new executable yourself). Restart your computer and finish installation and tweaking as usual.

**Will this get me banned?**

Remember, all the script does is allow dota2.exe to see the Windows key press while blocking it from the OS and every other application (only active while "Dota 2" is the active window). I have played 1000+ games and haven't been banned and won't be because there isn't anything in my script that is flagged as a cheat. There are AHK scripts that will get you banned, but that is not this script.

**Do you personally use the** `.ahk` **or** `.exe`**?**

I used the AHK script during development and the executable in post-development or long-term testing.

**Can you just disable the Windows key using registry edits?**

The dota2.exe will not be able to see the key press then.

## Made a cool keyboard layout and want to share it?

Contact me on reddit at [u/karrandalf](https://www.reddit.com/message/compose/?to=Karrandalf), and I might add it to the main repository crediting you.
If you know how to use git, just fork this repo and request a merge using a pull request.
