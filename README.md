### What is this?

This is a system of `.cfg` files that controls almost all aspects of the game.

Using this, you have full control over what Valve lets you configure: you can tweak every aspect of the game by modifying text files, and also create special functions and keybinds that are not possible to make from the in-game menu.

Easy to share, easy to customise.


### How to set it up?

1. **Remove _most_ in-game keybinds.** Each individual contributer's readme will tell how to setup your in-game binds. You can remove a bind by left clicking it, then click the `X` that appears. *If you want to be safe, press <kbd>F12</kbd> in-game to save a screenshot of your binds so you don't forget them if you ever wish to go back and uninstall my engine.*

2. Choose one of the keyboard layouts (found inside the Dota2 Vulkan folder). You can look in the __keyboard layout visual images__ folder to see how the layout will be. I *recommend* using "loopuleasa's super compact layout" since it's the one we've worked the most on. If you are experienced, you can take the Dota2 Core and start building your layout from scratch.

3. Copy the contents **from inside of the folder** you've chosen to `\Steam\SteamApps\common\dota 2 beta\game\dota\cfg\`. *Keep the file structure you see inside the folder as it is.*  
**There are two `dota\cfg` folders, one is in the `game` subfolder, one is not, make sure not to confuse them.**

4. You can modify the files however you want to match your preference, but make sure to read and understand what the files do. There are plenty of comments inside them. The main files which control the binds are the `dota2_keybinds.cfg` and `dota2_keybinds.ahk`.

5. Once the game loads, the autoexec.cfg will be executed automatically and you'll hear a Hand of Midas sound.


### How to uninstall?

1. Just delete the files you pasted in the `cfg` folder.

2. Set your keybinds back the way you want them

3. Run this command in the console to enable double-tap for self-cast (if it was affected by the setup in any way)

    `dota_ability_self_cast_timeout 0.5`

*Note: Double tap selfcast is intentionally disabled to prevent miscasts. This was done by making the tolerance so low only scripts can trigger it.*


### Frequently Asked Questions

##### Why can't I double-tap to self-cast or change wards with double-tap?

That is intended behaviour in my super compact layout, to prevent miscasts. <kbd>Alt</kbd>+<kbd>Key</kbd> should do self-cast or switch wards. Furthermore, double tap self cast is impossible if you are using quick casts.

This was done by lowering the `dota_ability_self_cast_timeout` tolerance from `0.5` to something low like `0.01`. So low that only scripts can trigger it.

##### Why do some of my commands, like <kbd>Tilde</kbd> pinging, not trigger?

That must be because of your non-standard keyboard. There is a maintained German keyboard layout you should try if you are German, if not you should consider changing your keyboard layout to standard English in Windows options.

For Tilde pinging, some exotic codes for that key are not recognized by the Source engine. Right now, Tilde on English keyboards is `` ` ``. If you type your Tilde key and it doesn't look like that then you know why it isn't working.


##### Can I bind keys like <kbd>Shift</kbd>, <kbd>Escape</kbd>, <kbd>Ctrl</kbd>, <kbd>Mouse 1</kbd>, and <kbd>Mouse 2</kbd>?

No. Those are specially coded keys in the source engine that sadly aren't supported for modification.


##### Does <kbd>CAPSLOCK</kbd> still do what it usually does, even if it's a keybind?

YES OF COURSE, BUT IT'S A KEY IN SUCH A USEFUL POSITION THAT YOU MIGHT AS WELL USE IT.  Alternatively, you may change the keybind, or even [change how windows handles that pesky button atogether](http://www.howtogeek.com/194705/how-to-disable-or-reassign-the-caps-lock-key-on-any-operating-system/).  
__Note that if you remap <kbd>CAPSLOCK</kbd> in windows, you will also need to do so in your dota config.__


### How does it work?

The `autoexec.cfg` file is loaded and executed automatically by the dota2 client which acts as an engine which loads and combines other `.cfg` files into a flexible keyboard layout which is easy to modify.

Take a sniff through the files—there are plenty of explanatory comments in each `.cfg` file.

Each config file has its own role. Once you understand what they do, they're easy to tweak.


### Made a cool keyboard layout and want to share it?

Contact me on reddit at [/u/loopuleasa](https://www.reddit.com/message/compose/?to=loopuleasa) and I might add it to the main repository for people to see and I'll give you credit.
If you know how to use git, just fork this repo and request a merge using a pull request.
