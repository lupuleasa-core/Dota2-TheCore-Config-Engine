//////////////////////////////////////////////////////
//What is in this folder?
//////////////////////////////////////////////////////

This folder is full of config files for hero custom modes. 

You can set custom hero functions, and set keybinds for them in the config files 
(depending on if you want them to be when alt, space or no modifier is pressed)



//////////////////////////////////////////////////////
//How to activate the configs
//////////////////////////////////////////////////////

Open up the console when the game starts, and type in something of the form:

<hero_name>_mode

like so:

templar_assassin_mode
lone_druid_mode

and hit Enter.

Now the configs for the said hero have been loaded from the respective files if they have been created beforehand.

Note: This must be done whenever dota2 starts, and I recommend doing it at game start.


//////////////////////////////////////////////////////
//How to modify the hero custom configs
//////////////////////////////////////////////////////

just look in the files and add scripts that you wish inside

File structure:

hero_name_functions  ->  here you place all aliases and custom configs relevant to that hero

hero_name_binds_nomod  ->  here you place all the "bind" commands that set the functionality of your keys (with no modifier keys pressed)

hero_name_binds_space  ->  here you place all the "bind" commands that set the functionality of your keys (with SPACE key pressed)

hero_name_binds_alt  ->  here you place all the "bind" commands that set the functionality of your keys (with ALT key pressed)



//////////////////////////////////////////////////////
//How to setup files for a hero you want
//////////////////////////////////////////////////////

If the files don't exist for a hero you need custom configs, then copy the 4 blank files called

hero_nohero_binds_nomod.cfg
hero_nohero_binds_space.cfg
hero_nohero_binds_alt.cfg
hero_nohero_functions.cfg

and rename the files to replace the "nohero" part with your hero (like "earth_spirit")

in the end you'll have 4 files like

hero_earth_spirit_binds_nomod.cfg
hero_earth_spirit_binds_space.cfg
hero_earth_spirit_binds_alt.cfg
hero_earth_spirit_functions.cfg

Now, you need to go into the setup_hero_mode.cfg

There you should copy the nohero line and then use Ctrl-H to find and replace all instance in that line of "nohero" with "earth_spirit"

in the end you'll have a new line there looking like:

alias earth_spirit_mode "alias load_current_hero_nomod_binds execifexists dota2_hero_custom_configs/hero_earth_spirit_binds_nomod.cfg;alias load_current_hero_space_binds execifexists dota2_hero_custom_configs/hero_earth_spirit_binds_space.cfg;alias load_current_hero_alt_binds execifexists dota2_hero_custom_configs/hero_earth_spirit_binds_alt.cfg;alias load_current_hero_functions execifexists dota2_hero_custom_configs/hero_earth_spirit_functions.cfg;exec autoexec.cfg"


Now load it in-game, by typing "earth_spirit_mode" in console