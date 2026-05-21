#include "gameboy.h"
#include <iostream>

int main(){
    //2x scaled window
    Platform platform("Gameboy", 320, 288, VIDEOWIDTH, VIDEOHEIGHT);
    gameboy gb;
    gb.logFile.open("gameboy.log");
    gb.loadROM("C:\\Users\\goldf\\Downloads\\Tetris.gb");

    //passed 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11
    bool quit = false;
    while(!quit){
        quit = platform.processInput(gb.actionBut, gb.dirBut);
        gb.updateJoypad(gb.actionBut, gb.dirBut);

        uint16_t cycles = gb.Cycle();
        gb.updateTimers(cycles);
        gb.updatePPU(cycles);
        gb.handleInterrupts();

        if(gb.frameReady){
            gb.frameReady = false;
            platform.Update(gb.videoBuffer, VIDEOWIDTH * sizeof(uint32_t));
        }

    }  
return 0;
}
