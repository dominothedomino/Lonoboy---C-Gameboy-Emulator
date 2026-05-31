#include "gameboy.h"
#include <iostream>

const double FPS = 59.7275;
const double TPF = 1000.0/FPS;
const int SCALE = 6;

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "Usage: gameboy.exe <rom path>\n";
        return 1;
    }

    Platform platform("Gameboy", SCALE * VIDEOWIDTH, SCALE * VIDEOHEIGHT, VIDEOWIDTH, VIDEOHEIGHT);
    gameboy gb;
    gb.audioStream = platform.getAudioStream();
    // gb.logFile.open("gameboy.log");
    gb.loadROM(argv[1]);

    //passed 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11
    bool quit = false;
    while(!quit){
        uint64_t startFrame = SDL_GetTicks();

        gb.updateRTC();
        quit = platform.processInput(gb.actionBut, gb.dirBut);
        gb.updateJoypad(gb.actionBut, gb.dirBut);

        //for more than one cpu instruction per frame
        while(!gb.frameReady){
            uint16_t cycles = gb.Cycle();
            gb.updateTimers(cycles);
            gb.updatePPU(cycles);
            gb.updateAPU(cycles);
            gb.handleInterrupts();
        }

        gb.frameReady = false;
        platform.Update(gb.videoBuffer, VIDEOWIDTH * sizeof(uint32_t));

        uint64_t endFrame = SDL_GetTicks();
        double elapsed = endFrame - startFrame;
        if(elapsed < TPF){
            SDL_Delay(static_cast<uint32_t>(TPF - elapsed));
        }

    }  
return 0;
}
