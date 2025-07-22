# AudioStreamExt
A Godot Audio waveform extension written in C++ with the minivorbis library!!!


### SETUP ----
for basic use download a Release (demo or standalone) and put it into godot 4.4 (or newer hopefully if it doesnt break) 
and then in code do 

`var waveform = WaveformRenderer.new()`

`waveform.create("res://example.ogg")`

to make a waveform!!, be sure to check out the demo code for more fresh tips!!

### Compiling from source ----
simply open the terminal and clone the git or 
download the repository from github directly

and then to compile simply open the project folder as a whole in VS CODE
and type in 'SCONS' after that let the compilation do its magic and profit

_if any errors with godot cpp occur download the 4.4 branch from https://github.com/godotengine/godot-cpp/tree/4.4_
