### What is this?

This is a system of `.cfg` files that controls almost all aspects of the game.

Using this, you have full control over what Valve lets you configure: you can tweak every aspect of the game by modifying text files, and also create special functions and keybinds that are not possible to make from the in-game menu.

Easy to share, easy to customise.


### How to set it up?

1. **Remove _all_ in-game keybinds.**  Do this by left clicking on a bind, then click the `X` that appears. *If you want to be safe, press <kbd>F12</kbd> in-game to save a screenshot of your binds so you don't forget them if you ever wish to go back and uninstall my engine.*

2. Choose one of the keyboard layouts (found inside either the Dota2 Reborn folder). You can look in the __keyboard layout visual images__ folder to see how the layout will be. I *recommend* using "loopuleasa's super compact layout" since it's the one we've worked the most on. If you are experienced, you can take the Dota2 Core and start building your layout from scratch.

3. Copy the contents **from inside of the folder** you've chosen to `\Steam\SteamApps\common\dota 2 beta\game\dota\cfg\`. *Keep the file structure you see inside the folder as it is.*  
**There are two `dota\cfg` folders, one is in the `game` subfolder, one is not, make sure not to confuse them.**

4. You can modify the files however you want to match your preference, but make sure to read and understand what the files do. There are plenty of comments inside them. The main files which control the binds are the `keybinds_default`, `keybinds_alt_pressed` and `keybinds_space_pressed`.

5. Once the game loads, the autoexec.cfg will be executed automatically and you'll hear a Hand of Midas sound. Alternatively, press <kbd>F8</kbd> in-game to reload it or force a reload by opening up the console and typing `exec autoexec.cfg`.


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

##### Do I have to remove ALL the in-game keybinds?

You can keep a few keybinds but they will override the `.cfg` files, but I cannot guarantee there will be no conflicts. Be careful.

##### Why do some of my commands, like <kbd>Tilde</kbd> pinging, not trigger?

That must be because of your non-standard keyboard. There is a maintained German keyboard layout you should try if you are German, if not you should consider changing your keyboard layout to standard English in Windows options.

For Tilde pinging, some exotic codes for that key are not recognized by the Source engine. Right now, Tilde on English keyboards is `` ` ``. If you type your Tilde key and it doesn't look like that then you know why it isn't working.

##### It says the Alt key was remapped to Tilde. Why do I still have to press <kbd>Alt</kbd>+<kbd>Click</kbd> on items and abilities to show their status in chat?

That is an issue on Valve's part, as their `dota_remap_alt_key` command isn't 100% complete and mostly just moves the map ping functionality, and the in-game <kbd>ALT</kbd>+<kbd>Key</kbd> commands (which become <kbd>Tilde</kbd>+<kbd>Key</kbd> commands).

##### Did you have to remap the <kbd>ALT</kbd> key to something like the Tilde key?

Yes, because otherwise <kbd>Alt</kbd>+<kbd>Key</kbd> commands would've only worked if you configured them in-game and I would have had less control over the layout from the files. Furthermore, some functionalities, like custom functions, couldn't be bound to <kbd>ALT</kbd>+<kbd>Key</kbd> if I didn't move the ping key. It's because of the source engine.

##### Can I bind keys like <kbd>Shift</kbd>, <kbd>Escape</kbd>, <kbd>Ctrl</kbd>, Ping Key (Default: <kbd>Alt</kbd>), <kbd>Mouse 1</kbd>, <kbd>Mouse 2</kbd> and <kbd>Mouse 3</kbd>?

No. Those are specially coded keys in the source engine that sadly aren't supported for modification.

##### My keyboard is different, and my Tilde key is in a different place. How do I change it?

Just go into the `autoexec.cfg` file and change the `dota_remap_alt_key` command to reflect the key that you want to be your secondary "ALT" modifier.

##### My in-game item/ability labels are blank, can't I put the keys there?

From the files you can't really do that, but there is a visual hack:

- You can put <kbd>ALT</kbd>+<kbd>QWER/DFXCMouse5</kbd> or whatever the keys are on each keybind, and you will have <kbd>Alt</kbd>+<kbd>Q</kbd>", <kbd>Alt</kbd>+<kbd>W</kbd>"... displayed in-game, even though those are actually (<kbd>Tilde</kbd>+<kbd>Q</kbd>, <kbd>Tilde</kbd>+<kbd>W</kbd>...) and no conflicts will exist. I do it like that just as a visual hack to see my key.
Mine [looks like this](http://i.imgur.com/ZMlrp16.png) (the <kbd>ALT</kbd>+<kbd>Key</kbd> keybinds are actually <kbd>Tilde</kbd>+<kbd>Key</kbd> in my super compact layout, and they don't conflict; I just use them to have some labels on my items in-game)

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
