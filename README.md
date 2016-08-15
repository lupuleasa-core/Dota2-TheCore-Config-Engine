### What is this?

Project Oracle is a custom keyboard setup for Dota 2 that makes use of two modifier keys for Windows users. SteamOS and other linux users already enjoy keyboard setups that utilize both their super key and alt as modifiers, whereas Windows users can only use one modifier. This is because Microsoft's Windows key shortcuts cannot be disabled entirely wihtout disabling the Windows key itself. To ensure that the user does not experience any of the shortcuts during gameplay while using the second modifier, a short (14 lines) one-to-one ahk key script runs in the background to block the Windows key shortcuts from the os while Dota 2 is the active window. The windows modifier is then bound to LAlt as many windows users choose to disable the windows key for all games. The original alt modifier is then rebound in-game to the spacebar. This gives you two modifiers: LAlt and Space.

Currently you have to press LCtrl-Tab instead Alt-Tab to switch out of dota 2. Since the script is only active when the dota 2 window is, you can press either Alt-Tab or LCtrl-Tab to switch back into the application.

At Project Oracle we believe that no player should gain an in-game advantage from his operating system. Project Oracle gives the same opportunities that SteamOS users have to windows players by making two modifiers viable.

### How to set it up?

1. Place either the 32-bit or the 64-bit .exe, depending upon your system, in your StartUp folder (I have provided a shortcut).
   You can use the .ahk instead if you have installed AutoHotKey. https://autohotkey.com/

2. Place the "Game" folder inside of "Steam\steamapps\common\dota 2 beta" and overwrite

3. Place the "570" folder inside of "Steam\userdata\ [Your 9 digit Steam ID here] " and overwrite

4. Reboot your computer or run the .exe you chose from step 1 manually


### How to uninstall?

1. Just delete the files you pasted in the `cfg` folder.

2. Set your keybinds back the way you want them

3. Run this command in the console to enable double-tap for self-cast (if it was affected by the setup in any way)

    `dota_ability_self_cast_timeout 0.5`

*Note: Double tap selfcast is intentionally disabled to prevent miscasts. This was done by making the tolerance so low only scripts can trigger it.*


### Frequently Asked Questions

##### Why can't I double-tap to self-cast or change wards with double-tap?

That is intended behaviour in my super compact layout, to prevent miscasts. <kbd>Alt</kbd>+<kbd>Key</kbd> should do self-cast or switch wards. Furthermore, double tap self cast is impossible if you are using quick casts.

This was done by disabling the `dota_ability_self_cast_timeout` tolerance. The way "double-taping" works is by using the "smart-double tap" in-game option (hold space and press an item or ability's hotkey).

##### Can I rebind stuff in-game?

You can change the bind of most things in-game but a few, you can only change by editing the `.cfg` files.

##### Why do some of my commands not trigger?

That must be because of your non-standard keyboard. You should consider changing your keyboard layout to standard English in Windows options.

##### How do I tell if I am running a 32 or 64 bit version of Windows?

Here is Microsoft's guide: https://support.microsoft.com/en-us/kb/827218

##### Can I bind keys like <kbd>Shift</kbd>, <kbd>Escape</kbd>, <kbd>Ctrl</kbd>, Ping Key (Default: <kbd>Alt</kbd>), <kbd>Mouse 1</kbd>, <kbd>Mouse 2</kbd> and <kbd>Mouse 3</kbd>?

No. Those are specially coded keys in the source engine that sadly aren't supported for modification.


##### Does <kbd>CAPSLOCK</kbd> still do what it usually does, even if it's a keybind?

CapsLock will always be off in dota. You can change this in the .ahk if you wish.

### How does it work?

The `autoexec.cfg` file is loaded and executed automatically by the dota2 client which acts as an engine which loads and combines other `.cfg` files into a flexible keyboard layout which is easy to modify.

Take a sniff through the files—there are plenty of explanatory comments in each `.cfg` file.

Each config file has its own role. Once you understand what they do, they're easy to tweak.

The AHK script is one-to-one and will NOT get you banned. It does not do anything that the SteamOS doesn't. All the AHK script does is block Windows from seeing the Windows key presses sent to dota2.exe.


### Made a cool keyboard layout and want to share it?

Contact me and I might add it to the main repository for people to see, and I'll give you credit.
If you know how to use git, just fork this repo and request a merge using a pull request.
