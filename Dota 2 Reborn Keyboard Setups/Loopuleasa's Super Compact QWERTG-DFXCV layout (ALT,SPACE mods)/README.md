#Loopuleasa's Super Compact QWERTG-DFXCV

This keyboard layout is what I currently use and I'm constantly tweaking it until I get an optimal layout.
Each file has a role, and modify what you want in it.
These aren't hacks, nor they are unfair, since one game action translates to one keystroke.
It's just efficient.

####Now for Dota 2 Reborn!
Some functions do not work in Source 2, like:
- left clicks
- console output
- playing hero/item sounds
- netgraph
- toggle health segmentation (command still there, toggle is not)

**NEW** Functions:
- hold right click to issue move
- Tilde + Right Click = move directly / face that way

Supported customization:
- Alt as modifier key
- Space as modifier key
- Quickcast/Normal cast mode toggle
- Hero custom modes
- Split config files structure, for ease of editing
- Really compact keybind reachable by your hand

##Autoexec engine

```
autoexec.cfg
```
- Manages the core functionality of the other files. It's loaded automatically by dota2. 
```
game_convars.vcfg
```
- Changed a bit to load autoexec.cfg automatically like in Legacy

##dota2_gameplay_mode folder

This folder contains all the .cfg files that are loaded
and they control the game settings and binds as follows:

```
dota2_functions_active.cfg
```
- This file holds all the aliases and custom defined functions made by the use.

```
dota2_keybinds_default.cfg
dota2_keybinds_space_pressed.cfg
dota2_keybinds_alt_pressed.cfg
```
- These three files are full of "bind" commands which connect one key on your keyboard to a function or command.
- Each one of them is responsible for setting the keybinds for either default mode (no ALT or SPACE key pressed), alt mode (ALT+Key) or space mode (SPACE+Key).
- Basically, when you press ALT, the keys from dota2_keybinds_alt_pressed are active, and when SPACE is pressed, the keys from dota2_keybinds_space_pressed are active.

```
dota2_settings_game.cfg
```
- This file contains all the game tweaks and settings (minimap, camera, various toggles, and many options related directly to the game)

```
dota2_settings_tech.cfg
```
- This file contains all the graphical, visual and technical options that are more related to the computer that is running dota2 and not the game itself.

```
dota2_keybinds_toggle_quickcast
dota2_keybinds_toggle_non-quickcast
```
- These two files are loaded depending on which mode is enabled by my engine(quickcast on or off). You shouldn't modify them, but you can use the aliases defined inside for your pleasure.

##dota2_hero_custom_configs folder

Inside this folder, you will have hero custom configs that 
you can load by typing a command in the console. 
They're used to add custom keybinds that were designed 
specifically for a single hero. 
Check the readme inside the folder on how to use them.

##dota2_caster_mode folder

This is something I'll work in the future to aid casters.
