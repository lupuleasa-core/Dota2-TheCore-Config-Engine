This keyboard layout is barebones, meaning that you should modify it however you want.

It has the entire core structure setup, and all you have to do is edit it to your tastes.

Each file has a role, and modify what you want in it.


-------------------
File Structure
-------------------

- autoexec.cfg

Manages the core functionality of the other files. It's loaded automatically by dota2.


- dota2_functions_active.cfg

This file holds all the aliases and custom defined functions made by the use.


- dota2_keybinds_default.cfg
- dota2_keybinds_space_pressed.cfg
- dota2_keybinds_alt_pressed.cfg

These three files are full of "bind" commands which connect one key on your keyboard to a function or command.
Each one of them is responsible for setting the keybinds for either default mode (no ALT or SPACE key pressed), alt mode (ALT+Key) or space mode (SPACE+Key).
Basaically, when you press ALT, the keys from dota2_keybinds_alt_pressed are active, and when SPACE is pressed, the keys from dota2_keybinds_space_pressed are active.


- dota2_settings_game.cfg

This file contains all the game tweaks and settings (minimap, camera, various toggles, and many options related directly to the game)


- dota2_settings_tech.cfg

This file contains all the graphical, visual and technical options that are more related to the computer that is running dota2 and not the game itself persay.


- dota2_hero_custom_configs folder

Inside this folder, you will have hero custom configs that you can load by typing a command in the console. They're used to add custom keybinds that were designed for a single hero. Check the readme inside the folder on how to use them.