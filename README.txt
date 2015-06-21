////////////////////////////////////////////////////
//What is this
////////////////////////////////////////////////////
 
This is a system of .cfg files that control all aspects of the game.

Using this, you have full control over what volvo gave you in-game, meaning you can tweak every aspect of the game by modifying text files. You can do special functions and keybinds that are not possible otherwise from the in-game menu.
 
 
 
////////////////////////////////////////////////////
//How to set it up
////////////////////////////////////////////////////
 
1. Remove all in-game keybinds (by left clicking a bind, then right clicking if you are on Dota 1 Legacy Source 1 or by pressing a far away key like KP_MINUS if you are Dota 2 Reborn Source 2). Do this for all your binds.
Mine looks like this - http://i.imgur.com/ZMlrp16.png (the ALT+Key keybinds are actually Tilde+Key in my super compact layout, and they don't conflict; I just use them to have some labels on my items in-game)
 
2. Choose one of the keyboard layouts (found inside either the Dota2 Reborn or Legacy folders). You can Look in the "keyboard layout visual images" folder to see how the layout will be.

3. Copy the contents from INSIDE of the folder you chose to \Steam\SteamApps\common\dota 2 beta\game\dota\cfg\ (for Dota 2 Reborn) or \Steam\SteamApps\common\dota 2 beta\dota\cfg\ (for Dota 2 Legacy)  
 
4. You can modify the files however you want to match your preference, just make sure to read and understand what the files do. The main files that control the binds are the keybinds_default, keybinds_alt_pressed and keybinds_space_pressed.

5. Once the game loads, the autoexec.cfg gets executed automatically and you'll hear a sound. Alternatively, press (default) F8 in-game to reload it.

 
 
////////////////////////////////////////////////////
//Frequently Asked Questions
////////////////////////////////////////////////////
 
- Do I have to remove ALL the in-game keybinds?

You can have a few keybinds there, but they will overwrite what are in the external .cfg files so I cannot guarantee there will be no conflicts.

- It says the Alt key was remapped to Tilde, but I still have to press Alt+Click on items and abilities to type in chat. Why so?

That is an issue on volvo's part, because their dota_remap_alt_key command isn't 100% working, it mostly just moves the map ping functionality, and the in-game ALT+Key commands (which become Tilde+Key commands). 

- Did you have to remap the ALT key to something like the Tilde key?

Yes, because otherwise Alt+Key commands would've only worked if you configured them in-game and I had less control over the layout from the files.

- My keyboard is different, and my Tilde key is in a different place. How do I change it?

Just go into the autoexec.cfg engine file and change the "dota_remap_alt_key" command and put the key you want to be your secondary "ALT" modifier

- My in-game item/ability labels are blank, can't I put the keys there?

From the files you can't really do that, but because the ALT key was remapped, you can put ALT+QWER/DFXCMouse5 on each keybind, and you will have "Alt+Q", "Alt+W"... displayed in-game, even though those are actually (Tilde+Q, Tilde+W...) and no conflicts will exist. I do it like that just as a visual hack to see my key.
 
- The quick attack and quick follow commands don't seem to work for me. Why?

You must be running dota on a mac or Linux, where +sixense_left_click doesn't really work as a normal click.
 
 
 
////////////////////////////////////////////////////
//How it works?
////////////////////////////////////////////////////
 
I turned the autoexec.cfg file that is loaded and executed automatically by the dota2 client into an engine that loads and combines the other .cfg files I created into a flexible keyboard layout that is easy to modify once you get the hang of it. 

Take a sniff through files, because I left many readme files laying around and in each .cfg files there are usually plenty of comments.

Each config file has its role.



////////////////////////////////////////////////////
//Made a cool keyboard layout and want to share it?
////////////////////////////////////////////////////

Contact me on reddit at /u/loopuleasa and I might add it to the main repository for people to see and I shall give you credit.
