#include <stdint.h>
#include <fstream>
#include <iomanip>
#include "graphics.cpp"

const unsigned int VIDEOWIDTH = 160;
const unsigned int VIDEOHEIGHT = 144;

union fullReg{
    uint16_t full;
    struct{
        uint8_t low;
        uint8_t high;
    };
};

struct Sprite{
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t flags;
};

class gameboy{
    public:
        fullReg BC{}, DE{}, HL{}, AF{};
        uint8_t* ROM{};
        uint8_t currentBank = 1;
        uint8_t VRAM[0x2000]{};
        uint8_t ExRAM[0x2000]{};
        uint8_t RAM[0x2000]{};
        uint8_t OamRAM[0xA0]{};
        uint8_t io[0x80]{};
        uint8_t hRAM[0x7F]{};
        uint8_t IE{};
        bool IME = false;
        bool futIME = false;

        uint32_t ROMsize;
        uint16_t sp{};
        uint16_t pc{};
        uint8_t opcode{};
        uint8_t mbctype{};

        bool stopped = false;
        bool halted = false;

        ///////////////////////For Input//////////////////////////////////
        //start unpressed
        uint8_t actionBut = 0x0F;
        uint8_t dirBut = 0x0F;

        void updateJoypad(uint8_t newAction, uint8_t newDir);

        ///////////////////////For Video///////////////////////////////////
        uint32_t videoBuffer[VIDEOWIDTH * VIDEOHEIGHT]{};
        bool frameReady = false;
        uint32_t ppuCycles = 0;
        uint8_t ppuMode = 2;
        uint8_t windowLine = 0;
        bool lastSTATIRQ = false;

        void updatePPU(uint16_t cycles);
        void drawScanLine();
        void renderLine();
        void checkGraphicsInt();


        /////////////////////For CPU Setup////////////////////////////////
        typedef uint8_t (gameboy::*gameboyFunc)();
        gameboyFunc opTable[256];
        gameboyFunc preTable[256];

        uint8_t voidOP();

        gameboy();
        ~gameboy();

        uint16_t Cycle();

        void setFlags(char flag, bool condition);
        uint8_t getFlags(char flag);
        void loadROM(const char* fileName);
        void switchBank(uint16_t address, uint8_t data);
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);

        void handleInterrupts();

        uint32_t divCounter = 0;
        uint32_t timerCounter = 0;

        void updateTimers(uint16_t cycles);


        //****************************DEBUG FILES AND FUNCTIONS******************************** */
        std::ofstream logFile;
        void logState(){
                logFile << std::uppercase << std::hex << std::setfill('0')
            << "A:" << std::setw(2) << (int)AF.high
            << " F:" << std::setw(2) << (int)AF.low
            << " B:" << std::setw(2) << (int)BC.high
            << " C:" << std::setw(2) << (int)BC.low
            << " D:" << std::setw(2) << (int)DE.high
            << " E:" << std::setw(2) << (int)DE.low
            << " H:" << std::setw(2) << (int)HL.high
            << " L:" << std::setw(2) << (int)HL.low
            << " SP:" << std::setw(4) << (int)sp
            << " PC:" << std::setw(4) << (int)pc
            << " PCMEM:" << std::setw(2) << (int)read(pc)
            << "," << std::setw(2) << (int)read(pc+1)
            << "," << std::setw(2) << (int)read(pc+2)
            << "," << std::setw(2) << (int)read(pc+3)
            << "\n";
        }

        //************************************************************* */


        //OPCODES INCOMING (returns cycles it was supposed to take for time to update PPU and timers)
        uint8_t OP_x00(); //NOP
        uint8_t OP_x01(); //LD BC, n16
        uint8_t OP_x02(); //LD [BC], A
        uint8_t OP_x03(); //INC BC
        uint8_t OP_x04(); //INC B
        uint8_t OP_x05(); //DEC B
        uint8_t OP_x06(); //LD B, n8
        uint8_t OP_x07(); //RCLA
        uint8_t OP_x08(); //LD [a16], SP
        uint8_t OP_x09(); //ADD HL, BC
        uint8_t OP_x0A(); //LD A, [BC]
        uint8_t OP_x0B(); //DEC BC
        uint8_t OP_x0C(); //INC C
        uint8_t OP_x0D(); //DEC C
        uint8_t OP_x0E(); //LD C, n8
        uint8_t OP_x0F(); //RRCA

        uint8_t OP_x10(); //STOP n8
        uint8_t OP_x11(); //LD DE, n16
        uint8_t OP_x12(); //LD [DE], A
        uint8_t OP_x13(); //INC DE
        uint8_t OP_x14(); //INC D
        uint8_t OP_x15(); //DEC D
        uint8_t OP_x16(); //LD D, n8
        uint8_t OP_x17(); //RLA
        uint8_t OP_x18(); //JR e8
        uint8_t OP_x19(); //ADD HL, DE
        uint8_t OP_x1A(); //LD A, [DE]
        uint8_t OP_x1B(); //DEC DE
        uint8_t OP_x1C(); //INC E
        uint8_t OP_x1D(); //DEC E
        uint8_t OP_x1E(); //LD E, n8
        uint8_t OP_x1F(); //RRA

        uint8_t OP_x20(); //JR NZ, e8
        uint8_t OP_x21(); //LD HL, n16
        uint8_t OP_x22(); //LD [HL+], A
        uint8_t OP_x23(); //INC HL
        uint8_t OP_x24(); //INC H
        uint8_t OP_x25(); //DEC H
        uint8_t OP_x26(); //LD H, n8
        uint8_t OP_x27(); //DAA
        uint8_t OP_x28(); //JR C, e8
        uint8_t OP_x29(); //ADD HL, SP
        uint8_t OP_x2A(); //LD A, [HL+]
        uint8_t OP_x2B(); //DEC HL
        uint8_t OP_x2C(); //INC L
        uint8_t OP_x2D(); //DEC L
        uint8_t OP_x2E(); //LD L, n8
        uint8_t OP_x2F(); //CPL

        uint8_t OP_x30(); //JR NC, e8
        uint8_t OP_x31(); //LD SP, n16
        uint8_t OP_x32(); //LD [HL-], A
        uint8_t OP_x33(); //INC SP
        uint8_t OP_x34(); //INC [HL]
        uint8_t OP_x35(); //DEC [HL]
        uint8_t OP_x36(); //LD [HL], n8
        uint8_t OP_x37(); //SCF
        uint8_t OP_x38(); //JR C, e8
        uint8_t OP_x39(); //ADD HL, SP
        uint8_t OP_x3A(); //LD A, [HL-]
        uint8_t OP_x3B(); //DEC SP
        uint8_t OP_x3C(); //INC A
        uint8_t OP_x3D(); //DEC A
        uint8_t OP_x3E(); //LD A, n8
        uint8_t OP_x3F(); //CCF

        uint8_t OP_x40(); //LD B, B
        uint8_t OP_x41(); //LD B, C
        uint8_t OP_x42(); //LD B, D
        uint8_t OP_x43(); //LD B, E
        uint8_t OP_x44(); //LD B, H
        uint8_t OP_x45(); //LD B, L
        uint8_t OP_x46(); //LD B, [HL]
        uint8_t OP_x47(); //LD B, A
        uint8_t OP_x48(); //LD C, B
        uint8_t OP_x49(); //LD C, C
        uint8_t OP_x4A(); //LD C, D
        uint8_t OP_x4B(); //LD C, E
        uint8_t OP_x4C(); //LD C, H
        uint8_t OP_x4D(); //LD C, L
        uint8_t OP_x4E(); //LD C, [HL]
        uint8_t OP_x4F(); //LD C, A

        uint8_t OP_x50(); //LD D, B
        uint8_t OP_x51(); //LD D, C
        uint8_t OP_x52(); //LD D, D
        uint8_t OP_x53(); //LD D, E
        uint8_t OP_x54(); //LD D, H
        uint8_t OP_x55(); //LD D, L
        uint8_t OP_x56(); //LD D, [HL]
        uint8_t OP_x57(); //LD D, A
        uint8_t OP_x58(); //LD E, B
        uint8_t OP_x59(); //LD E, C
        uint8_t OP_x5A(); //LD E, D
        uint8_t OP_x5B(); //LD E, E
        uint8_t OP_x5C(); //LD E, H
        uint8_t OP_x5D(); //LD E, L
        uint8_t OP_x5E(); //LD E, [HL]
        uint8_t OP_x5F(); //LD E, A

        uint8_t OP_x60(); //LD H, B
        uint8_t OP_x61(); //LD H, C
        uint8_t OP_x62(); //LD H, D
        uint8_t OP_x63(); //LD H, E
        uint8_t OP_x64(); //LD H, H
        uint8_t OP_x65(); //LD H, L
        uint8_t OP_x66(); //LD H, [HL]
        uint8_t OP_x67(); //LD H, A
        uint8_t OP_x68(); //LD L, B
        uint8_t OP_x69(); //LD L, C
        uint8_t OP_x6A(); //LD L, D
        uint8_t OP_x6B(); //LD L, E
        uint8_t OP_x6C(); //LD L, H
        uint8_t OP_x6D(); //LD L, L
        uint8_t OP_x6E(); //LD L, [HL]
        uint8_t OP_x6F(); //LD L, A

        uint8_t OP_x70(); //LD [HL], B
        uint8_t OP_x71(); //LD [HL], C
        uint8_t OP_x72(); //LD [HL], D
        uint8_t OP_x73(); //LD [HL], E
        uint8_t OP_x74(); //LD [HL], H
        uint8_t OP_x75(); //LD [HL], L
        uint8_t OP_x76(); //HALT
        uint8_t OP_x77(); //LD [HL], A
        uint8_t OP_x78(); //LD A, B
        uint8_t OP_x79(); //LD A, C
        uint8_t OP_x7A(); //LD A, D
        uint8_t OP_x7B(); //LD A, E
        uint8_t OP_x7C(); //LD A, H
        uint8_t OP_x7D(); //LD A, L
        uint8_t OP_x7E(); //LD A, [HL]
        uint8_t OP_x7F(); //LD A, A

        uint8_t OP_x80(); //ADD A, B
        uint8_t OP_x81(); //ADD A, C
        uint8_t OP_x82(); //ADD A, D
        uint8_t OP_x83(); //ADD A, E
        uint8_t OP_x84(); //ADD A, H
        uint8_t OP_x85(); //ADD A, L
        uint8_t OP_x86(); //ADD A, [HL]
        uint8_t OP_x87(); //ADD A, A
        uint8_t OP_x88(); //ADC A, B
        uint8_t OP_x89(); //ADC A, C
        uint8_t OP_x8A(); //ADC A, D
        uint8_t OP_x8B(); //ADC A, E
        uint8_t OP_x8C(); //ADC A, H
        uint8_t OP_x8D(); //ADC A, L
        uint8_t OP_x8E(); //ADC A, [HL]
        uint8_t OP_x8F(); //ADC A, A

        uint8_t OP_x90(); //SUB A, B
        uint8_t OP_x91(); //SUB A, C
        uint8_t OP_x92(); //SUB A, D
        uint8_t OP_x93(); //SUB A, E
        uint8_t OP_x94(); //SUB A, H
        uint8_t OP_x95(); //SUB A, L
        uint8_t OP_x96(); //SUB A, [HL]
        uint8_t OP_x97(); //SUB A, A
        uint8_t OP_x98(); //SBC A, B
        uint8_t OP_x99(); //SBC A, C
        uint8_t OP_x9A(); //SBC A, D
        uint8_t OP_x9B(); //SBC A, E
        uint8_t OP_x9C(); //SBC A, H
        uint8_t OP_x9D(); //SBC A, L
        uint8_t OP_x9E(); //SBC A, [HL]
        uint8_t OP_x9F(); //SBC A, A

        uint8_t OP_xA0(); //AND A, B
        uint8_t OP_xA1(); //AND A, C
        uint8_t OP_xA2(); //AND A, D
        uint8_t OP_xA3(); //AND A, E
        uint8_t OP_xA4(); //AND A, H
        uint8_t OP_xA5(); //AND A, L
        uint8_t OP_xA6(); //AND A, [HL]
        uint8_t OP_xA7(); //AND A, A
        uint8_t OP_xA8(); //XOR A, B
        uint8_t OP_xA9(); //XOR A, C
        uint8_t OP_xAA(); //XOR A, D
        uint8_t OP_xAB(); //XOR A, E
        uint8_t OP_xAC(); //XOR A, H
        uint8_t OP_xAD(); //XOR A, L
        uint8_t OP_xAE(); //XOR A, [HL]
        uint8_t OP_xAF(); //XOR A, A

        uint8_t OP_xB0(); //OR A, B
        uint8_t OP_xB1(); //OR A, C
        uint8_t OP_xB2(); //OR A, D
        uint8_t OP_xB3(); //OR A, E
        uint8_t OP_xB4(); //OR A, H
        uint8_t OP_xB5(); //OR A, L
        uint8_t OP_xB6(); //OR A, [HL]
        uint8_t OP_xB7(); //OR A, A
        uint8_t OP_xB8(); //CP A, B
        uint8_t OP_xB9(); //CP A, C
        uint8_t OP_xBA(); //CP A, D
        uint8_t OP_xBB(); //CP A, E
        uint8_t OP_xBC(); //CP A, H
        uint8_t OP_xBD(); //CP A, L
        uint8_t OP_xBE(); //CP A, [HL]
        uint8_t OP_xBF(); //CP A, A

        uint8_t OP_xC0(); //RET NZ
        uint8_t OP_xC1(); //POP BC
        uint8_t OP_xC2(); //JP NZ, a16
        uint8_t OP_xC3(); //JP a16
        uint8_t OP_xC4(); //CALL NZ, a16
        uint8_t OP_xC5(); //PUSH BC
        uint8_t OP_xC6(); //ADD A, n8
        uint8_t OP_xC7(); //RST $00
        uint8_t OP_xC8(); //RET Z
        uint8_t OP_xC9(); //RET
        uint8_t OP_xCA(); //JP Z, a16
        uint8_t OP_xCB(); //PREFIX
        uint8_t OP_xCC(); //CALL Z, a16
        uint8_t OP_xCD(); //CALL a16
        uint8_t OP_xCE(); //ADC A, n8
        uint8_t OP_xCF(); //RST $08

        uint8_t OP_xD0(); //RET NC
        uint8_t OP_xD1(); //POP DE
        uint8_t OP_xD2(); //JP NC, a16
        uint8_t OP_xD4(); //CALL NC, a16
        uint8_t OP_xD5(); //PUSH DE
        uint8_t OP_xD6(); //SUB A, n8
        uint8_t OP_xD7(); //RST $10
        uint8_t OP_xD8(); //RET C
        uint8_t OP_xD9(); //RETI
        uint8_t OP_xDA(); //JP C, a16
        uint8_t OP_xDC(); //CALL C, a16
        uint8_t OP_xDE(); //SBC A, n8
        uint8_t OP_xDF(); //RST $18

        uint8_t OP_xE0(); //LDH [a8], A
        uint8_t OP_xE1(); //POP HL
        uint8_t OP_xE2(); //LDH [C], A
        uint8_t OP_xE5(); //PUSH HL
        uint8_t OP_xE6(); //AND A, n8
        uint8_t OP_xE7(); //RST $20
        uint8_t OP_xE8(); //ADD SP, e8
        uint8_t OP_xE9(); //JP HL
        uint8_t OP_xEA(); //LD [a16], A
        uint8_t OP_xEE(); //XOR A, n8
        uint8_t OP_xEF(); //RST $28

        uint8_t OP_xF0(); //LDH A, [a8]
        uint8_t OP_xF1(); //POP AF
        uint8_t OP_xF2(); //LDH A, [C]
        uint8_t OP_xF3(); //DI
        uint8_t OP_xF5(); //PUSH AF
        uint8_t OP_xF6(); //OR A, n8
        uint8_t OP_xF7(); //RST $30
        uint8_t OP_xF8(); //LD HL, SP + e8
        uint8_t OP_xF9(); //LD SP, HL
        uint8_t OP_xFA(); //LD A, [a16]
        uint8_t OP_xFB(); //EI
        uint8_t OP_xFE(); //CP A, n8
        uint8_t OP_xFF(); //RST $38

        //Prefixes /////////////////////////////////////////////////////////////////////////
        uint8_t PRE_x00(); //RLC B
        uint8_t PRE_x01(); //RLC C
        uint8_t PRE_x02(); //RLC D
        uint8_t PRE_x03(); //RLC E
        uint8_t PRE_x04(); //RLC H
        uint8_t PRE_x05(); //RLC L
        uint8_t PRE_x06(); //RLC [HL]
        uint8_t PRE_x07(); //RLC A
        uint8_t PRE_x08(); //RRC B
        uint8_t PRE_x09(); //RRC C
        uint8_t PRE_x0A(); //RRC D
        uint8_t PRE_x0B(); //RRC E
        uint8_t PRE_x0C(); //RRC H
        uint8_t PRE_x0D(); //RRC L
        uint8_t PRE_x0E(); //RRC [HL]
        uint8_t PRE_x0F(); //RRC A

        uint8_t PRE_x10(); //RL B
        uint8_t PRE_x11(); //RL C
        uint8_t PRE_x12(); //RL D
        uint8_t PRE_x13(); //RL E
        uint8_t PRE_x14(); //RL H
        uint8_t PRE_x15(); //RL L
        uint8_t PRE_x16(); //RL [HL]
        uint8_t PRE_x17(); //RL A
        uint8_t PRE_x18(); //RR B
        uint8_t PRE_x19(); //RR C
        uint8_t PRE_x1A(); //RR D
        uint8_t PRE_x1B(); //RR E
        uint8_t PRE_x1C(); //RR H
        uint8_t PRE_x1D(); //RR L
        uint8_t PRE_x1E(); //RR [HL]
        uint8_t PRE_x1F(); //RR A

        uint8_t PRE_x20(); //SLA B
        uint8_t PRE_x21(); //SLA C
        uint8_t PRE_x22(); //SLA D
        uint8_t PRE_x23(); //SLA E
        uint8_t PRE_x24(); //SLA H
        uint8_t PRE_x25(); //SLA L
        uint8_t PRE_x26(); //SLA [HL]
        uint8_t PRE_x27(); //SLA A
        uint8_t PRE_x28(); //SRA B
        uint8_t PRE_x29(); //SRA C
        uint8_t PRE_x2A(); //SRA D
        uint8_t PRE_x2B(); //SRA E
        uint8_t PRE_x2C(); //SRA H
        uint8_t PRE_x2D(); //SRA L
        uint8_t PRE_x2E(); //SRA [HL]
        uint8_t PRE_x2F(); //SRA A

        uint8_t PRE_x30(); //SWAP B
        uint8_t PRE_x31(); //SWAP C
        uint8_t PRE_x32(); //SWAP D
        uint8_t PRE_x33(); //SWAP E
        uint8_t PRE_x34(); //SWAP H
        uint8_t PRE_x35(); //SWAP L
        uint8_t PRE_x36(); //SWAP [HL]
        uint8_t PRE_x37(); //SWAP A
        uint8_t PRE_x38(); //SRL B
        uint8_t PRE_x39(); //SRL C
        uint8_t PRE_x3A(); //SRL D
        uint8_t PRE_x3B(); //SRL E
        uint8_t PRE_x3C(); //SRL H
        uint8_t PRE_x3D(); //SRL L
        uint8_t PRE_x3E(); //SRL [HL]
        uint8_t PRE_x3F(); //SRL A

        uint8_t PRE_x40(); //BIT 0, B
        uint8_t PRE_x41(); //BIT 0, C
        uint8_t PRE_x42(); //BIT 0, D
        uint8_t PRE_x43(); //BIT 0, E
        uint8_t PRE_x44(); //BIT 0, H
        uint8_t PRE_x45(); //BIT 0, L
        uint8_t PRE_x46(); //BIT 0, [HL]
        uint8_t PRE_x47(); //BIT 0, A
        uint8_t PRE_x48(); //BIT 1, B
        uint8_t PRE_x49(); //BIT 1, C
        uint8_t PRE_x4A(); //BIT 1, D
        uint8_t PRE_x4B(); //BIT 1, E
        uint8_t PRE_x4C(); //BIT 1, H
        uint8_t PRE_x4D(); //BIT 1, L
        uint8_t PRE_x4E(); //BIT 1, [HL]
        uint8_t PRE_x4F(); //BIT 1, A

        uint8_t PRE_x50(); //BIT 2, B
        uint8_t PRE_x51(); //BIT 2, C
        uint8_t PRE_x52(); //BIT 2, D
        uint8_t PRE_x53(); //BIT 2, E
        uint8_t PRE_x54(); //BIT 2, H
        uint8_t PRE_x55(); //BIT 2, L
        uint8_t PRE_x56(); //BIT 2, [HL]
        uint8_t PRE_x57(); //BIT 2, A
        uint8_t PRE_x58(); //BIT 3, B
        uint8_t PRE_x59(); //BIT 3, C
        uint8_t PRE_x5A(); //BIT 3, D
        uint8_t PRE_x5B(); //BIT 3, E
        uint8_t PRE_x5C(); //BIT 3, H
        uint8_t PRE_x5D(); //BIT 3, L
        uint8_t PRE_x5E(); //BIT 3, [HL]
        uint8_t PRE_x5F(); //BIT 3, A

        uint8_t PRE_x60(); //BIT 4, B
        uint8_t PRE_x61(); //BIT 4, C
        uint8_t PRE_x62(); //BIT 4, D
        uint8_t PRE_x63(); //BIT 4, E
        uint8_t PRE_x64(); //BIT 4, H
        uint8_t PRE_x65(); //BIT 4, L
        uint8_t PRE_x66(); //BIT 4, [HL]
        uint8_t PRE_x67(); //BIT 4, A
        uint8_t PRE_x68(); //BIT 5, B
        uint8_t PRE_x69(); //BIT 5, C
        uint8_t PRE_x6A(); //BIT 5, D
        uint8_t PRE_x6B(); //BIT 5, E
        uint8_t PRE_x6C(); //BIT 5, H
        uint8_t PRE_x6D(); //BIT 5, L
        uint8_t PRE_x6E(); //BIT 5, [HL]
        uint8_t PRE_x6F(); //BIT 5, A

        uint8_t PRE_x70(); //BIT 6, B
        uint8_t PRE_x71(); //BIT 6, C
        uint8_t PRE_x72(); //BIT 6, D
        uint8_t PRE_x73(); //BIT 6, E
        uint8_t PRE_x74(); //BIT 6, H
        uint8_t PRE_x75(); //BIT 6, L
        uint8_t PRE_x76(); //BIT 6, [HL]
        uint8_t PRE_x77(); //BIT 6, A
        uint8_t PRE_x78(); //BIT 7, B
        uint8_t PRE_x79(); //BIT 7, C
        uint8_t PRE_x7A(); //BIT 7, D
        uint8_t PRE_x7B(); //BIT 7, E
        uint8_t PRE_x7C(); //BIT 7, H
        uint8_t PRE_x7D(); //BIT 7, L
        uint8_t PRE_x7E(); //BIT 7, [HL]
        uint8_t PRE_x7F(); //BIT 7, A

        uint8_t PRE_x80(); //RES 0, B
        uint8_t PRE_x81(); //RES 0, C
        uint8_t PRE_x82(); //RES 0, D
        uint8_t PRE_x83(); //RES 0, E
        uint8_t PRE_x84(); //RES 0, H
        uint8_t PRE_x85(); //RES 0, L
        uint8_t PRE_x86(); //RES 0, [HL]
        uint8_t PRE_x87(); //RES 0, A
        uint8_t PRE_x88(); //RES 1, B
        uint8_t PRE_x89(); //RES 1, C
        uint8_t PRE_x8A(); //RES 1, D
        uint8_t PRE_x8B(); //RES 1, E
        uint8_t PRE_x8C(); //RES 1, H
        uint8_t PRE_x8D(); //RES 1, L
        uint8_t PRE_x8E(); //RES 1, [HL]
        uint8_t PRE_x8F(); //RES 1, A

        uint8_t PRE_x90(); //RES 2, B
        uint8_t PRE_x91(); //RES 2, C
        uint8_t PRE_x92(); //RES 2, D
        uint8_t PRE_x93(); //RES 2, E
        uint8_t PRE_x94(); //RES 2, H
        uint8_t PRE_x95(); //RES 2, L
        uint8_t PRE_x96(); //RES 2, [HL]
        uint8_t PRE_x97(); //RES 2, A
        uint8_t PRE_x98(); //RES 3, B
        uint8_t PRE_x99(); //RES 3, C
        uint8_t PRE_x9A(); //RES 3, D
        uint8_t PRE_x9B(); //RES 3, E
        uint8_t PRE_x9C(); //RES 3, H
        uint8_t PRE_x9D(); //RES 3, L
        uint8_t PRE_x9E(); //RES 3, [HL]
        uint8_t PRE_x9F(); //RES 3, A

        uint8_t PRE_xA0(); //RES 4, B
        uint8_t PRE_xA1(); //RES 4, C
        uint8_t PRE_xA2(); //RES 4, D
        uint8_t PRE_xA3(); //RES 4, E
        uint8_t PRE_xA4(); //RES 4, H
        uint8_t PRE_xA5(); //RES 4, L
        uint8_t PRE_xA6(); //RES 4, [HL]
        uint8_t PRE_xA7(); //RES 4, A
        uint8_t PRE_xA8(); //RES 5, B
        uint8_t PRE_xA9(); //RES 5, C
        uint8_t PRE_xAA(); //RES 5, D
        uint8_t PRE_xAB(); //RES 5, E
        uint8_t PRE_xAC(); //RES 5, H
        uint8_t PRE_xAD(); //RES 5, L
        uint8_t PRE_xAE(); //RES 5, [HL]
        uint8_t PRE_xAF(); //RES 5, A

        uint8_t PRE_xB0(); //RES 6, B
        uint8_t PRE_xB1(); //RES 6, C
        uint8_t PRE_xB2(); //RES 6, D
        uint8_t PRE_xB3(); //RES 6, E
        uint8_t PRE_xB4(); //RES 6, H
        uint8_t PRE_xB5(); //RES 6, L
        uint8_t PRE_xB6(); //RES 6, [HL]
        uint8_t PRE_xB7(); //RES 6, A
        uint8_t PRE_xB8(); //RES 7, B
        uint8_t PRE_xB9(); //RES 7, C
        uint8_t PRE_xBA(); //RES 7, D
        uint8_t PRE_xBB(); //RES 7, E
        uint8_t PRE_xBC(); //RES 7, H
        uint8_t PRE_xBD(); //RES 7, L
        uint8_t PRE_xBE(); //RES 7, [HL]
        uint8_t PRE_xBF(); //RES 7, A

        uint8_t PRE_xC0(); //SET 0, B
        uint8_t PRE_xC1(); //SET 0, C
        uint8_t PRE_xC2(); //SET 0, D
        uint8_t PRE_xC3(); //SET 0, E
        uint8_t PRE_xC4(); //SET 0, H
        uint8_t PRE_xC5(); //SET 0, L
        uint8_t PRE_xC6(); //SET 0, [HL]
        uint8_t PRE_xC7(); //SET 0, A
        uint8_t PRE_xC8(); //SET 1, B
        uint8_t PRE_xC9(); //SET 1, C
        uint8_t PRE_xCA(); //SET 1, D
        uint8_t PRE_xCB(); //SET 1, E
        uint8_t PRE_xCC(); //SET 1, H
        uint8_t PRE_xCD(); //SET 1, L
        uint8_t PRE_xCE(); //SET 1, [HL]
        uint8_t PRE_xCF(); //SET 1, A

        uint8_t PRE_xD0(); //SET 2, B
        uint8_t PRE_xD1(); //SET 2, C
        uint8_t PRE_xD2(); //SET 2, D
        uint8_t PRE_xD3(); //SET 2, E
        uint8_t PRE_xD4(); //SET 2, H
        uint8_t PRE_xD5(); //SET 2, L
        uint8_t PRE_xD6(); //SET 2, [HL]
        uint8_t PRE_xD7(); //SET 2, A
        uint8_t PRE_xD8(); //SET 3, B
        uint8_t PRE_xD9(); //SET 3, C
        uint8_t PRE_xDA(); //SET 3, D
        uint8_t PRE_xDB(); //SET 3, E
        uint8_t PRE_xDC(); //SET 3, H
        uint8_t PRE_xDD(); //SET 3, L
        uint8_t PRE_xDE(); //SET 3, [HL]
        uint8_t PRE_xDF(); //SET 3, A

        uint8_t PRE_xE0(); //SET 4, B
        uint8_t PRE_xE1(); //SET 4, C
        uint8_t PRE_xE2(); //SET 4, D
        uint8_t PRE_xE3(); //SET 4, E
        uint8_t PRE_xE4(); //SET 4, H
        uint8_t PRE_xE5(); //SET 4, L
        uint8_t PRE_xE6(); //SET 4, [HL]
        uint8_t PRE_xE7(); //SET 4, A
        uint8_t PRE_xE8(); //SET 5, B
        uint8_t PRE_xE9(); //SET 5, C
        uint8_t PRE_xEA(); //SET 5, D
        uint8_t PRE_xEB(); //SET 5, E
        uint8_t PRE_xEC(); //SET 5, H
        uint8_t PRE_xED(); //SET 5, L
        uint8_t PRE_xEE(); //SET 5, [HL]
        uint8_t PRE_xEF(); //SET 5, A

        uint8_t PRE_xF0(); //SET 6, B
        uint8_t PRE_xF1(); //SET 6, C
        uint8_t PRE_xF2(); //SET 6, D
        uint8_t PRE_xF3(); //SET 6, E
        uint8_t PRE_xF4(); //SET 6, H
        uint8_t PRE_xF5();//SET 6, L
        uint8_t PRE_xF6(); //SET 6, [HL]
        uint8_t PRE_xF7(); //SET 6, A
        uint8_t PRE_xF8(); //SET 7, B
        uint8_t PRE_xF9(); //SET 7, C
        uint8_t PRE_xFA(); //SET 7, D
        uint8_t PRE_xFB(); //SET 7, E
        uint8_t PRE_xFC(); //SET 7, H
        uint8_t PRE_xFD(); //SET 7, L
        uint8_t PRE_xFE(); //SET 7, [HL]
        uint8_t PRE_xFF(); //SET 7, A

};