//////////////////////////////////////////////////////
//What is in this folder?
//////////////////////////////////////////////////////

This folder is full of config files for hero custom modes. 

You can set custom hero functions, and set keybinds for them in the config files 
(depending on if you want them to be when alt, space or no modifier is pressed)



//////////////////////////////////////////////////////
//How to activate the configs
//////////////////////////////////////////////////////

1. Open up the console when the game starts, and type in something of the form:

	<hero_name>_mode

	like so:

	templar_assassin_mode
	lone_druid_mode

2. Hit Enter.

	Now the configs for the said hero have been loaded from the respective files.
	
3. When you are done using those configs, you can type in console "nohero_mode" to remove any leftover commands. 



//////////////////////////////////////////////////////
//How to modify the hero custom configs
//////////////////////////////////////////////////////

1. Look in the files for the hero you want to tweak. If the hero files don't exist, skip to the next section.

2. Add scripts that you wish inside the files, which function as follows:

hero_name_functions  ->  here you place all aliases and custom configs you set

hero_name_binds_nomod  ->  here you place all the "bind" commands that set the functionality of your keys (with no modifier keys pressed)

hero_name_binds_space  ->  here you place all the "bind" commands that set the functionality of your keys (with SPACE key pressed)

hero_name_binds_alt  ->  here you place all the "bind" commands that set the functionality of your keys (with ALT key pressed)



//////////////////////////////////////////////////////
//How to setup files for a hero you want
//////////////////////////////////////////////////////

1. If the files don't exist for a hero you need custom configs, then copy the 4 blank files called

hero_nohero_binds_nomod.cfg
hero_nohero_binds_space.cfg
hero_nohero_binds_alt.cfg
hero_nohero_functions.cfg

2. Rename the files to replace the "nohero" part with your hero (like "earth_spirit" or "bat_rider")
in the end you'll have 4 files like:

hero_earth_spirit_binds_nomod.cfg
hero_earth_spirit_binds_space.cfg
hero_earth_spirit_binds_alt.cfg
hero_earth_spirit_functions.cfg

3. Now, you need to go into the setup_hero_mode.cfg and you should copy any of the already available setups, like for templar_assassin for instance. 
Copy it at the end of the setup file.

4. Then use Ctrl-H to find and replace all instance in that line of "templar_assassin" with "earth_spirit"
in the end you'll have a new line there looking like:

alias earth_spirit_mode "alias load_current_hero_nomod_binds execifexists dota2_hero_custom_configs/hero_earth_spirit_binds_nomod.cfg;alias load_current_hero_space_binds execifexists dota2_hero_custom_configs/hero_earth_spirit_binds_space.cfg;alias load_current_hero_alt_binds execifexists dota2_hero_custom_configs/hero_earth_spirit_binds_alt.cfg;alias load_current_hero_functions execifexists dota2_hero_custom_configs/hero_earth_spirit_functions.cfg;refresh_mode;earth_spirit_sound"

5. Add scripts that you wish inside the files you've just made.

5. Now load it in-game, by typing "earth_spirit_mode" in console.

Optional: You can also add a "PlaySound" that can play when you load the respective hero mode, if you want. You can type when you joined a lobby with the hero you want in console try "PlaySound" and hit Tab to see available sound codes.