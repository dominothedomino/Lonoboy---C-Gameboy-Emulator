Lonoboy - A Gameboy Emulator made fully in C++!
This is a personal project I undertook in order to better teach myself C++, as well as how a game console functions.
The emulator is run through console, where the syntax is as follows:
gameboy.exe <rom path>

Currently the emulator is well tested with Tetris, Pokemon Fire, and The Legend of Zelda, which I deemed the minimum requirements for my emulator
to be considered "working" as the games test the CPU, PPU, APU, and the additional memory banks (MBC's).
There are a few noticeable graphical glitches that I might continue to fix, but for now I will take a break for a week or two on development.
The APU seems to be working good, but it sounds a bit grainy. I suspect this might just be a gameboy probelm, but I coded a filter in pushSample(),
so if you want to change how the sound SOUNDS (grainy or muffled), I recommend changing the coeff variable to fine tune the sound system to your liking.
Additionally, you might try to change how many frames worth of samples the pushSample() function can actually take, but I found this increased my audio
lag by a bit.

If you have any issues, let me know and I would love to continue supporting this project!
