##What does this do?

It creates a modifier key (like for instance you can use SPACE+Key now, or even another key of your choice). You can use the code here to create as many layers of this kind as you want.

##How to set it up?

- Remove all in-game commands.

- Place your standard binds in the "default_layer.cfg".

- Set up a modifier key to the "+keyShift" command

- Place the binds you want to load on the modifier key press in the "layer_1.cfg"

- In your autoexec file, exec the "aliases.cfg" and the "default_layer.cfg".

##Issues:

- not all keys work (like Ctrl, Shift, Alt, Escape, Mouse 1,2)
- the "unbindall" command removes all in-game commands (so if you want to keep the ones you have in-game, remove that line, but be careful of conflicts)
