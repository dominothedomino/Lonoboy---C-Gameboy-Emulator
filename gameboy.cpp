#include "gameboy.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <set>

//4 duty cycle modes for apu
static const uint8_t DUTY_TABLE[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 0, 0, 0, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0},  
};

static const uint16_t NOISE_DIVISORS[8] = {8, 16, 32, 48, 64, 80, 96, 112};

gameboy::gameboy(){
    //skip Boot ROM
    sp = 0xFFFE; //goes to ff80
    pc = 0x0100;
    AF.full = 0x01B0; //A identifies hardware
    DE.full = 0x00D8;
    BC.full = 0x0013;
    HL.full = 0x014D;
    IME = stopped = halted = false;
    
    opTable[0x00] = &gameboy::OP_x00;
    opTable[0x01] = &gameboy::OP_x01;
    opTable[0x02] = &gameboy::OP_x02;
    opTable[0x03] = &gameboy::OP_x03;
    opTable[0x04] = &gameboy::OP_x04;
    opTable[0x05] = &gameboy::OP_x05;
    opTable[0x06] = &gameboy::OP_x06;
    opTable[0x07] = &gameboy::OP_x07;
    opTable[0x08] = &gameboy::OP_x08;
    opTable[0x09] = &gameboy::OP_x09;
    opTable[0x0A] = &gameboy::OP_x0A;
    opTable[0x0B] = &gameboy::OP_x0B;
    opTable[0x0C] = &gameboy::OP_x0C;
    opTable[0x0D] = &gameboy::OP_x0D;
    opTable[0x0E] = &gameboy::OP_x0E;
    opTable[0x0F] = &gameboy::OP_x0F;
    opTable[0x10] = &gameboy::OP_x10;
    opTable[0x11] = &gameboy::OP_x11;
    opTable[0x12] = &gameboy::OP_x12;
    opTable[0x13] = &gameboy::OP_x13;
    opTable[0x14] = &gameboy::OP_x14;
    opTable[0x15] = &gameboy::OP_x15;
    opTable[0x16] = &gameboy::OP_x16;
    opTable[0x17] = &gameboy::OP_x17;
    opTable[0x18] = &gameboy::OP_x18;
    opTable[0x19] = &gameboy::OP_x19;
    opTable[0x1A] = &gameboy::OP_x1A;
    opTable[0x1B] = &gameboy::OP_x1B;
    opTable[0x1C] = &gameboy::OP_x1C;
    opTable[0x1D] = &gameboy::OP_x1D;
    opTable[0x1E] = &gameboy::OP_x1E;
    opTable[0x1F] = &gameboy::OP_x1F;
    opTable[0x20] = &gameboy::OP_x20;
    opTable[0x21] = &gameboy::OP_x21;
    opTable[0x22] = &gameboy::OP_x22;
    opTable[0x23] = &gameboy::OP_x23;
    opTable[0x24] = &gameboy::OP_x24;
    opTable[0x25] = &gameboy::OP_x25;
    opTable[0x26] = &gameboy::OP_x26;
    opTable[0x27] = &gameboy::OP_x27;
    opTable[0x28] = &gameboy::OP_x28;
    opTable[0x29] = &gameboy::OP_x29;
    opTable[0x2A] = &gameboy::OP_x2A;
    opTable[0x2B] = &gameboy::OP_x2B;
    opTable[0x2C] = &gameboy::OP_x2C;
    opTable[0x2D] = &gameboy::OP_x2D;
    opTable[0x2E] = &gameboy::OP_x2E;
    opTable[0x2F] = &gameboy::OP_x2F;
    opTable[0x30] = &gameboy::OP_x30;
    opTable[0x31] = &gameboy::OP_x31;
    opTable[0x32] = &gameboy::OP_x32;
    opTable[0x33] = &gameboy::OP_x33;
    opTable[0x34] = &gameboy::OP_x34;
    opTable[0x35] = &gameboy::OP_x35;
    opTable[0x36] = &gameboy::OP_x36;
    opTable[0x37] = &gameboy::OP_x37;
    opTable[0x38] = &gameboy::OP_x38;
    opTable[0x39] = &gameboy::OP_x39;
    opTable[0x3A] = &gameboy::OP_x3A;
    opTable[0x3B] = &gameboy::OP_x3B;
    opTable[0x3C] = &gameboy::OP_x3C;
    opTable[0x3D] = &gameboy::OP_x3D;
    opTable[0x3E] = &gameboy::OP_x3E;
    opTable[0x3F] = &gameboy::OP_x3F;
    opTable[0x40] = &gameboy::OP_x40;
    opTable[0x41] = &gameboy::OP_x41;
    opTable[0x42] = &gameboy::OP_x42;
    opTable[0x43] = &gameboy::OP_x43;
    opTable[0x44] = &gameboy::OP_x44;
    opTable[0x45] = &gameboy::OP_x45;
    opTable[0x46] = &gameboy::OP_x46;
    opTable[0x47] = &gameboy::OP_x47;
    opTable[0x48] = &gameboy::OP_x48;
    opTable[0x49] = &gameboy::OP_x49;
    opTable[0x4A] = &gameboy::OP_x4A;
    opTable[0x4B] = &gameboy::OP_x4B;
    opTable[0x4C] = &gameboy::OP_x4C;
    opTable[0x4D] = &gameboy::OP_x4D;
    opTable[0x4E] = &gameboy::OP_x4E;
    opTable[0x4F] = &gameboy::OP_x4F;
    opTable[0x50] = &gameboy::OP_x50;
    opTable[0x51] = &gameboy::OP_x51;
    opTable[0x52] = &gameboy::OP_x52;
    opTable[0x53] = &gameboy::OP_x53;
    opTable[0x54] = &gameboy::OP_x54;
    opTable[0x55] = &gameboy::OP_x55;
    opTable[0x56] = &gameboy::OP_x56;
    opTable[0x57] = &gameboy::OP_x57;
    opTable[0x58] = &gameboy::OP_x58;
    opTable[0x59] = &gameboy::OP_x59;
    opTable[0x5A] = &gameboy::OP_x5A;
    opTable[0x5B] = &gameboy::OP_x5B;
    opTable[0x5C] = &gameboy::OP_x5C;
    opTable[0x5D] = &gameboy::OP_x5D;
    opTable[0x5E] = &gameboy::OP_x5E;
    opTable[0x5F] = &gameboy::OP_x5F;
    opTable[0x60] = &gameboy::OP_x60;
    opTable[0x61] = &gameboy::OP_x61;
    opTable[0x62] = &gameboy::OP_x62;
    opTable[0x63] = &gameboy::OP_x63;
    opTable[0x64] = &gameboy::OP_x64;
    opTable[0x65] = &gameboy::OP_x65;
    opTable[0x66] = &gameboy::OP_x66;
    opTable[0x67] = &gameboy::OP_x67;
    opTable[0x68] = &gameboy::OP_x68;
    opTable[0x69] = &gameboy::OP_x69;
    opTable[0x6A] = &gameboy::OP_x6A;
    opTable[0x6B] = &gameboy::OP_x6B;
    opTable[0x6C] = &gameboy::OP_x6C;
    opTable[0x6D] = &gameboy::OP_x6D;
    opTable[0x6E] = &gameboy::OP_x6E;
    opTable[0x6F] = &gameboy::OP_x6F;
    opTable[0x70] = &gameboy::OP_x70;
    opTable[0x71] = &gameboy::OP_x71;
    opTable[0x72] = &gameboy::OP_x72;
    opTable[0x73] = &gameboy::OP_x73;
    opTable[0x74] = &gameboy::OP_x74;
    opTable[0x75] = &gameboy::OP_x75;
    opTable[0x76] = &gameboy::OP_x76;
    opTable[0x77] = &gameboy::OP_x77;
    opTable[0x78] = &gameboy::OP_x78;
    opTable[0x79] = &gameboy::OP_x79;
    opTable[0x7A] = &gameboy::OP_x7A;
    opTable[0x7B] = &gameboy::OP_x7B;
    opTable[0x7C] = &gameboy::OP_x7C;
    opTable[0x7D] = &gameboy::OP_x7D;
    opTable[0x7E] = &gameboy::OP_x7E;
    opTable[0x7F] = &gameboy::OP_x7F;
    opTable[0x80] = &gameboy::OP_x80;
    opTable[0x81] = &gameboy::OP_x81;
    opTable[0x82] = &gameboy::OP_x82;
    opTable[0x83] = &gameboy::OP_x83;
    opTable[0x84] = &gameboy::OP_x84;
    opTable[0x85] = &gameboy::OP_x85;
    opTable[0x86] = &gameboy::OP_x86;
    opTable[0x87] = &gameboy::OP_x87;
    opTable[0x88] = &gameboy::OP_x88;
    opTable[0x89] = &gameboy::OP_x89;
    opTable[0x8A] = &gameboy::OP_x8A;
    opTable[0x8B] = &gameboy::OP_x8B;
    opTable[0x8C] = &gameboy::OP_x8C;
    opTable[0x8D] = &gameboy::OP_x8D;
    opTable[0x8E] = &gameboy::OP_x8E;
    opTable[0x8F] = &gameboy::OP_x8F;
    opTable[0x90] = &gameboy::OP_x90;
    opTable[0x91] = &gameboy::OP_x91;
    opTable[0x92] = &gameboy::OP_x92;
    opTable[0x93] = &gameboy::OP_x93;
    opTable[0x94] = &gameboy::OP_x94;
    opTable[0x95] = &gameboy::OP_x95;
    opTable[0x96] = &gameboy::OP_x96;
    opTable[0x97] = &gameboy::OP_x97;
    opTable[0x98] = &gameboy::OP_x98;
    opTable[0x99] = &gameboy::OP_x99;
    opTable[0x9A] = &gameboy::OP_x9A;
    opTable[0x9B] = &gameboy::OP_x9B;
    opTable[0x9C] = &gameboy::OP_x9C;
    opTable[0x9D] = &gameboy::OP_x9D;
    opTable[0x9E] = &gameboy::OP_x9E;
    opTable[0x9F] = &gameboy::OP_x9F;
    opTable[0xA0] = &gameboy::OP_xA0;
    opTable[0xA1] = &gameboy::OP_xA1;
    opTable[0xA2] = &gameboy::OP_xA2;
    opTable[0xA3] = &gameboy::OP_xA3;
    opTable[0xA4] = &gameboy::OP_xA4;
    opTable[0xA5] = &gameboy::OP_xA5;
    opTable[0xA6] = &gameboy::OP_xA6;
    opTable[0xA7] = &gameboy::OP_xA7;
    opTable[0xA8] = &gameboy::OP_xA8;
    opTable[0xA9] = &gameboy::OP_xA9;
    opTable[0xAA] = &gameboy::OP_xAA;
    opTable[0xAB] = &gameboy::OP_xAB;
    opTable[0xAC] = &gameboy::OP_xAC;
    opTable[0xAD] = &gameboy::OP_xAD;
    opTable[0xAE] = &gameboy::OP_xAE;
    opTable[0xAF] = &gameboy::OP_xAF;
    opTable[0xB0] = &gameboy::OP_xB0;
    opTable[0xB1] = &gameboy::OP_xB1;
    opTable[0xB2] = &gameboy::OP_xB2;
    opTable[0xB3] = &gameboy::OP_xB3;
    opTable[0xB4] = &gameboy::OP_xB4;
    opTable[0xB5] = &gameboy::OP_xB5;
    opTable[0xB6] = &gameboy::OP_xB6;
    opTable[0xB7] = &gameboy::OP_xB7;
    opTable[0xB8] = &gameboy::OP_xB8;
    opTable[0xB9] = &gameboy::OP_xB9;
    opTable[0xBA] = &gameboy::OP_xBA;
    opTable[0xBB] = &gameboy::OP_xBB;
    opTable[0xBC] = &gameboy::OP_xBC;
    opTable[0xBD] = &gameboy::OP_xBD;
    opTable[0xBE] = &gameboy::OP_xBE;
    opTable[0xBF] = &gameboy::OP_xBF;
    opTable[0xC0] = &gameboy::OP_xC0;
    opTable[0xC1] = &gameboy::OP_xC1;
    opTable[0xC2] = &gameboy::OP_xC2;
    opTable[0xC3] = &gameboy::OP_xC3;
    opTable[0xC4] = &gameboy::OP_xC4;
    opTable[0xC5] = &gameboy::OP_xC5;
    opTable[0xC6] = &gameboy::OP_xC6;
    opTable[0xC7] = &gameboy::OP_xC7;
    opTable[0xC8] = &gameboy::OP_xC8;
    opTable[0xC9] = &gameboy::OP_xC9;
    opTable[0xCA] = &gameboy::OP_xCA;
    opTable[0xCB] = &gameboy::OP_xCB;
    opTable[0xCC] = &gameboy::OP_xCC;
    opTable[0xCD] = &gameboy::OP_xCD;
    opTable[0xCE] = &gameboy::OP_xCE;
    opTable[0xCF] = &gameboy::OP_xCF;
    opTable[0xD0] = &gameboy::OP_xD0;
    opTable[0xD1] = &gameboy::OP_xD1;
    opTable[0xD2] = &gameboy::OP_xD2;
    opTable[0xD3] = &gameboy::voidOP;
    opTable[0xD4] = &gameboy::OP_xD4;
    opTable[0xD5] = &gameboy::OP_xD5;
    opTable[0xD6] = &gameboy::OP_xD6;
    opTable[0xD7] = &gameboy::OP_xD7;
    opTable[0xD8] = &gameboy::OP_xD8;
    opTable[0xD9] = &gameboy::OP_xD9;
    opTable[0xDA] = &gameboy::OP_xDA;
    opTable[0xDB] = &gameboy::voidOP;
    opTable[0xDC] = &gameboy::OP_xDC;
    opTable[0xDD] = &gameboy::voidOP;
    opTable[0xDE] = &gameboy::OP_xDE;
    opTable[0xDF] = &gameboy::OP_xDF;
    opTable[0xE0] = &gameboy::OP_xE0;
    opTable[0xE1] = &gameboy::OP_xE1;
    opTable[0xE2] = &gameboy::OP_xE2;
    opTable[0xE3] = &gameboy::voidOP;
    opTable[0xE4] = &gameboy::voidOP;
    opTable[0xE5] = &gameboy::OP_xE5;
    opTable[0xE6] = &gameboy::OP_xE6;
    opTable[0xE7] = &gameboy::OP_xE7;
    opTable[0xE8] = &gameboy::OP_xE8;
    opTable[0xE9] = &gameboy::OP_xE9;
    opTable[0xEA] = &gameboy::OP_xEA;
    opTable[0xEB] = &gameboy::voidOP;
    opTable[0xEC] = &gameboy::voidOP;
    opTable[0xED] = &gameboy::voidOP;
    opTable[0xEE] = &gameboy::OP_xEE;
    opTable[0xEF] = &gameboy::OP_xEF;
    opTable[0xF0] = &gameboy::OP_xF0;
    opTable[0xF1] = &gameboy::OP_xF1;
    opTable[0xF2] = &gameboy::OP_xF2;
    opTable[0xF3] = &gameboy::OP_xF3;
    opTable[0xF4] = &gameboy::voidOP;
    opTable[0xF5] = &gameboy::OP_xF5;
    opTable[0xF6] = &gameboy::OP_xF6;
    opTable[0xF7] = &gameboy::OP_xF7;
    opTable[0xF8] = &gameboy::OP_xF8;
    opTable[0xF9] = &gameboy::OP_xF9;
    opTable[0xFA] = &gameboy::OP_xFA;
    opTable[0xFB] = &gameboy::OP_xFB;
    opTable[0xFC] = &gameboy::voidOP;
    opTable[0xFD] = &gameboy::voidOP;
    opTable[0xFE] = &gameboy::OP_xFE;
    opTable[0xFF] = &gameboy::OP_xFF;
    preTable[0x00] = &gameboy::PRE_x00;
    preTable[0x01] = &gameboy::PRE_x01;
    preTable[0x02] = &gameboy::PRE_x02;
    preTable[0x03] = &gameboy::PRE_x03;
    preTable[0x04] = &gameboy::PRE_x04;
    preTable[0x05] = &gameboy::PRE_x05;
    preTable[0x06] = &gameboy::PRE_x06;
    preTable[0x07] = &gameboy::PRE_x07;
    preTable[0x08] = &gameboy::PRE_x08;
    preTable[0x09] = &gameboy::PRE_x09;
    preTable[0x0A] = &gameboy::PRE_x0A;
    preTable[0x0B] = &gameboy::PRE_x0B;
    preTable[0x0C] = &gameboy::PRE_x0C;
    preTable[0x0D] = &gameboy::PRE_x0D;
    preTable[0x0E] = &gameboy::PRE_x0E;
    preTable[0x0F] = &gameboy::PRE_x0F;
    preTable[0x10] = &gameboy::PRE_x10;
    preTable[0x11] = &gameboy::PRE_x11;
    preTable[0x12] = &gameboy::PRE_x12;
    preTable[0x13] = &gameboy::PRE_x13;
    preTable[0x14] = &gameboy::PRE_x14;
    preTable[0x15] = &gameboy::PRE_x15;
    preTable[0x16] = &gameboy::PRE_x16;
    preTable[0x17] = &gameboy::PRE_x17;
    preTable[0x18] = &gameboy::PRE_x18;
    preTable[0x19] = &gameboy::PRE_x19;
    preTable[0x1A] = &gameboy::PRE_x1A;
    preTable[0x1B] = &gameboy::PRE_x1B;
    preTable[0x1C] = &gameboy::PRE_x1C;
    preTable[0x1D] = &gameboy::PRE_x1D;
    preTable[0x1E] = &gameboy::PRE_x1E;
    preTable[0x1F] = &gameboy::PRE_x1F;
    preTable[0x20] = &gameboy::PRE_x20;
    preTable[0x21] = &gameboy::PRE_x21;
    preTable[0x22] = &gameboy::PRE_x22;
    preTable[0x23] = &gameboy::PRE_x23;
    preTable[0x24] = &gameboy::PRE_x24;
    preTable[0x25] = &gameboy::PRE_x25;
    preTable[0x26] = &gameboy::PRE_x26;
    preTable[0x27] = &gameboy::PRE_x27;
    preTable[0x28] = &gameboy::PRE_x28;
    preTable[0x29] = &gameboy::PRE_x29;
    preTable[0x2A] = &gameboy::PRE_x2A;
    preTable[0x2B] = &gameboy::PRE_x2B;
    preTable[0x2C] = &gameboy::PRE_x2C;
    preTable[0x2D] = &gameboy::PRE_x2D;
    preTable[0x2E] = &gameboy::PRE_x2E;
    preTable[0x2F] = &gameboy::PRE_x2F;
    preTable[0x30] = &gameboy::PRE_x30;
    preTable[0x31] = &gameboy::PRE_x31;
    preTable[0x32] = &gameboy::PRE_x32;
    preTable[0x33] = &gameboy::PRE_x33;
    preTable[0x34] = &gameboy::PRE_x34;
    preTable[0x35] = &gameboy::PRE_x35;
    preTable[0x36] = &gameboy::PRE_x36;
    preTable[0x37] = &gameboy::PRE_x37;
    preTable[0x38] = &gameboy::PRE_x38;
    preTable[0x39] = &gameboy::PRE_x39;
    preTable[0x3A] = &gameboy::PRE_x3A;
    preTable[0x3B] = &gameboy::PRE_x3B;
    preTable[0x3C] = &gameboy::PRE_x3C;
    preTable[0x3D] = &gameboy::PRE_x3D;
    preTable[0x3E] = &gameboy::PRE_x3E;
    preTable[0x3F] = &gameboy::PRE_x3F;
    preTable[0x40] = &gameboy::PRE_x40;
    preTable[0x41] = &gameboy::PRE_x41;
    preTable[0x42] = &gameboy::PRE_x42;
    preTable[0x43] = &gameboy::PRE_x43;
    preTable[0x44] = &gameboy::PRE_x44;
    preTable[0x45] = &gameboy::PRE_x45;
    preTable[0x46] = &gameboy::PRE_x46;
    preTable[0x47] = &gameboy::PRE_x47;
    preTable[0x48] = &gameboy::PRE_x48;
    preTable[0x49] = &gameboy::PRE_x49;
    preTable[0x4A] = &gameboy::PRE_x4A;
    preTable[0x4B] = &gameboy::PRE_x4B;
    preTable[0x4C] = &gameboy::PRE_x4C;
    preTable[0x4D] = &gameboy::PRE_x4D;
    preTable[0x4E] = &gameboy::PRE_x4E;
    preTable[0x4F] = &gameboy::PRE_x4F;
    preTable[0x50] = &gameboy::PRE_x50;
    preTable[0x51] = &gameboy::PRE_x51;
    preTable[0x52] = &gameboy::PRE_x52;
    preTable[0x53] = &gameboy::PRE_x53;
    preTable[0x54] = &gameboy::PRE_x54;
    preTable[0x55] = &gameboy::PRE_x55;
    preTable[0x56] = &gameboy::PRE_x56;
    preTable[0x57] = &gameboy::PRE_x57;
    preTable[0x58] = &gameboy::PRE_x58;
    preTable[0x59] = &gameboy::PRE_x59;
    preTable[0x5A] = &gameboy::PRE_x5A;
    preTable[0x5B] = &gameboy::PRE_x5B;
    preTable[0x5C] = &gameboy::PRE_x5C;
    preTable[0x5D] = &gameboy::PRE_x5D;
    preTable[0x5E] = &gameboy::PRE_x5E;
    preTable[0x5F] = &gameboy::PRE_x5F;
    preTable[0x60] = &gameboy::PRE_x60;
    preTable[0x61] = &gameboy::PRE_x61;
    preTable[0x62] = &gameboy::PRE_x62;
    preTable[0x63] = &gameboy::PRE_x63;
    preTable[0x64] = &gameboy::PRE_x64;
    preTable[0x65] = &gameboy::PRE_x65;
    preTable[0x66] = &gameboy::PRE_x66;
    preTable[0x67] = &gameboy::PRE_x67;
    preTable[0x68] = &gameboy::PRE_x68;
    preTable[0x69] = &gameboy::PRE_x69;
    preTable[0x6A] = &gameboy::PRE_x6A;
    preTable[0x6B] = &gameboy::PRE_x6B;
    preTable[0x6C] = &gameboy::PRE_x6C;
    preTable[0x6D] = &gameboy::PRE_x6D;
    preTable[0x6E] = &gameboy::PRE_x6E;
    preTable[0x6F] = &gameboy::PRE_x6F;
    preTable[0x70] = &gameboy::PRE_x70;
    preTable[0x71] = &gameboy::PRE_x71;
    preTable[0x72] = &gameboy::PRE_x72;
    preTable[0x73] = &gameboy::PRE_x73;
    preTable[0x74] = &gameboy::PRE_x74;
    preTable[0x75] = &gameboy::PRE_x75;
    preTable[0x76] = &gameboy::PRE_x76;
    preTable[0x77] = &gameboy::PRE_x77;
    preTable[0x78] = &gameboy::PRE_x78;
    preTable[0x79] = &gameboy::PRE_x79;
    preTable[0x7A] = &gameboy::PRE_x7A;
    preTable[0x7B] = &gameboy::PRE_x7B;
    preTable[0x7C] = &gameboy::PRE_x7C;
    preTable[0x7D] = &gameboy::PRE_x7D;
    preTable[0x7E] = &gameboy::PRE_x7E;
    preTable[0x7F] = &gameboy::PRE_x7F;
    preTable[0x80] = &gameboy::PRE_x80;
    preTable[0x81] = &gameboy::PRE_x81;
    preTable[0x82] = &gameboy::PRE_x82;
    preTable[0x83] = &gameboy::PRE_x83;
    preTable[0x84] = &gameboy::PRE_x84;
    preTable[0x85] = &gameboy::PRE_x85;
    preTable[0x86] = &gameboy::PRE_x86;
    preTable[0x87] = &gameboy::PRE_x87;
    preTable[0x88] = &gameboy::PRE_x88;
    preTable[0x89] = &gameboy::PRE_x89;
    preTable[0x8A] = &gameboy::PRE_x8A;
    preTable[0x8B] = &gameboy::PRE_x8B;
    preTable[0x8C] = &gameboy::PRE_x8C;
    preTable[0x8D] = &gameboy::PRE_x8D;
    preTable[0x8E] = &gameboy::PRE_x8E;
    preTable[0x8F] = &gameboy::PRE_x8F;
    preTable[0x90] = &gameboy::PRE_x90;
    preTable[0x91] = &gameboy::PRE_x91;
    preTable[0x92] = &gameboy::PRE_x92;
    preTable[0x93] = &gameboy::PRE_x93;
    preTable[0x94] = &gameboy::PRE_x94;
    preTable[0x95] = &gameboy::PRE_x95;
    preTable[0x96] = &gameboy::PRE_x96;
    preTable[0x97] = &gameboy::PRE_x97;
    preTable[0x98] = &gameboy::PRE_x98;
    preTable[0x99] = &gameboy::PRE_x99;
    preTable[0x9A] = &gameboy::PRE_x9A;
    preTable[0x9B] = &gameboy::PRE_x9B;
    preTable[0x9C] = &gameboy::PRE_x9C;
    preTable[0x9D] = &gameboy::PRE_x9D;
    preTable[0x9E] = &gameboy::PRE_x9E;
    preTable[0x9F] = &gameboy::PRE_x9F;
    preTable[0xA0] = &gameboy::PRE_xA0;
    preTable[0xA1] = &gameboy::PRE_xA1;
    preTable[0xA2] = &gameboy::PRE_xA2;
    preTable[0xA3] = &gameboy::PRE_xA3;
    preTable[0xA4] = &gameboy::PRE_xA4;
    preTable[0xA5] = &gameboy::PRE_xA5;
    preTable[0xA6] = &gameboy::PRE_xA6;
    preTable[0xA7] = &gameboy::PRE_xA7;
    preTable[0xA8] = &gameboy::PRE_xA8;
    preTable[0xA9] = &gameboy::PRE_xA9;
    preTable[0xAA] = &gameboy::PRE_xAA;
    preTable[0xAB] = &gameboy::PRE_xAB;
    preTable[0xAC] = &gameboy::PRE_xAC;
    preTable[0xAD] = &gameboy::PRE_xAD;
    preTable[0xAE] = &gameboy::PRE_xAE;
    preTable[0xAF] = &gameboy::PRE_xAF;
    preTable[0xB0] = &gameboy::PRE_xB0;
    preTable[0xB1] = &gameboy::PRE_xB1;
    preTable[0xB2] = &gameboy::PRE_xB2;
    preTable[0xB3] = &gameboy::PRE_xB3;
    preTable[0xB4] = &gameboy::PRE_xB4;
    preTable[0xB5] = &gameboy::PRE_xB5;
    preTable[0xB6] = &gameboy::PRE_xB6;
    preTable[0xB7] = &gameboy::PRE_xB7;
    preTable[0xB8] = &gameboy::PRE_xB8;
    preTable[0xB9] = &gameboy::PRE_xB9;
    preTable[0xBA] = &gameboy::PRE_xBA;
    preTable[0xBB] = &gameboy::PRE_xBB;
    preTable[0xBC] = &gameboy::PRE_xBC;
    preTable[0xBD] = &gameboy::PRE_xBD;
    preTable[0xBE] = &gameboy::PRE_xBE;
    preTable[0xBF] = &gameboy::PRE_xBF;
    preTable[0xC0] = &gameboy::PRE_xC0;
    preTable[0xC1] = &gameboy::PRE_xC1;
    preTable[0xC2] = &gameboy::PRE_xC2;
    preTable[0xC3] = &gameboy::PRE_xC3;
    preTable[0xC4] = &gameboy::PRE_xC4;
    preTable[0xC5] = &gameboy::PRE_xC5;
    preTable[0xC6] = &gameboy::PRE_xC6;
    preTable[0xC7] = &gameboy::PRE_xC7;
    preTable[0xC8] = &gameboy::PRE_xC8;
    preTable[0xC9] = &gameboy::PRE_xC9;
    preTable[0xCA] = &gameboy::PRE_xCA;
    preTable[0xCB] = &gameboy::PRE_xCB;
    preTable[0xCC] = &gameboy::PRE_xCC;
    preTable[0xCD] = &gameboy::PRE_xCD;
    preTable[0xCE] = &gameboy::PRE_xCE;
    preTable[0xCF] = &gameboy::PRE_xCF;
    preTable[0xD0] = &gameboy::PRE_xD0;
    preTable[0xD1] = &gameboy::PRE_xD1;
    preTable[0xD2] = &gameboy::PRE_xD2;
    preTable[0xD3] = &gameboy::PRE_xD3;
    preTable[0xD4] = &gameboy::PRE_xD4;
    preTable[0xD5] = &gameboy::PRE_xD5;
    preTable[0xD6] = &gameboy::PRE_xD6;
    preTable[0xD7] = &gameboy::PRE_xD7;
    preTable[0xD8] = &gameboy::PRE_xD8;
    preTable[0xD9] = &gameboy::PRE_xD9;
    preTable[0xDA] = &gameboy::PRE_xDA;
    preTable[0xDB] = &gameboy::PRE_xDB;
    preTable[0xDC] = &gameboy::PRE_xDC;
    preTable[0xDD] = &gameboy::PRE_xDD;
    preTable[0xDE] = &gameboy::PRE_xDE;
    preTable[0xDF] = &gameboy::PRE_xDF;
    preTable[0xE0] = &gameboy::PRE_xE0;
    preTable[0xE1] = &gameboy::PRE_xE1;
    preTable[0xE2] = &gameboy::PRE_xE2;
    preTable[0xE3] = &gameboy::PRE_xE3;
    preTable[0xE4] = &gameboy::PRE_xE4;
    preTable[0xE5] = &gameboy::PRE_xE5;
    preTable[0xE6] = &gameboy::PRE_xE6;
    preTable[0xE7] = &gameboy::PRE_xE7;
    preTable[0xE8] = &gameboy::PRE_xE8;
    preTable[0xE9] = &gameboy::PRE_xE9;
    preTable[0xEA] = &gameboy::PRE_xEA;
    preTable[0xEB] = &gameboy::PRE_xEB;
    preTable[0xEC] = &gameboy::PRE_xEC;
    preTable[0xED] = &gameboy::PRE_xED;
    preTable[0xEE] = &gameboy::PRE_xEE;
    preTable[0xEF] = &gameboy::PRE_xEF;
    preTable[0xF0] = &gameboy::PRE_xF0;
    preTable[0xF1] = &gameboy::PRE_xF1;
    preTable[0xF2] = &gameboy::PRE_xF2;
    preTable[0xF3] = &gameboy::PRE_xF3;
    preTable[0xF4] = &gameboy::PRE_xF4;
    preTable[0xF5] = &gameboy::PRE_xF5;
    preTable[0xF6] = &gameboy::PRE_xF6;
    preTable[0xF7] = &gameboy::PRE_xF7;
    preTable[0xF8] = &gameboy::PRE_xF8;
    preTable[0xF9] = &gameboy::PRE_xF9;
    preTable[0xFA] = &gameboy::PRE_xFA;
    preTable[0xFB] = &gameboy::PRE_xFB;
    preTable[0xFC] = &gameboy::PRE_xFC;
    preTable[0xFD] = &gameboy::PRE_xFD;
    preTable[0xFE] = &gameboy::PRE_xFE;
    preTable[0xFF] = &gameboy::PRE_xFF;

    io[0x00] = 0xCF; //joypad
    io[0x01] = 0;
    io[0x02] = 0x7E;
    io[0x04] = 0xAB;
    io[0x05] = 0;   //timer counter, modulo, control
    io[0x06] = 0;
    io[0x07] = 0;
    io[0x0F] = 0xE1;
    io[0x10] = 0x80; //sound
    io[0x11] = 0xBF;
    io[0x12] = 0xF3;
    io[0x13] = 0xFF;
    io[0x14] = 0xBF;
    io[0x16] = 0x3F;
    io[0x17] = 0;
    io[0x18] = 0xFF;
    io[0x19] = 0xBF;
    io[0x1A] = 0x7F;
    io[0x1B] = 0xFF;
    io[0x1C] = 0x9F;
    io[0x1D] = 0xFF;
    io[0x1E] = 0xBF;
    io[0x20] = 0xFF;
    io[0x21] = 0;
    io[0x22] = 0;
    io[0x23] = 0xBF;
    io[0x24] = 0x77;
    io[0x25] = 0xF3;
    io[0x26] = 0xF1;
    io[0x40] = 0x91; //lcd control
    io[0x41] = 0x85;
    io[0x42] = 0;
    io[0x43] = 0;
    io[0x44] = 0;
    io[0x45] = 0;
    io[0x46] = 0xFF;
    io[0x47] = 0xFC; //bg
    io[0x48] = 0;
    io[0x49] = 0;
    io[0x4A] = 0;
    io[0x4B] = 0;
    ppuMode = 2;
    io[0x44] = 0;
    io[0x41] = (io[0x41] & 0xFC) | 2;  // STAT reflects mode 2
    ppuCycles = 0;
    IE = 0x00;
}

void gameboy::updateAPU(uint16_t cycles){
    if(!(io[0x26] & 0x80)) return; //master sound off

    //update ch1 timers
    ch1.freqTimer -= cycles;
    while(ch1.freqTimer <= 0){
        ch1.freqTimer += (2048 - ch1.frequency) * 4;
        ch1.dutyPos = (ch1.dutyPos + 1) & 0x07;
    }

    //update ch2 timers
    ch2.freqTimer -= cycles;
    while(ch2.freqTimer <= 0){
        ch2.freqTimer += (2048 - ch2.frequency) * 4;
        ch2.dutyPos = (ch2.dutyPos + 1) & 0x07;
    }

    //update ch3 timers
    ch3.freqTimer -= cycles;
    if(ch3.freqTimer <= 0){
        ch3.freqTimer = (2048 - ch3.frequency) * 2;
        ch3.wavePos = (ch3.wavePos + 1) & 31;

        //get sample from the wave table
        uint8_t waveBits = io[0x30 + (ch3.wavePos / 2)];
        if(ch3.wavePos & 0x01) ch3.samepleBuffer = waveBits & 0x0F;
        else ch3.samepleBuffer = waveBits >> 4;
    }

    //update ch4 lfsr
    ch4.freqTimer -= cycles;
    while(ch4.freqTimer <= 0){
        ch4.freqTimer += NOISE_DIVISORS[ch4.divisor] << ch4.clockShift;

        //lfsr msb is loaded with xor of bits 1,0
        uint8_t xorBit = (ch4.lfsr & 0x01) ^ ((ch4.lfsr >> 1) & 0x01);
        ch4.lfsr = (ch4.lfsr >> 1) | (xorBit << 14);
        if(ch4.shortMode) ch4.lfsr = (ch4.lfsr & ~0x40) | (xorBit << 6);
    }

    apuDivTimer += cycles;
    //Frame sequencer steps every 8192 cycles as counted by Div Timer (512 Hz)
    while(apuDivTimer >= 8192){
        apuDivTimer -= 8192;
        stepFrameSequencer();
    }

    apuSampleTimer += cycles;
    //95.1 cycles per sample technically, but i chose to round down
    while(apuSampleTimer >= 95){
        apuSampleTimer -= 95;
        pushSample();
    }
}

void gameboy::stepFrameSequencer(){
    apuDiv = (apuDiv + 1) & 7;

    //if steps 0, 2, 4, or 6
    if(!(apuDiv & 0x01)) clockLengthCounters();

    if(apuDiv == 2 || apuDiv == 6) clockSweep();

    if(apuDiv == 7) clockEnvelopes();
}

void gameboy::clockLengthCounters(){
    //channel 1
    if(ch1.lengthEn && (ch1.lengthCounter > 0)){
        ch1.lengthCounter--;
        if(!ch1.lengthCounter) ch1.En = false;
    }

    //channel 2
    if(ch2.lengthEn && (ch2.lengthCounter > 0)){
        ch2.lengthCounter--;
        if(!ch2.lengthCounter) ch2.En = false;
    }

    //channel 3
    if(ch3.lengthEn && (ch3.lengthCounter > 0)){
        ch3.lengthCounter--;
        if(!ch3.lengthCounter) ch3.En = false;
    }

    //channel 4
    if(ch4.lengthEn && (ch4.lengthCounter > 0)){
        ch4.lengthCounter--;
        if(!ch4.lengthCounter) ch4.En = false;
    }
}

void gameboy::clockSweep(){
    //channel 1
    if(ch1.sweepTimer > 0) ch1.sweepTimer--;

    if(!ch1.sweepTimer){
        if(ch1.sweepPace > 0) ch1.sweepTimer = ch1.sweepPace;
        else ch1.sweepTimer = 8;

        if(ch1.sweepEn && (ch1.sweepPace > 0)){
            uint16_t newFreq = calculateSweep();

            //freq is not overflow and step is nonzero
            if((ch1.frequency <= 2047) && (ch1.sweepStep > 0)){
                ch1.frequency = newFreq;
                ch1.sweepFreq = newFreq;

                //update freq regs
                io[0x13] = ch1.frequency & 0xFF;
                io[0x14] = (io[0x14] & 0xF8) | ((ch1.frequency >> 8) & 0x07);

                //dummy var, only want to set overflow regs
                uint16_t dummyFreq = calculateSweep();
            }
        }

    }

}

void gameboy::clockEnvelopes(){
    //channel 1
    if(ch1.envPace != 0){
        if(ch1.envTimer > 0) ch1.envTimer--;
        if(!ch1.envTimer){
            ch1.envTimer = ch1.envPace;
            if(ch1.envUp && (ch1.volume < 15)) ch1.volume++;
            else if(!ch1.envUp && (ch1.volume > 0)) ch1.volume--;
        }
    }

    //channel 2
    if(ch2.envPace != 0){
        if(ch2.envTimer > 0) ch2.envTimer--;
        if(!ch2.envTimer){
            ch2.envTimer = ch2.envPace;
            if(ch2.envUp && (ch2.volume < 15)) ch2.volume++;
            else if(!ch2.envUp && (ch2.volume > 0)) ch2.volume--;
        }
    }

    //channel 3 has no envelope

    //channel 4
    if(ch4.envPace != 0){
        if(ch4.envTimer > 0) ch4.envTimer--;
        if(!ch4.envTimer){
            ch4.envTimer = ch4.envPace;
            if(ch4.envUp && (ch4.volume < 15)) ch4.volume++;
            else if(!ch4.envUp && (ch4.volume > 0)) ch4.volume--;
        }
    }
}

void gameboy::pushSample(){
    if(!audioStream) return;

    //prevent SDL from taking too many samples
    //735 samples per frame, stereo sound (2 channels), want bytes, and want to keep at most 4 frames worth of samples
    int queued = SDL_GetAudioStreamQueued(audioStream);
    if(queued > (735 * 4 * sizeof(float) * 2)){
        return;
    }

    //get ch1 output
    float ch1Sample = 0.0;
    if(ch1.En){
        uint8_t dutyBit = DUTY_TABLE[ch1.duty][ch1.dutyPos];
        if(dutyBit) ch1Sample = ch1.volume / 15.0f;
        else ch1Sample = 0.0;
    }

    //ch2
    float ch2Sample = 0.0;
    if(ch2.En){
        uint8_t dutyBit = DUTY_TABLE[ch2.duty][ch2.dutyPos];
        if(dutyBit) ch2Sample = ch2.volume / 15.0f;
        else ch2Sample = 0.0;
    }

    //ch3
    float ch3Sample = 0.0;
    if(ch3.En && ch3.dacEn){
        float rawData = ch3.samepleBuffer / 15.0f;
        //refer to .h for volume buffer settings
        switch(ch3.volumeShift){
            case 0:{
                ch3Sample = 0.0;
                break;
            }
            case 1:{
                ch3Sample = rawData;
                break;
            }
            case 2:{
                ch3Sample = rawData / 2.0f;
                break;
            }
            case 3:{
                ch3Sample = rawData / 4.0f;
                break;
            }
        }
    }

    //ch4
    float ch4Sample = 0.0;
    if(ch4.En){
        //output negative logic with lfsr lsb
        if(!(ch4.lfsr & 0x01)) ch4Sample = ch4.volume / 15.0f;
    }

    //master vol - NR50
    float leftVol = ((io[0x24] >> 4) & 0x07) / 7.0f;
    float rightVol= (io[0x24] & 0x07) / 7.0f;

    //panning - NR51
    float left, right;
    left = right = 0.0;
    if(io[0x25] & 0x10) left += ch1Sample * leftVol;
    if(io[0x25] & 0x01) right += ch1Sample * rightVol;
    if(io[0x25] & 0x20) left += ch2Sample * leftVol;
    if(io[0x25] & 0x02) right += ch2Sample * rightVol;
    if(io[0x25] & 0x40) left += ch3Sample * leftVol;
    if(io[0x25] & 0x04) right += ch3Sample * rightVol;
    if(io[0x25] & 0x80) left += ch4Sample * leftVol;
    if(io[0x25] & 0x08) right += ch4Sample * rightVol;

    //clamp to -1/1 to avoid clipping
    if(left > 1.0) left = 1.0;
    if(left < -1.0) left = -1.0;
    if(right > 1.0) right = 1.0;
    if(right < -1.0) right = -1.0;

    //low pass filter to get rid of grainyness
    float audioFilterL = 0.0;
    float audioFilterR = 0.0;
    const float coeff = 0.85;
    audioFilterL = (audioFilterL * coeff) + (left * (1.0 - coeff));
    audioFilterR = (audioFilterR * coeff) + (left * (1.0 - coeff));

    float samples[2] = {audioFilterL, audioFilterR}; //L and R channels
    SDL_PutAudioStreamData(audioStream, samples, sizeof(samples));
}

void gameboy::triggerChannel1(){
    ch1.En = true;

    if(!ch1.lengthCounter) ch1.lengthCounter = 64;
    ch1.freqTimer = (2048 - ch1.frequency) * 4;
    ch1.volume = ch1.initialVol;
    ch1.envTimer = ch1.envPace;

    //init sweep
    ch1.sweepFreq = ch1.frequency;
    if(ch1.sweepPace > 0) ch1.sweepTimer = ch1.sweepPace;
    else ch1.sweepTimer = 8;
    ch1.sweepEn = (ch1.sweepPace > 0) || (ch1.sweepStep > 0);

    //if sweep step nonzero, caluclate overflow; use a dummy variable to do so, we just want to check overflow and set channels, not get new freq
    if(ch1.sweepStep > 0){
        uint16_t newFreq = calculateSweep();
    } 
}

void gameboy::triggerChannel2(){
    ch2.En = true;

    if(!ch2.lengthCounter) ch2.lengthCounter = 64;
    ch2.freqTimer = (2048 - ch2.frequency) * 4;
    ch2.volume = ch2.initialVol;
    ch2.envTimer = ch2.envPace;
}

void gameboy::triggerChannel3(){
    ch3.En = ch3.dacEn;
    if(!ch3.lengthCounter) ch3.lengthCounter = 256;
    ch3.freqTimer = (2048 - ch3.freqTimer) * 2;
    ch3.wavePos = 0;
}

void gameboy::triggerChannel4(){
    ch4.En = true;
    if(!ch4.lengthCounter) ch4.lengthCounter = 64;
    ch4.volume = ch4.initialVol;
    ch4.envTimer = ch4.envPace;
    ch4.lfsr = 0x7FFF;
    ch4.freqTimer = NOISE_DIVISORS[ch4.divisor] << ch4.clockShift;
}

uint16_t gameboy::calculateSweep(){
    uint16_t newFreq = ch1.sweepFreq >> ch1.sweepStep;
    if(ch1.sweepUp) newFreq = ch1.sweepFreq + newFreq;
    else newFreq = ch1.sweepFreq - newFreq;

    //overflow
    if(newFreq > 2047) ch1.En = false;

    return newFreq;
}

void gameboy::updateRTC(){
    uint64_t currTime = time(nullptr);
    if(rtcPrev == 0){
        rtcPrev = currTime;
        return;
    }

    //if halt flag, clocks are frozen
    if(rtcRegs[4] & 0x40) return;

    uint64_t elapsed = currTime - rtcPrev;
    if(elapsed == 0) return;
    rtcPrev = currTime;

    rtcRegs[0] += elapsed;

    if(rtcRegs[0] >= 60){
        rtcRegs[1] += rtcRegs[0] / 60;
        rtcRegs[0] %= 60;
    }

    if(rtcRegs[1] >= 60){
        rtcRegs[2] += rtcRegs[1] / 60;
        rtcRegs[1] %= 60;
    }

    if(rtcRegs[2] >= 24){
        uint16_t days = ((rtcRegs[4] & 0x01) << 8) | rtcRegs[3];
        days += rtcRegs[2] / 24;
        rtcRegs[2] %= 24;

        if(days > 511){
            days &= 0x1FF;
            rtcRegs[4] |= 0x80; //overflow flag
        }

        rtcRegs[3] = days & 0xFF;
        rtcRegs[4] = (rtcRegs[4] & 0xFE) | ((days >> 8) & 0x01);
    }
}

void gameboy::updateJoypad(uint8_t newAction, uint8_t newDir){
    uint8_t select = io[0x00] & 0x30;
    uint8_t newBut, oldBut;

    if(!(select & 0x20)){
        oldBut = actionBut;
        newBut = newAction;
    }
    if(!(select & 0x10)){
        oldBut = dirBut;
        newBut = newDir;
    }
    else{
        actionBut = newAction;
        dirBut = newDir;
        return;
    }

    if(oldBut & ~newBut){
        io[0x0F] |= 0x10;   //joypad interrupt
    }

    actionBut = newAction;
    dirBut = newDir;
}

void gameboy::updatePPU(uint16_t cycles){
    if(!(io[0x40] & 0x80)){
        return;
    }

    ppuCycles += cycles;
    switch(ppuMode){
        case 2: //OAM scan
            if(ppuCycles >= 80){
                ppuCycles -= 80;
                ppuMode = 3;
                io[0x41] = (io[0x41] & 0xFC) | ppuMode;
                checkGraphicsInt();
            }
            break;
        case 3: //Pixel Transfer
            if(ppuCycles >= 172){
                ppuCycles -= 172;
                ppuMode = 0;
                io[0x41] = (io[0x41] & 0xFC) | ppuMode;
                drawScanLine();
                checkGraphicsInt();
            }
            break;
        case 0: //HBlank
            if(ppuCycles >= 204){
                ppuCycles -= 204;
                io[0x44]++;

                if(io[0x44] == 144){    //last line to draw scanlines
                    ppuMode = 1;
                    io[0x41] = (io[0x41] & 0xFC) | ppuMode;
                    checkGraphicsInt();
                    io[0x0F] |= 0x01;
                    renderLine();
                }
                else{
                    ppuMode = 2;
                    io[0x41] = (io[0x41] & 0xFC) | ppuMode;
                    checkGraphicsInt();
                }
            }
            break;
        case 1: //Vblank
            if(ppuCycles >= 456){
                ppuCycles -= 456;
                io[0x44]++;
                if(io[0x44] ==  154){
                    io[0x44] = 0;
                    windowLine = 0;
                    ppuMode = 2;
                    io[0x41] = (io[0x41] & 0xFC) | ppuMode;
                    checkGraphicsInt();
                }
                else{
                    checkGraphicsInt();
                }
            }
            break;
    }
}

void gameboy::checkGraphicsInt(){
    bool interrupt = false;

    //interrupt if ly == lyc
    if(io[0x44] == io[0x45]){
        io[0x41] |= 0x04;
        if(io[0x41] & 0x40) interrupt = true;
    }
    else io[0x41] &= ~(0x04);

    switch(ppuMode){
        //HBLANK
        case 0: if(io[0x41] & 0x08) interrupt = true; break; 
        //VBLANK
        case 1: if(io[0x41] & 0x10) interrupt = true; break;
        //OAM Scan
        case 2: if(io[0x41] & 0x20) interrupt = true; break;
    }
    
    if(interrupt && !lastSTATIRQ) io[0x0F] |= 0x02; //request stat interrupt
    lastSTATIRQ = interrupt;
}

void gameboy::drawScanLine(){
    static const uint32_t palette[] = {
        0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000
    };
    uint8_t lcdControl = io[0x40];
    uint8_t line = io[0x44];
    if(line >= 144) return;

    //Background control////////////////////////////////////////////////
    if(lcdControl & 0x01){

    uint16_t tilemapAddr;
    if(lcdControl & 0x08) tilemapAddr = 0x9C00;
    else tilemapAddr = 0x9800;

    uint16_t tiledataAddr;
    if(lcdControl & 0x10) tiledataAddr = 0x8000;
    else tiledataAddr = 0x8800;

    uint8_t scrollY = io[0x42];
    uint8_t scrollX = io[0x43];

    uint8_t yPos = scrollY + line;
    uint16_t tileRow = ((yPos) / 8) * 32;

    for(int pixel = 0; pixel < VIDEOWIDTH; pixel++){
        uint8_t xPos = scrollX + pixel;
        uint16_t tileCol = ((xPos) / 8);
        
        uint16_t tileAddr = tilemapAddr + tileRow + tileCol;
        uint16_t tileNum, tileLocation;
        if(lcdControl & 0x10){
            tileNum = read(tileAddr);
            tileLocation = tiledataAddr + (tileNum * 16);
        }
        else{
            tileNum = (int8_t)read(tileAddr);
            tileLocation = tiledataAddr + ((tileNum + 128) * 16); //making the number a positive offset instead of anoffset from x9000
        }

        uint8_t tileY = yPos % 8;
        uint8_t tileX = xPos % 8;
        uint8_t bitNum = 7 - tileX;
        uint8_t byte1 = read(tileLocation + (tileY * 2));
        uint8_t byte2 = read(tileLocation + (tileY * 2) + 1);

        uint8_t colorBits = ((byte1 >> bitNum) & 0x01) | (((byte2 >> bitNum) & 0x01) << 1);
        uint8_t colorIndex = (io[0x47] >> (colorBits * 2)) & 0x03;

        videoBuffer[line * 160 + pixel] = palette[colorIndex];
    }
    }


    //Window Controls////////////////////////////////////////////////////////////////////
    if(lcdControl & 0x20){

    uint8_t wy = io[0x4A];
    uint8_t wx = io[0x4B];  //windowX + 7

    //if not visible on the screen
    if(line >= wy && wx <= 166){

    uint16_t windowmapAddr;
    if(lcdControl & 0x40) windowmapAddr = 0x9C00;
    else windowmapAddr = 0x9800;

    uint16_t windowdataAddr;
    if(lcdControl & 0x10) windowdataAddr = 0x8000;
    else windowdataAddr = 0x8800;

    uint16_t windowRow = (windowLine / 8) * 32;
    int16_t screenX = (int16_t)wx - 7;

    for(int pixel = 0; pixel < 160; pixel++){
        if(pixel >= screenX){
            uint8_t xPos = pixel - screenX;

            uint16_t windowCol = xPos / 8;
            uint16_t windowAddr = windowmapAddr + windowRow + windowCol;

            int16_t windowOff;
            uint16_t windowLocation;
            if(lcdControl & 0x10){
                windowOff = (uint8_t)read(windowAddr);
                windowLocation = windowdataAddr + (windowOff * 16);
            } 
            else{
                windowOff = (int8_t)read(windowAddr);
                windowLocation = windowdataAddr + ((windowOff + 128) * 16);
            }

            uint8_t windowX = xPos % 8;
            uint8_t windowY = windowLine % 8;
            uint8_t byte1 = read(windowLocation + (windowY * 2));
            uint8_t byte2 = read(windowLocation + (windowY * 2) + 1);

            uint8_t bitNum = 7 - windowX;
            uint8_t colorBit = ((byte1 >> bitNum) & 1) | (((byte2 >> bitNum) & 1) << 1);

            uint8_t bgp = io[0x47];
            uint8_t colorIndex = (bgp >> (colorBit * 2)) & 0x03;

            videoBuffer[line * 160 + pixel] = palette[colorIndex];
        }
    }
    if((lcdControl & 0x20) && line >= wy && wx <= 166){
        windowLine++;
    }
    }
    }


    //Sprite Control/////////////////////////////////////////////////////
    if(!(lcdControl & 0x02)) return;

    uint8_t spriteHeight;
    if(lcdControl & 0x04) spriteHeight = 16;
    else spriteHeight = 8;

    Sprite spriteArray[10];
    uint8_t count = 0;

    for(int i = 0; i < 40; i++){
        if(count < 10){
            uint8_t y, x, tile, flags;
            y = OamRAM[4 * i];
            x = OamRAM[(4 * i) + 1];
            tile = OamRAM[(4 * i) + 2];
            flags = OamRAM[(4 * i) + 3];

            int16_t actualY = (int)y - 16;
            if(line >= actualY && line < actualY + spriteHeight){
                spriteArray[count++] = {y, x, tile, flags};
            }
        }
    }

    for(int pixel = 0; pixel < 160; pixel++){
        //for priority
        for(int curr = count - 1; curr >= 0; curr--){
            uint8_t currY, currX, currTile, currFlags;
            currY = spriteArray[curr].y;
            currX = spriteArray[curr].x;
            currTile = spriteArray[curr].tile;
            currFlags = spriteArray[curr].flags;
            
            int screenX = (int)currX - 8;
            int screenY = (int)currY - 16;

            if(!(pixel < screenX || pixel >= screenX + 8)){
                //if pixel is at this space, figure out the row of the sprite, depending on if it is flipped or not
                int tileRow = line - screenY;
                if(currFlags & 0x40) tileRow = (spriteHeight - 1) - tileRow;

                //special cases for 16 pixel high sprites
                uint8_t tileIndex = currTile;
                if(spriteHeight == 16) tileIndex &= 0xFE;
                if(spriteHeight == 16 && tileRow >= 8){
                    tileIndex |= 0x01;
                    tileRow -= 8;
                }

                uint16_t spriteAddr = 0x8000 + (tileIndex * 16) + (tileRow * 2);
                uint8_t byte1, byte2;
                byte1 = read(spriteAddr);
                byte2 = read(spriteAddr + 1);

                int tileCol = pixel - screenX;
                if(currFlags & 0x20) tileCol = 7 - tileCol;

                uint8_t bitNum = 7 - tileCol;
                uint8_t colorBits = ((byte1 >> bitNum) & 0x01) | (((byte2 >> bitNum) & 0x01) << 1);
                //not transparent
                if(colorBits != 0){
                    uint8_t paletteReg;
                    if(currFlags & 0x10) paletteReg = io[0x49];
                    else paletteReg = io[0x48];

                    uint8_t paletteIndex = (paletteReg >> (colorBits * 2)) & 0x03;

                    //skip pixel if priority flag is set and background isn't white
                    if(currFlags & 0x80){
                        if(videoBuffer[line * 160 + pixel] != palette[0]) continue;
                    }

                    videoBuffer[line * 160 + pixel] = palette[paletteIndex];
                }

            }
        }
    }
}

void gameboy::renderLine(){
    frameReady = true;
}

uint16_t gameboy::Cycle(){
    if(halted){
        if(io[0x0F] & IE & 0x1F){
            halted = false;
        } else{
            return 4;
        }
    }
    if(stopped) return 4;

    //**************************************************************** */
    // logState();
    //*************************************************************** */

    opcode = read(pc++);
    // std::cout << "Current Opcode: " << std::hex << std::showbase << opcode << " " << "Current pc: " << pc << '\n';
    uint16_t cycles = (this->*opTable[opcode])();

    if(futIME){
        futIME = false;
        IME = true;
    }

    return cycles;
}

void gameboy::setFlags(char flag, bool condition){
    if(flag == 'Z'){
        if(condition) AF.low |= (1 << 7);
        else AF.low &= ~(1 << 7);
    }
    else if(flag == 'N'){
        if(condition) AF.low |= (1 << 6);
        else AF.low &= ~(1 << 6);
    }
    else if(flag == 'H'){
        if(condition) AF.low |= (1 << 5);
        else AF.low &= ~(1 << 5);
    }
    else if(flag == 'C'){
        if(condition) AF.low |= (1 << 4);
        else AF.low &= ~(1 << 4);
    }
}

uint8_t gameboy::getFlags(char flag){
    if(flag =='Z'){
        return (AF.low & ((1 << 7))) >> 7;
    }
    else if(flag == 'N'){
        return (AF.low & ((1 << 6))) >> 6;
    }
    else if(flag == 'H'){
        return (AF.low & ((1 << 5))) >> 5;
    }
    else if(flag == 'C'){
        return (AF.low & ((1 << 4))) >> 4;
    }  
    else return 0;
}

void gameboy::loadROM(const char* fileName){
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if(file.is_open()){
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);

        ROM = new uint8_t[size];

        file.read(reinterpret_cast<char*>(ROM), size);
        file.close();
        ROMsize = static_cast<uint32_t>(size);

        mbctype = ROM[0x0147];
    }
}

gameboy::~gameboy(){
    delete [] ROM;
    if(audioStream) SDL_DestroyAudioStream(audioStream);
}

void gameboy::switchBank(uint16_t address, uint8_t value){
    switch(mbctype){
        //no MBC (ROM only)
        case 0x00:
            break;
        
        //MBC 1
        case 0x01:
        case 0x02:
        case 0x03:
            if(address <= 0x1FFF){
                ramEn = (value & 0x0F) == 0x0A;
            }
            else if(address <= 0x3FFF){
                //5 bit ROM bank
                uint8_t bank = value & 0x1F;
                romBank = (romBank & 0x60) | bank;
                if(romBank == 0x00 || romBank == 0x20 || romBank == 0x40 || romBank == 0x60) romBank++; //cannot map to bank 0
            }
            else if(address <= 0x5FFF){
                //2 bit secondary register
                mbc1UpperBits = value & 0x03;
                if(!mbc1RomRamMode) romBank = (romBank & 0x1F) | (mbc1UpperBits << 5);
                else ramBank = mbc1UpperBits;

            }
            else{
                mbc1RomRamMode = value & 0x1F;
                if(!mbc1RomRamMode){
                    //switch to rom bank mode
                    ramBank = 0;
                    romBank = (romBank & 0x1F) | (mbc1UpperBits << 5);
                }
                else{
                    //ram bank mode
                    romBank &= 0x1F;
                }
            }
            break;
        
        //MBC3
        case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
            if(address <= 0x1FFF){
                ramEn = (value & 0x0F) == 0x0A;
            }
            else if(address <= 0x3FFF){
                //7 bit ROM bank, bank 0 to 1
                romBank = value & 0x7F;
                if(romBank == 0) romBank = 1;
            }
            else if(address <= 0x5FFF){
                //ram bank OR real time clock reg
                if(value <= 0x03){
                    ramBank = value;
                    rtcMapping = false;
                }
                else if(value >= 0x08 && value <= 0x0F){
                    rtcReg = value - 0x08;
                    rtcMapping = true;
                }
            }
            else{
                if(value == 0x00){
                    rtcLatchArmed = true;
                }
                else if(value == 0x01 && rtcLatchArmed){
                    for(int i = 0; i < 5; i++){
                        rtcLatched[i] = rtcRegs[i];
                    }
                    rtcLatchArmed = false;
                }
                else{
                    rtcLatchArmed = false;
                }
            }
            break;

        //MBC5
        case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E:
            if(address <= 0x1FFF){
                ramEn = (value & 0x0F) == 0x0A;
            }
            else if(address <= 0x2FFF){
                //8 bit rom bank, allows bank 0
                romBank = (romBank & 0x100) | value;
            }
            else if(address <= 0x3FFF){
                romBank = (romBank & 0xFF) | ((value & 0x01) << 8);
            }
            else if(address <= 0x5FFF){
                //4 bit ram bank
                ramBank = value & 0x0F;
            }
            break;
        
        default:
            break;
    }
}

uint8_t gameboy::read(uint16_t address){
    if(address <= 0x3FFF){
        if(address < ROMsize) return ROM[address];
        else return 0xFF;
    }
    else if(address <= 0x7FFF){
        uint32_t offset = (static_cast<uint32_t>(romBank) * 0x4000) + (address - 0x4000);
        if(offset < ROMsize) return ROM[offset];
        else return 0xFF;
    }
    else if(address <= 0x9FFF){
        return VRAM[address - 0x8000];
    }
    else if(address <= 0xBFFF){
        if(!ramEn) return 0xFF;
        if(rtcMapping) return rtcLatched[rtcReg];

        uint32_t offset = (static_cast<uint32_t>(ramBank) * 0x2000) + (address - 0xA000);
        return ExRAM[offset % sizeof(ExRAM)];
    }
    else if(address <= 0xDFFF){
        return RAM[address - 0xC000];
    }
    else if(address <= 0xFDFF){
        return RAM[address - 0xE000]; //echo RAM mirrors RAM
    }
    else if(address <= 0xFE9F){
        return OamRAM[address - 0xFE00];
    }
    else if(address <= 0xFEFF){
        return 0xFF;
    } 
    else if(address <= 0xFF7F){

        if(address == 0xFF00){
            if(!(io[0x00] & 0x20)){
                return (io[0x00] & 0xF0) | actionBut;
            }
            else if(!(io[0x00] & 0x10)){
                return (io[0x00] & 0xF0) | dirBut;
            }
            return 0xFF;
        } 

        return io[address - 0xFF00];
    }
    else if(address <= 0xFFFE){
        return hRAM[address - 0xFF80];
    }
    else return IE;
}

void gameboy::write(uint16_t address, uint8_t data){
    if(address <= 0x7FFF){
        switchBank(address, data);
    }
    else if(address <= 0x9FFF){
        VRAM[address - 0x8000] = data;
    }
    else if(address <= 0xBFFF){
        //write to ex ram only if enabled
        if(!ramEn) return;
        
        if(rtcMapping){
            rtcRegs[rtcReg] = data;
            return;
        }

        uint32_t offset = (static_cast<uint32_t>(ramBank) * 0x2000) + (address - 0xA000);
        ExRAM[offset % sizeof(ExRAM)] = data;
    }
    else if(address <= 0xDFFF){
        RAM[address - 0xC000] = data;
    }
    else if(address <= 0xFDFF){
        RAM[address - 0xE000] = data; //echo RAM mirrors RAM
    }
    else if(address <= 0xFE9F){
        OamRAM[address - 0xFE00] = data;
    }
    else if(address <= 0xFEFF){}
    else if(address <= 0xFF7F){
        io[address - 0xFF00] = data;

        if(address == 0xFF10){
            //NR10 - sweep
            ch1.sweepPace = (data >> 4) & 0x07;
            ch1.sweepUp = !((data  >> 3) & 0x01);
            //ch1.sweepFreq = data & 0x03;
        }

        else if(address == 0xFF11){
            //NR11 - length
            ch1.duty = (data >> 6) & 0x03;
            ch1.lengthLoad = data & 0x3F;
            ch1.lengthCounter = 64 - ch1.lengthLoad;
        }

        else if(address == 0xFF12){
            //NR12 - volume
            ch1.initialVol = (data >> 4) & 0x0F;
            ch1.envUp = (data >> 3) & 0x01;
            ch1.envPace = data & 0x07;
            if(!(data & 0xF8)) ch1.En = false;
        }

        else if(address == 0xFF13){
            //NR13 - frequency low byte
            ch1.frequency = (ch1.frequency & 0x700) | data;
        }

        else if(address == 0xFF14){
            //NR14 - frequency upper bits, control
            ch1.frequency = (ch1.frequency & 0x0FF) | ((data & 0x07) << 8);
            ch1.lengthEn = (data >> 6) & 0x01;

            //if bit 7, restart channel 1
            if(data & 0x80) triggerChannel1();
        }

        else if(address == 0xFF16){
            //NR21 - Length (skip sweep for channel 2)
            ch2.duty = (data >> 6) & 0x03;
            ch2.lengthLoad = data & 0x3F;
            ch2.lengthCounter = 64 - ch2.lengthLoad;
        }

        else if(address == 0xFF17){
            //NR22 - volume
            ch2.initialVol = (data >> 4) & 0x0F;
            ch2.envUp = (data >> 3) & 0x01;
            ch2.envPace = data & 0x07;
            if(!(data & 0xF8)) ch2.En = false;
        }

        else if(address == 0xFF18){
            //NR23 - Freq low byte
            ch2.frequency = (ch2.frequency & 0x700) | data;
        }

        else if(address == 0xFF19){
            //NR24 - freq upper bits/control
            ch2.frequency = (ch2.frequency & 0x0FF) | ((data & 0x07) << 8);
            ch2.lengthEn = (data >> 6) & 0x01;

            //if bit 7, restart channel 1
            if(data & 0x80) triggerChannel2();
        }

        else if(address == 0xFF1A){
            //NR30 - DAC en
            ch3.dacEn = (data >> 7) & 0x01;
            if(!ch3.dacEn) ch3.En = false;
        }

        else if(address == 0xFF1B){
            //NR31 - length
            ch3.lengthLoad = data;
            ch3.lengthCounter = 256 - ch3.lengthLoad;
        }

        else if(address == 0xFF1C){
            //NR32 - volume
            ch3.volumeShift = (data >> 5) & 0x03;
        }

        else if(address == 0xFF1D){
            //NR33 - freq low byte
            ch3.frequency = (ch3.frequency & 0x700) | data;
        }

        else if(address == 0xFF1E){
            //NR34 - high byte freq/control
            ch3.frequency = (ch3.frequency & 0x0FF) | ((data & 0x07) << 8);
            ch3.lengthEn = (data >> 6) & 0x01;
            if(data & 0x80) triggerChannel3();
        }

        else if(address == 0xFF20){
            //NR41 - length
            ch4.lengthLoad = data & 0x3F;
            ch4.lengthCounter = 64 - ch4.lengthLoad;
        }

        else if(address == 0xFF21){
            //NR42 - volume
            ch4.initialVol = (data >> 4) & 0x0F;
            ch4.envUp = (data >> 3) & 0x01;
            ch4.envPace = data & 0x07;
            if(!(data & 0xF8)) ch4.En = false;
        }

        else if(address == 0xFF22){
            //NR43 - clock shift, lfsr width, clock divide
            ch4.clockShift = (data >> 4) & 0x0F;
            ch4.shortMode = (data >> 3) & 0x01;
            ch4.divisor = data & 0x07;
        }

        else if(address == 0xFF23){
            //NR44 - control
            ch4.lengthEn = (data >> 6) & 0x01;
            if(data & 0x80) triggerChannel4();
        }



        if(address == 0xFF44){
            io[0x44] = 0;
            return;
        }

        if(address == 0xFF46){
            uint16_t addr = static_cast<uint16_t>(data) << 8;
            for(int i = 0; i < 160; i++){
                OamRAM[i] = read(addr + i);
            }
        }

        //serial transfer
        if((address == 0xFF02) && data == 0x81){
            char c = static_cast<char>(io[0x01]);
            // if(c >= 32 && c < 127 || c == '\n' || c == '\r' || c == '\t'){
                std::cout << c;
                std::cout.flush();
                io[0x02] = 0;
                io[0x0F] |= 0x08;
        }

    }
    else if(address <= 0xFFFE){
        hRAM[address - 0xFF80] = data;
    }
    else IE = data;
}

void gameboy::handleInterrupts(){
    if(!IME) return;
    uint8_t status = io[0x0F] & IE & 0x1F;
    if(!status) return;

    IME = false;    //no dual interrupts

    static const uint16_t vectors[] = {0x40, 0x48, 0x50, 0x58, 0x60};

    for(int i = 0; i < 5; i++){
        if(status & (1 << i)){
            io[0x0F] &= ~(1 << i);

            write(--sp, (pc >> 8) & 0xFF);
            write(--sp, pc & 0xFF);

            pc = vectors[i];
            break;
        }
    }
}

void gameboy::updateTimers(uint16_t cycles){
    divCounter += cycles;
    if(divCounter >= 256){
        divCounter -= 256;  //inc every 256 T-cycles
        io[0x04]++;
    }

    if(!(io[0x07]&0x04)) return;

    timerCounter += cycles;
    uint16_t limit; uint8_t limBits = io[0x07] & 0x03;
    if(limBits == 0) limit = 1024;
    else if(limBits == 1) limit = 16;
    else if(limBits == 2) limit = 64;
    else limit = 256;
    while(timerCounter >= limit){
        timerCounter -= limit;
        io[0x05]++;

        if(io[0x05] == 0){
            io[0x05] = io[0x06];    //reload TIMA with reload value
            io[0x0F] |= 0x04;       //set bit for interrupt
        }   
    }
}

uint8_t gameboy::voidOP(){
    return 4;
}

//*********************************OPCODES *****************************************//
uint8_t gameboy::OP_x00(){
    return 4;
}

uint8_t gameboy::OP_x01(){
    BC.low = read(pc++);
    BC.high = read(pc++);

    return 12;
}

uint8_t gameboy::OP_x02(){
    write(BC.full, AF.high);

    return 8;
}

uint8_t gameboy::OP_x03(){
    BC.full++;
    return 8;
}

uint8_t gameboy::OP_x04(){
    BC.high++;
    setFlags('Z', BC.high == 0);
    setFlags('N', 0);
    setFlags('H', (BC.high & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x05(){
    BC.high--;
    setFlags('Z', BC.high == 0);
    setFlags('N', 1);
    setFlags('H', (BC.high & 0x0F) == 0x0F);
    return 4;
}

uint8_t gameboy::OP_x06(){
    BC.high = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x07(){
    setFlags('C', (AF.high & 0x80) >> 7);
    uint8_t val = AF.high;
    AF.high = (val << 1) | ((val & 0x80) >> 7);
    setFlags('Z', 0);
    setFlags('H', 0);
    setFlags('N', 0);

    return 4;
}

uint8_t gameboy::OP_x08(){
    uint8_t low, high;
    low = read(pc++);
    high = read(pc++);
    uint16_t address = (high << 8) | low;
    write(address, sp & 0xFF);
    write(address + 1, sp >> 8);
    return 20;
}

uint8_t gameboy::OP_x09(){
    uint32_t result = HL.full + BC.full;
    setFlags('N', 0);
    setFlags('H', ((HL.full&0xFFF) + (BC.full&0xFFF)) > 0xFFF);
    setFlags('C', result > 0xFFFF);
    HL.full = result & 0xFFFF;
    return 8;
}

uint8_t gameboy::OP_x0A(){
    AF.high = read(BC.full);
    return 8;
}

uint8_t gameboy::OP_x0B(){
    BC.full--;
    return 8;
}

uint8_t gameboy::OP_x0C(){
    BC.low++;
    setFlags('Z', BC.low == 0);
    setFlags('N', 0);
    setFlags('H', (BC.low & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x0D(){
    BC.low--;
    setFlags('Z', BC.low == 0);
    setFlags('N', 1);
    setFlags('H', (BC.low & 0x0F) == 0x0F);
    return 4;
}

uint8_t gameboy::OP_x0E(){
    BC.low = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x0F(){
    setFlags('Z', 0);
    setFlags('N', 0);
    setFlags('H', 0),
    setFlags('C', AF.high & 0x01);
    uint8_t oldByte = AF.high;
    AF.high = (oldByte >> 1) | ((oldByte & 0x01) << 7);
    return 4;
}

uint8_t gameboy::OP_x10(){
    pc++;
    stopped = true;
    return 4;
}

uint8_t gameboy::OP_x11(){
    DE.low = read(pc++);
    DE.high = read(pc++);
    return 12;
}

uint8_t gameboy::OP_x12(){
    write(DE.full, AF.high);
    return 8;
}

uint8_t gameboy::OP_x13(){
    DE.full++;
    return 8;
}

uint8_t gameboy::OP_x14(){
    DE.high++;
    setFlags('Z', DE.high == 0);
    setFlags('N', 0);
    setFlags('H', (DE.high & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x15(){
    DE.high--;
    setFlags('Z', DE.high == 0);
    setFlags('N', 1);
    setFlags('H', (DE.high & 0x0F) == 0x0F);
    return 4;
}

uint8_t gameboy::OP_x16(){
    DE.high = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x17(){
    uint8_t currC = getFlags('C');
    setFlags('Z', 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', (AF.high  & 0x80) >> 7);
    AF.high = (AF.high << 1) | currC;
    return 4;
}

uint8_t gameboy::OP_x18(){
    int8_t offset = static_cast<int8_t>(read(pc++));
    pc = pc + offset;
    return 12;
}

uint8_t gameboy::OP_x19(){
    uint32_t result = HL.full + DE.full;
    setFlags('N', 0);
    setFlags('H', ((HL.full&0x0FFF) + (DE.full&0x0FFF)) > 0x0FFF);
    setFlags('C', result > 0xFFFF);
    HL.full = result & 0xFFFF;
    return 8;
}

uint8_t gameboy::OP_x1A(){
    AF.high = read(DE.full);
    return 8;
}

uint8_t gameboy::OP_x1B(){
    DE.full--;
    return 8;
}

uint8_t gameboy::OP_x1C(){
    DE.low++;
    setFlags('Z', DE.low == 0);
    setFlags('N', 0);
    setFlags('H', (DE.low & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x1D(){
    DE.low--;
    setFlags('Z', DE.low == 0);
    setFlags('N', 1);
    setFlags('H', (DE.low & 0x0F) == 0x0F);
    return 4;
}

uint8_t gameboy::OP_x1E(){
    DE.low = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x1F(){
    uint8_t currC = getFlags('C');
    setFlags('Z', 0);
    setFlags('H', 0);
    setFlags('N', 0);
    setFlags('C', AF.high & 0x01);
    AF.high = (AF.high >> 1) | (currC << 7);
    return 4;
}

uint8_t gameboy::OP_x20(){
    if(!getFlags('Z')){
        int8_t offset = static_cast<int8_t>(read(pc++));
        pc = pc + offset;
        return 12;
    }
    else{
        pc++;
        return 8;
    } 
}

uint8_t gameboy::OP_x21(){
    HL.low = read(pc++);
    HL.high = read(pc++);
    return 12;
}

uint8_t gameboy::OP_x22(){
    write(HL.full++, AF.high);
    return 8;
}

uint8_t gameboy::OP_x23(){
    HL.full++;
    return 8;
}

uint8_t gameboy::OP_x24(){
    HL.high++;
    setFlags('Z', HL.high == 0);
    setFlags('N', 0);
    setFlags('H', (HL.high & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x25(){
    HL.high--;
    setFlags('Z', HL.high == 0);
    setFlags('N', 1);
    setFlags('H', (HL.high & 0x0F) == 0x0F);
    return 4;
}

uint8_t gameboy::OP_x26(){
    HL.high = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x27(){
    bool carry = getFlags('C');
    uint8_t adjust = 0;
    if(getFlags('N')){
        if(getFlags('H')) adjust += 6;
        if(getFlags('C')) adjust += 0x60;
        AF.high -= adjust;
    }
    else{
        if(getFlags('H') || ((AF.high & 0x0F) > 9)) adjust += 6;
        if(getFlags('C') || (AF.high > 0x99)){
            adjust += 0x60;
            carry = true;
        } 
        AF.high += adjust;
    }
    setFlags('Z', AF.high == 0);
    setFlags('H', 0);
    setFlags('C', carry);

    return 4;
}

uint8_t gameboy::OP_x28(){
    if(getFlags('Z')){
        int8_t offset = static_cast<int8_t>(read(pc++));
        pc += offset;
        return 12;
    }
    pc++;
    return 8;
}

uint8_t gameboy::OP_x29(){
    uint32_t result = HL.full + HL.full;
    setFlags('N', 0);
    setFlags('H', ((HL.full & 0x0FFF) + (HL.full & 0x0FFF)) > 0x0FFF);
    setFlags('C', result > 0x0FFFF);
    HL.full = result & 0x0FFFF;
    return 8;
}

uint8_t gameboy::OP_x2A(){
    AF.high = read(HL.full++);
    return 8;
}

uint8_t gameboy::OP_x2B(){
    HL.full--;
    return 8;
}

uint8_t gameboy::OP_x2C(){
    HL.low++;
    setFlags('Z', HL.low ==0);
    setFlags('N', 0);
    setFlags('H', (HL.low & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x2D(){
    HL.low--;
    setFlags('Z', HL.low ==0);
    setFlags('N', 1);
    setFlags('H', (HL.low & 0x0F) == 0x0F);
    return 4; 
}

uint8_t gameboy::OP_x2E(){
    HL.low = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x2F(){
    AF.high = ~AF.high;
    setFlags('N', 1);
    setFlags('H', 1);
    return 4;
}

uint8_t gameboy::OP_x30(){
    if(!getFlags('C')){
        int8_t offset = static_cast<int8_t>(read(pc++));
        pc += offset;
        return 12;
    }
    pc++;
    return 8;
}

uint8_t gameboy::OP_x31(){
    uint8_t low = read(pc++);
    uint8_t high = read(pc++);
    sp = (high << 8) | low;
    return 12;
}

uint8_t gameboy::OP_x32(){
    write(HL.full--, AF.high);
    return 8;
}

uint8_t gameboy::OP_x33(){
    sp++;
    return 8;
}

uint8_t gameboy::OP_x34(){
    uint8_t data = read(HL.full);
    data++;
    write(HL.full, data);
    setFlags('Z', data == 0);
    setFlags('N', 0);
    setFlags('H', (data & 0x0F) == 0);
    return 12;
}

uint8_t gameboy::OP_x35(){
    uint8_t data = read(HL.full);
    data--;
    write(HL.full, data);
    setFlags('Z', data == 0);
    setFlags('N', 1);
    setFlags('H', (data & 0x0F) == 0x0F);
    return 12;
}

uint8_t gameboy::OP_x36(){
    uint8_t data = read(pc++);
    write(HL.full, data);
    return 12;
}

uint8_t gameboy::OP_x37(){
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 1);
    return 4;
}

uint8_t gameboy::OP_x38(){
    if(getFlags('C')){
        int8_t offset = static_cast<int8_t>(read(pc++));
        pc += offset;
        return 12;
    }
    pc++;
    return 8;
}

uint8_t gameboy::OP_x39(){
    uint32_t result = HL.full + sp;
    setFlags('N', 0);
    setFlags('H', ((HL.full & 0x0FFF) + (sp & 0x0FFF)) > 0x0FFF);
    setFlags('C', result > 0x0FFFF);
    HL.full = result & 0x0FFFF;
    return 8;
}

uint8_t gameboy::OP_x3A(){
    AF.high = read(HL.full--);
    return 8;
}

uint8_t gameboy::OP_x3B(){
    sp--;
    return 8;
}

uint8_t gameboy::OP_x3C(){
    AF.high++;
    setFlags('Z', AF.high ==0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) == 0);
    return 4;
}

uint8_t gameboy::OP_x3D(){
    AF.high--;
    setFlags('Z', AF.high ==0);
    setFlags('N', 1);
    setFlags('H', (AF.high & 0x0F) == 0x0F);
    return 4;
}

uint8_t gameboy::OP_x3E(){
    AF.high = read(pc++);
    return 8;
}

uint8_t gameboy::OP_x3F(){
    bool currC = getFlags('C');
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', !currC);
    return 4;
}

uint8_t gameboy::OP_x40(){
    BC.high = BC.high;
    return 4;
}

uint8_t gameboy::OP_x41(){
    BC.high = BC.low;
    return 4;
}

uint8_t gameboy::OP_x42(){
    BC.high = DE.high;
    return 4;
}

uint8_t gameboy::OP_x43(){
    BC.high = DE.low;
    return 4;
}

uint8_t gameboy::OP_x44(){
    BC.high = HL.high;
    return 4;
}

uint8_t gameboy::OP_x45(){
    BC.high = HL.low;
    return 4;
}

uint8_t gameboy::OP_x46(){
    BC.high = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x47(){
    BC.high = AF.high;
    return 4;
}

uint8_t gameboy::OP_x48(){
    BC.low = BC.high;
    return 4;
}

uint8_t gameboy::OP_x49(){
    BC.low = BC.low;
    return 4;
}

uint8_t gameboy::OP_x4A(){
    BC.low = DE.high;
    return 4;
}

uint8_t gameboy::OP_x4B(){
    BC.low = DE.low;
    return 4;
}

uint8_t gameboy::OP_x4C(){
    BC.low = HL.high;
    return 4;
}

uint8_t gameboy::OP_x4D(){
    BC.low = HL.low;
    return 4;
}

uint8_t gameboy::OP_x4E(){
    BC.low = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x4F(){
    BC.low = AF.high;
    return 4;
}

uint8_t gameboy::OP_x50(){
    DE.high = BC.high;
    return 4;
}

uint8_t gameboy::OP_x51(){
    DE.high = BC.low;
    return 4;
}

uint8_t gameboy::OP_x52(){
    DE.high = DE.high;
    return 4;
}

uint8_t gameboy::OP_x53(){
    DE.high = DE.low;
    return 4;
}

uint8_t gameboy::OP_x54(){
    DE.high = HL.high;
    return 4;
}

uint8_t gameboy::OP_x55(){
    DE.high = HL.low;
    return 4;
}

uint8_t gameboy::OP_x56(){
    DE.high = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x57(){
    DE.high = AF.high;
    return 4;
}

uint8_t gameboy::OP_x58(){
    DE.low = BC.high;
    return 4;
}

uint8_t gameboy::OP_x59(){
    DE.low = BC.low;
    return 4;
}

uint8_t gameboy::OP_x5A(){
    DE.low = DE.high;
    return 4;
}

uint8_t gameboy::OP_x5B(){
    DE.low = DE.low;
    return 4;
}

uint8_t gameboy::OP_x5C(){
    DE.low = HL.high;
    return 4;
}

uint8_t gameboy::OP_x5D(){
    DE.low = HL.low;
    return 4;
}

uint8_t gameboy::OP_x5E(){
    DE.low = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x5F(){
    DE.low = AF.high;
    return 4;
}

uint8_t gameboy::OP_x60(){
    HL.high = BC.high;
    return 4;
}

uint8_t gameboy::OP_x61(){
    HL.high = BC.low;
    return 4;
}

uint8_t gameboy::OP_x62(){
    HL.high = DE.high;
    return 4;
}

uint8_t gameboy::OP_x63(){
    HL.high = DE.low;
    return 4;
}

uint8_t gameboy::OP_x64(){
    HL.high = HL.high;
    return 4;
}

uint8_t gameboy::OP_x65(){
    HL.high = HL.low;
    return 4;
}

uint8_t gameboy::OP_x66(){
    HL.high = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x67(){
    HL.high = AF.high;
    return 4;
}

uint8_t gameboy::OP_x68(){
    HL.low = BC.high;
    return 4;
}

uint8_t gameboy::OP_x69(){
    HL.low = BC.low;
    return 4;
}

uint8_t gameboy::OP_x6A(){
    HL.low = DE.high;
    return 4;
}

uint8_t gameboy::OP_x6B(){
    HL.low = DE.low;
    return 4;
}

uint8_t gameboy::OP_x6C(){
    HL.low = HL.high;
    return 4;
}

uint8_t gameboy::OP_x6D(){
    HL.low = HL.low;
    return 4;
}

uint8_t gameboy::OP_x6E(){
    HL.low = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x6F(){
    HL.low = AF.high;
    return 4;
}

uint8_t gameboy::OP_x70(){
    write(HL.full, BC.high);
    return 8;
}

uint8_t gameboy::OP_x71(){
    write(HL.full, BC.low);
    return 8;
}

uint8_t gameboy::OP_x72(){
    write(HL.full, DE.high);
    return 8;
}

uint8_t gameboy::OP_x73(){
    write(HL.full, DE.low);
    return 8;
}

uint8_t gameboy::OP_x74(){
    write(HL.full, HL.high);
    return 8;
}

uint8_t gameboy::OP_x75(){
    write(HL.full, HL.low);
    return 8;
}

uint8_t gameboy::OP_x76(){
    halted = true;
    return 4;
}

uint8_t gameboy::OP_x77(){
    write(HL.full, AF.high);
    return 8;
}

uint8_t gameboy::OP_x78(){
    AF.high = BC.high;
    return 4;
}

uint8_t gameboy::OP_x79(){
    AF.high = BC.low;
    return 4;
}

uint8_t gameboy::OP_x7A(){
    AF.high = DE.high;
    return 4;
}

uint8_t gameboy::OP_x7B(){
    AF.high = DE.low;
    return 4;
}

uint8_t gameboy::OP_x7C(){
    AF.high = HL.high;
    return 4;
}

uint8_t gameboy::OP_x7D(){
    AF.high = HL.low;
    return 4;
}

uint8_t gameboy::OP_x7E(){
    AF.high = read(HL.full);
    return 8;
}

uint8_t gameboy::OP_x7F(){
    AF.high = AF.high;
    return 4;
}

uint8_t gameboy::OP_x80(){
    uint8_t value = BC.high;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x81(){
    uint8_t value = BC.low;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x82(){
    uint8_t value = DE.high;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x83(){
    uint8_t value = DE.low;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x84(){
    uint8_t value = HL.high;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x85(){
    uint8_t value = HL.low;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x86(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_x87(){
    uint8_t value = AF.high;
    uint16_t result = AF.high + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x88(){
    uint8_t value = BC.high;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x89(){
    uint8_t value = BC.low;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x8A(){
    uint8_t value = DE.high;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x8B(){
    uint8_t value = DE.low;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x8C(){
    uint8_t value = HL.high;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x8D(){
    uint8_t value = HL.low;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x8E(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_x8F(){
    uint8_t value = AF.high;
    uint16_t result = AF.high + getFlags('C') + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', getFlags('C') + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x90(){
    uint8_t value = BC.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x91(){
    uint8_t value = BC.low;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x92(){
    uint8_t value = DE.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x93(){
    uint8_t value = DE.low;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x94(){
    uint8_t value = HL.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x95(){
    uint8_t value = HL.low;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x96(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_x97(){
    uint8_t value = AF.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x98(){
    uint8_t value = BC.high;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x99(){
    uint8_t value = BC.low;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x9A(){
    uint8_t value = DE.high;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x9B(){
    uint8_t value = DE.low;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x9C(){
    uint8_t value = HL.high;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x9D(){
    uint8_t value = HL.low;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_x9E(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_x9F(){
    uint8_t value = AF.high;
    uint16_t result = AF.high - value - getFlags('C');
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + getFlags('C') > (AF.high & 0x0F));
    setFlags('C', (value+getFlags('C')) > AF.high);
    AF.high = result & 0xFF;
    return 4;
}

uint8_t gameboy::OP_xA0(){
    uint8_t value = BC.high;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA1(){
    uint8_t value = BC.low;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA2(){
    uint8_t value = DE.high;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA3(){
    uint8_t value = DE.low;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA4(){
    uint8_t value = HL.high;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA5(){
    uint8_t value = HL.low;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA6(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 8;
}

uint8_t gameboy::OP_xA7(){
    uint8_t value = AF.high;
    uint16_t result = AF.high & value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA8(){
    uint8_t value = BC.high;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xA9(){
    uint8_t value = BC.low;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xAA(){
    uint8_t value = DE.high;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xAB(){
    uint8_t value = DE.low;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xAC(){
    uint8_t value = HL.high;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xAD(){
    uint8_t value = HL.low;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xAE(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 8;
}

uint8_t gameboy::OP_xAF(){
    uint8_t value = AF.high;
    uint16_t result = AF.high ^ value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB0(){
    uint8_t value = BC.high;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB1(){
    uint8_t value = BC.low;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB2(){
    uint8_t value = DE.high;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB3(){
    uint8_t value = DE.low;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB4(){
    uint8_t value = HL.high;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB5(){
    uint8_t value = HL.low;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB6(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 8;
}

uint8_t gameboy::OP_xB7(){
    uint8_t value = AF.high;
    uint16_t result = AF.high | value;
    setFlags('Z', result == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result;
    return 4;
}

uint8_t gameboy::OP_xB8(){
    uint8_t value = BC.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xB9(){
    uint8_t value = BC.low;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xBA(){
    uint8_t value = DE.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xBB(){
    uint8_t value = DE.low;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xBC(){
    uint8_t value = HL.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xBD(){
    uint8_t value = HL.low;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xBE(){
    uint8_t value = read(HL.full);
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 8;
}

uint8_t gameboy::OP_xBF(){
    uint8_t value = AF.high;
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 4;
}

uint8_t gameboy::OP_xC0(){
    if(!getFlags('Z')){
        uint8_t high, low;
        low = read(sp++);
        high = read(sp++);
        pc = (high << 8) | low;
        return 20;
    }
    return 8;
}

uint8_t gameboy::OP_xC1(){
        uint8_t high, low;
        low = read(sp++);
        high = read(sp++);
        BC.full = (high << 8) | low;
        return 12;
}

uint8_t gameboy::OP_xC2(){
    if(!getFlags('Z')){
        uint8_t high, low;
        low = read(pc++);
        high = read(pc++);
        pc = (high << 8) | low;
        return 16;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xC3(){
    uint8_t high, low;
    low = read(pc++);
    high = read(pc++);
    pc = (high << 8) | low;
    return 16;
}

uint8_t gameboy::OP_xC4(){
    if(!getFlags('Z')){
        uint8_t high, low;
        low = read(pc++);
        high = read(pc++);
        uint16_t address = (high << 8) | low;

        write(--sp, (pc >> 8) & 0xFF); //high byte
        write(--sp, pc & 0xFF);
        pc = address;
        return 24;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xC5(){
    write(--sp, BC.high);
    write(--sp, BC.low);
    return 16;
}

uint8_t gameboy::OP_xC6(){
    uint8_t data = read(pc++);
    uint16_t value = AF.high + data;
    setFlags('Z', (value&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', (AF.high & 0x0F) + (data & 0x0F) > 0x0F);
    setFlags('C', value > 0xFF);
    AF.high = value & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xC7(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0000;
    return 16;
}

uint8_t gameboy::OP_xC8(){
    if(getFlags('Z')){
        uint8_t low, high;
        low = read(sp++);
        high = read(sp++);
        pc = (high << 8) | low;
        return 20;
    }
    return 8;
}

uint8_t gameboy::OP_xC9(){
    uint8_t low, high;
    low = read(sp++);
    high = read(sp++);
    pc = (high << 8) | low;
    return 16;
}

uint8_t gameboy::OP_xCA(){
    if(getFlags('Z')){
        uint8_t low, high;
        low = read(pc++);
        high = read(pc++);
        pc = (high << 8) | low;
        return 16;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xCB(){
    opcode = read(pc++);
    return (this->*preTable[opcode])();
}

uint8_t gameboy::OP_xCC(){
    if(getFlags('Z')){
        uint8_t low, high;
        low = read(pc++);
        high = read(pc++);
        uint16_t address = (high << 8) | low;

        write(--sp, (pc >> 8) & 0xFF);
        write(--sp, pc & 0xFF);
        pc = address;
        return 24;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xCD(){
    uint8_t low, high;
    low = read(pc++);
    high = read(pc++);
    uint16_t address = (high << 8) | low;

    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, (pc & 0x00FF));
    pc = address;
    return 24;
}

uint8_t gameboy::OP_xCE(){
    uint8_t value = read(pc++);
    uint8_t c = getFlags('C');
    uint16_t result = AF.high + c + value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', c + (AF.high & 0x0F) + (value & 0x0F) > 0x0F);
    setFlags('C', result > 0xFF);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xCF(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0008;
    return 16;
}

uint8_t gameboy::OP_xD0(){
    if(!getFlags('C')){
        uint8_t high, low;
        low = read(sp++);
        high = read(sp++);
        pc = (high << 8) | low;
        return 20;
    }
    return 8;
}

uint8_t gameboy::OP_xD1(){
        uint8_t high, low;
        low = read(sp++);
        high = read(sp++);
        DE.full = (high << 8) | low;
        return 12;
}

uint8_t gameboy::OP_xD2(){
    if(!getFlags('C')){
        uint8_t high, low;
        low = read(pc++);
        high = read(pc++);
        pc = (high << 8) | low;
        return 16;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xD4(){
    if(!getFlags('C')){
        uint8_t low, high;
        low = read(pc++);
        high = read(pc++);
        uint16_t address = (high << 8) | low;

        write(--sp, (pc >> 8) & 0xFF);
        write(--sp, pc & 0xFF);
        pc = address;

        return 24;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xD5(){
    write(--sp, DE.high);
    write(--sp, DE.low);
    return 16;
}

uint8_t gameboy::OP_xD6(){
    uint8_t value = read(pc++);
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xD7(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0010;
    return 16;
}

uint8_t gameboy::OP_xD8(){
    if(getFlags('C')){
        uint8_t low, high;
        low = read(sp++);
        high = read(sp++);
        pc = (high << 8) | low;
        return 20;
    }
    return 8;
}

uint8_t gameboy::OP_xD9(){
    uint8_t low, high;
    low = read(sp++);
    high = read(sp++);
    pc = (high << 8) | low;

    IME = true;


    return 16;
}

uint8_t gameboy::OP_xDA(){
    if(getFlags('C')){
        uint8_t low, high;
        low = read(pc++);
        high = read(pc++);
        pc = (high << 8) | low;
        return 16;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xDC(){
    if(getFlags('C')){
        uint8_t low, high;
        low = read(pc++);
        high = read(pc++);
        uint16_t address = (high << 8) | low;

        write(--sp, (pc >> 8) & 0xFF);
        write(--sp, pc & 0xFF);
        pc = address;
        return 24;
    }
    pc += 2;
    return 12;
}

uint8_t gameboy::OP_xDE(){
    uint8_t value = read(pc++);
    uint8_t c = getFlags('C');
    uint16_t result = AF.high - (value+c);
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) + c > (AF.high & 0x0F));
    setFlags('C', (value+c) > AF.high);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xDF(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0018;
    return 16;
}
//for illegal ops, should just cycle 4 ///////////////////////////////////////////////
uint8_t gameboy::OP_xE0(){
    uint8_t offset = read(pc++);
    write(0xFF00 + offset, AF.high);
    return 12;
}

uint8_t gameboy::OP_xE1(){
        uint8_t high, low;
        low = read(sp++);
        high = read(sp++);
        HL.full = (high << 8) | low;
        return 12;
}

uint8_t gameboy::OP_xE2(){
    write(0xFF00 + BC.low, AF.high);
    return 8;
}

uint8_t gameboy::OP_xE5(){
    write(--sp, HL.high);
    write(--sp, HL.low);
    return 16;
}

uint8_t gameboy::OP_xE6(){
    uint8_t value = read(pc++);
    uint8_t result = AF.high & value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', 1);
    setFlags('C', 0);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xE7(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0020;
    return 16;
}

uint8_t gameboy::OP_xE8(){
    int8_t value = read(pc++);
    setFlags('Z', 0);
    setFlags('N', 0);
    setFlags('H', (sp & 0x0F) + ((uint8_t)value & 0x0F) > 0x0F);
    setFlags('C', (sp & 0xFF) + (uint8_t)value > 0xFF);
    sp += value;
    return 16;
}

uint8_t gameboy::OP_xE9(){
    pc = HL.full;
    return 4;
}

uint8_t gameboy::OP_xEA(){
    uint8_t offsetLow = read(pc++);
    uint8_t offsetHigh = read(pc++);
    uint16_t address = (offsetHigh << 8) | offsetLow;
    write(address, AF.high);
    return 16;
}

uint8_t gameboy::OP_xEE(){
    uint8_t value = read(pc++);
    uint8_t result = AF.high ^ value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xEF(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0028;
    return 16;
}

uint8_t gameboy::OP_xF0(){
    uint8_t offset = read(pc++);
    AF.high = read(0xFF00 + offset);
    return 12;
}

uint8_t gameboy::OP_xF1(){
    uint8_t high, low;
    low = read(sp++);
    high = read(sp++);
    AF.full = (high << 8) | low;
    AF.low &= 0xF0;
    return 12;
}

uint8_t gameboy::OP_xF2(){
    AF.high = read(0xFF00 + BC.low);
    return 8;
}

uint8_t gameboy::OP_xF3(){
    IME = false;
    return 4;
}

uint8_t gameboy::OP_xF5(){
    write(--sp, AF.high);
    write(--sp, AF.low);
    return 16;
}

uint8_t gameboy::OP_xF6(){
    uint8_t value = read(pc++);
    uint8_t result = AF.high | value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    AF.high = result & 0xFF;
    return 8;
}

uint8_t gameboy::OP_xF7(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0030;
    return 16;
}

uint8_t gameboy::OP_xF8(){
    int8_t value = static_cast<int8_t>(read(pc++));
    setFlags('Z', 0);
    setFlags('N', 0);
    setFlags('H', (sp & 0x0F) + ((uint8_t)value & 0x0F) > 0x0F);
    setFlags('C', (sp & 0xFF) + (uint8_t)value > 0xFF);
    HL.full = sp + value;
    return 12;
}

uint8_t gameboy::OP_xF9(){
    sp = HL.full;
    return 8;
}

uint8_t gameboy::OP_xFA(){
    uint8_t offsetLow = read(pc++);
    uint8_t offsetHigh = read(pc++);
    uint16_t address = (offsetHigh << 8) | offsetLow;
    AF.high = read(address);
    return 16;
}

uint8_t gameboy::OP_xFB(){
    futIME = true;
    return 4;
}

uint8_t gameboy::OP_xFE(){
    uint8_t value = read(pc++);
    uint16_t result = AF.high - value;
    setFlags('Z', (result&0xFF) == 0);
    setFlags('N', 1);
    setFlags('H', (value & 0x0F) > (AF.high & 0x0F));
    setFlags('C', value > AF.high);
    return 8;
}

uint8_t gameboy::OP_xFF(){
    write(--sp, (pc >> 8) & 0xFF);
    write(--sp, pc & 0xFF);
    pc = 0x0038;
    return 16;
}

//****************************PREFIX ************************************/
uint8_t gameboy::PRE_x00(){
    uint8_t nextC = (BC.high & 0x80) >> 7;
    uint8_t value = (BC.high << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x01(){
    uint8_t nextC = (BC.low & 0x80) >> 7;
    uint8_t value = (BC.low << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x02(){
    uint8_t nextC = (DE.high & 0x80) >> 7;
    uint8_t value = (DE.high << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x03(){
    uint8_t nextC = (DE.low & 0x80) >> 7;
    uint8_t value = (DE.low << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x04(){
    uint8_t nextC = (HL.high & 0x80) >> 7;
    uint8_t value = (HL.high << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x05(){
    uint8_t nextC = (HL.low & 0x80) >> 7;
    uint8_t value = (HL.low << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x06(){
    uint8_t currVal = read(HL.full);
    uint8_t nextC = (currVal & 0x80) >> 7;
    uint8_t value = (currVal << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x07(){
    uint8_t nextC = (AF.high & 0x80) >> 7;
    uint8_t value = (AF.high << 1) | nextC;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x08(){
    uint8_t nextC = (BC.high & 0x01);
    uint8_t value = (BC.high >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x09(){
    uint8_t nextC = (BC.low & 0x01);
    uint8_t value = (BC.low >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x0A(){
    uint8_t nextC = (DE.high & 0x01);
    uint8_t value = (DE.high >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x0B(){
    uint8_t nextC = (DE.low & 0x01);
    uint8_t value = (DE.low >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x0C(){
    uint8_t nextC = (HL.high & 0x01);
    uint8_t value = (HL.high >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x0D(){
    uint8_t nextC = (HL.low & 0x01);
    uint8_t value = (HL.low >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x0E(){
    uint8_t currVal = read(HL.full);
    uint8_t nextC = (currVal & 0x01);
    uint8_t value = (currVal >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x0F(){
    uint8_t nextC = (AF.high & 0x01);
    uint8_t value = (AF.high >> 1) | (nextC<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x10(){
    uint8_t nextC = (BC.high & 0x80) >> 7;
    uint8_t value = (BC.high << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x11(){
    uint8_t nextC = (BC.low & 0x80) >> 7;
    uint8_t value = (BC.low << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x12(){
    uint8_t nextC = (DE.high & 0x80) >> 7;
    uint8_t value = (DE.high << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x13(){
    uint8_t nextC = (DE.low & 0x80) >> 7;
    uint8_t value = (DE.low << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x14(){
    uint8_t nextC = (HL.high & 0x80) >> 7;
    uint8_t value = (HL.high << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x15(){
    uint8_t nextC = (HL.low & 0x80) >> 7;
    uint8_t value = (HL.low << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x16(){
    uint8_t currVal = read(HL.full);
    uint8_t nextC = (currVal & 0x80) >> 7;
    uint8_t value = (currVal << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x17(){
    uint8_t nextC = (AF.high & 0x80) >> 7;
    uint8_t value = (AF.high << 1) | getFlags('C');
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x18(){
    uint8_t nextC = (BC.high & 0x01);
    uint8_t value = (BC.high >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x19(){
    uint8_t nextC = (BC.low & 0x01);
    uint8_t value = (BC.low >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x1A(){
    uint8_t nextC = (DE.high & 0x01);
    uint8_t value = (DE.high >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x1B(){
    uint8_t nextC = (DE.low & 0x01);
    uint8_t value = (DE.low >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x1C(){
    uint8_t nextC = (HL.high & 0x01);
    uint8_t value = (HL.high >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x1D(){
    uint8_t nextC = (HL.low & 0x01);
    uint8_t value = (HL.low >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x1E(){
    uint8_t currVal = read(HL.full);
    uint8_t nextC = (currVal & 0x01);
    uint8_t value = (currVal >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x1F(){
    uint8_t nextC = (AF.high & 0x01);
    uint8_t value = (AF.high >> 1) | (getFlags('C')<<7);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x20(){
    uint8_t nextC = (BC.high & 0x80) >> 7;
    uint8_t value = (BC.high << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x21(){
    uint8_t nextC = (BC.low & 0x80) >> 7;
    uint8_t value = (BC.low << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x22(){
    uint8_t nextC = (DE.high & 0x80) >> 7;
    uint8_t value = (DE.high << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x23(){
    uint8_t nextC = (DE.low & 0x80) >> 7;
    uint8_t value = (DE.low << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x24(){
    uint8_t nextC = (HL.high & 0x80) >> 7;
    uint8_t value = (HL.high << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x25(){
    uint8_t nextC = (HL.low & 0x80) >> 7;
    uint8_t value = (HL.low << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x26(){
    uint8_t currVal = read(HL.full);
    uint8_t nextC = (currVal & 0x80) >> 7;
    uint8_t value = (currVal << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x27(){
    uint8_t nextC = (AF.high & 0x80) >> 7;
    uint8_t value = (AF.high << 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x28(){
    uint8_t high = BC.high & 0x80;
    uint8_t nextC = (BC.high & 0x01);
    uint8_t value = (BC.high >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x29(){
    uint8_t high = BC.low & 0x80;
    uint8_t nextC = (BC.low & 0x01);
    uint8_t value = (BC.low >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x2A(){
    uint8_t high = DE.high & 0x80;
    uint8_t nextC = (DE.high & 0x01);
    uint8_t value = (DE.high >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x2B(){
    uint8_t high = DE.low & 0x80;
    uint8_t nextC = (DE.low & 0x01);
    uint8_t value = (DE.low >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x2C(){
    uint8_t high = HL.high & 0x80;
    uint8_t nextC = (HL.high & 0x01);
    uint8_t value = (HL.high >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x2D(){
    uint8_t high = HL.low & 0x80;
    uint8_t nextC = (HL.low & 0x01);
    uint8_t value = (HL.low >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x2E(){
    uint8_t currVal = read(HL.full);
    uint8_t high = currVal & 0x80;
    uint8_t nextC = (currVal & 0x01);
    uint8_t value = (currVal >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x2F(){
    uint8_t high = AF.high & 0x80;
    uint8_t nextC = (AF.high & 0x01);
    uint8_t value = (AF.high >> 1) | high;
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x30(){
    uint8_t low = BC.high & 0x0F;
    BC.high = ((BC.high & 0xF0) >> 4) | (low << 4);
    setFlags('Z', BC.high == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x31(){
    uint8_t low = BC.low & 0x0F;
    BC.low = ((BC.low & 0xF0) >> 4) | (low << 4);
    setFlags('Z', BC.low == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x32(){
    uint8_t low = DE.high & 0x0F;
    DE.high = ((DE.high & 0xF0) >> 4) | (low << 4);
    setFlags('Z', DE.high == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x33(){
    uint8_t low = DE.low & 0x0F;
    DE.low = ((DE.low & 0xF0) >> 4) | (low << 4);
    setFlags('Z', DE.low == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x34(){
    uint8_t low = HL.high & 0x0F;
    HL.high = ((HL.high & 0xF0) >> 4) | (low << 4);
    setFlags('Z', HL.high == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x35(){
    uint8_t low = HL.low & 0x0F;
    HL.low = ((HL.low & 0xF0) >> 4) | (low << 4);
    setFlags('Z', HL.low == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x36(){
    uint8_t currVal = read(HL.full);
    uint8_t low = currVal & 0x0F;
    uint8_t value = ((currVal & 0xF0) >> 4) | (low << 4);
    write(HL.full, value);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 16;
}

uint8_t gameboy::PRE_x37(){
    uint8_t low = AF.high & 0x0F;
    AF.high = ((AF.high & 0xF0) >> 4) | (low << 4);
    setFlags('Z', AF.high == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', 0);
    return 8;
}

uint8_t gameboy::PRE_x38(){
    uint8_t nextC = (BC.high & 0x01);
    uint8_t value = (BC.high >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.high = value;
    return 8;
}

uint8_t gameboy::PRE_x39(){
    uint8_t nextC = (BC.low & 0x01);
    uint8_t value = (BC.low >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    BC.low = value;
    return 8;
}

uint8_t gameboy::PRE_x3A(){
    uint8_t nextC = (DE.high & 0x01);
    uint8_t value = (DE.high >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.high = value;
    return 8;
}

uint8_t gameboy::PRE_x3B(){
    uint8_t nextC = (DE.low & 0x01);
    uint8_t value = (DE.low >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    DE.low = value;
    return 8;
}

uint8_t gameboy::PRE_x3C(){
    uint8_t nextC = (HL.high & 0x01);
    uint8_t value = (HL.high >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.high = value;
    return 8;
}

uint8_t gameboy::PRE_x3D(){
    uint8_t nextC = (HL.low & 0x01);
    uint8_t value = (HL.low >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    HL.low = value;
    return 8;
}

uint8_t gameboy::PRE_x3E(){
    uint8_t currVal = read(HL.full);
    uint8_t nextC = (currVal & 0x01);
    uint8_t value = (currVal >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    write(HL.full, value);
    return 16;
}

uint8_t gameboy::PRE_x3F(){
    uint8_t nextC = (AF.high & 0x01);
    uint8_t value = (AF.high >> 1);
    setFlags('Z', value == 0);
    setFlags('N', 0);
    setFlags('H', 0);
    setFlags('C', nextC);
    AF.high = value;
    return 8;
}

uint8_t gameboy::PRE_x40(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x41(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x42(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x43(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x44(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x45(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x46(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x47(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x01));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x48(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x49(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x4A(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x4B(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x4C(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x4D(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x4E(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x4F(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x02));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x50(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x51(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x52(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x53(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x54(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x55(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x56(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x57(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x04));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x58(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x59(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x5A(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x5B(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x5C(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x5D(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x5E(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x5F(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x08));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x60(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x61(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x62(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x63(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x64(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x65(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x66(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x67(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x10));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x68(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x69(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x6A(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x6B(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x6C(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x6D(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x6E(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x6F(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x20));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x70(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x71(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x72(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x73(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x74(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x75(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x76(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x77(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x40));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x78(){
    uint8_t value = BC.high;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x79(){
    uint8_t value = BC.low;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x7A(){
    uint8_t value = DE.high;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x7B(){
    uint8_t value = DE.low;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x7C(){
    uint8_t value = HL.high;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x7D(){
    uint8_t value = HL.low;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x7E(){
    uint8_t value = read(HL.full);
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 12;
}

uint8_t gameboy::PRE_x7F(){
    uint8_t value = AF.high;
    setFlags('Z', !(value&0x80));
    setFlags('N', 0);
    setFlags('H', 1);
    return 8;
}

uint8_t gameboy::PRE_x80(){
    BC.high &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x81(){
    BC.low &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x82(){
    DE.high &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x83(){
    DE.low &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x84(){
    HL.high &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x85(){
    HL.low &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x86(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xFE);
    return 16;
}

uint8_t gameboy::PRE_x87(){
    AF.high &= 0xFE;
    return 8;
}

uint8_t gameboy::PRE_x88(){
    BC.high &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x89(){
    BC.low &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x8A(){
    DE.high &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x8B(){
    DE.low &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x8C(){
    HL.high &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x8D(){
    HL.low &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x8E(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xFD);
    return 16;
}

uint8_t gameboy::PRE_x8F(){
    AF.high &= 0xFD;
    return 8;
}

uint8_t gameboy::PRE_x90(){
    BC.high &= 0xFB; //1011
    return 8;
}

uint8_t gameboy::PRE_x91(){
    BC.low &= 0xFB;
    return 8;
}

uint8_t gameboy::PRE_x92(){
    DE.high &= 0xFB;
    return 8;
}

uint8_t gameboy::PRE_x93(){
    DE.low &= 0xFB;
    return 8;
}

uint8_t gameboy::PRE_x94(){
    HL.high &= 0xFB;
    return 8;
}

uint8_t gameboy::PRE_x95(){
    HL.low &= 0xFB;
    return 8;
}

uint8_t gameboy::PRE_x96(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xFB);
    return 16;
}

uint8_t gameboy::PRE_x97(){
    AF.high &= 0xFB;
    return 8;
}

uint8_t gameboy::PRE_x98(){
    BC.high &= 0xF7; //
    return 8;
}

uint8_t gameboy::PRE_x99(){
    BC.low &= 0xF7;
    return 8;
}

uint8_t gameboy::PRE_x9A(){
    DE.high &= 0xF7;
    return 8;
}

uint8_t gameboy::PRE_x9B(){
    DE.low &= 0xF7;
    return 8;
}

uint8_t gameboy::PRE_x9C(){
    HL.high &= 0xF7;
    return 8;
}

uint8_t gameboy::PRE_x9D(){
    HL.low &= 0xF7;
    return 8;
}

uint8_t gameboy::PRE_x9E(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xF7);
    return 16;
}

uint8_t gameboy::PRE_x9F(){
    AF.high &= 0xF7;
    return 8;
}

uint8_t gameboy::PRE_xA0(){
    BC.high &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA1(){
    BC.low &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA2(){
    DE.high &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA3(){
    DE.low &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA4(){
    HL.high &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA5(){
    HL.low &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA6(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xEF);
    return 16;
}

uint8_t gameboy::PRE_xA7(){
    AF.high &= 0xEF;
    return 8;
}

uint8_t gameboy::PRE_xA8(){
    BC.high &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xA9(){
    BC.low &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xAA(){
    DE.high &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xAB(){
    DE.low &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xAC(){
    HL.high &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xAD(){
    HL.low &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xAE(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xDF);
    return 16;
}

uint8_t gameboy::PRE_xAF(){
    AF.high &= 0xDF;
    return 8;
}

uint8_t gameboy::PRE_xB0(){
    BC.high &= 0xBF; //1011
    return 8;
}

uint8_t gameboy::PRE_xB1(){
    BC.low &= 0xBF;
    return 8;
}

uint8_t gameboy::PRE_xB2(){
    DE.high &= 0xBF;
    return 8;
}

uint8_t gameboy::PRE_xB3(){
    DE.low &= 0xBF;
    return 8;
}

uint8_t gameboy::PRE_xB4(){
    HL.high &= 0xBF;
    return 8;
}

uint8_t gameboy::PRE_xB5(){
    HL.low &= 0xBF;
    return 8;
}

uint8_t gameboy::PRE_xB6(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0xBF);
    return 16;
}

uint8_t gameboy::PRE_xB7(){
    AF.high &= 0xBF;
    return 8;
}

uint8_t gameboy::PRE_xB8(){
    BC.high &= 0x7F; //
    return 8;
}

uint8_t gameboy::PRE_xB9(){
    BC.low &= 0x7F;
    return 8;
}

uint8_t gameboy::PRE_xBA(){
    DE.high &= 0x7F;
    return 8;
}

uint8_t gameboy::PRE_xBB(){
    DE.low &= 0x7F;
    return 8;
}

uint8_t gameboy::PRE_xBC(){
    HL.high &= 0x7F;
    return 8;
}

uint8_t gameboy::PRE_xBD(){
    HL.low &= 0x7F;
    return 8;
}

uint8_t gameboy::PRE_xBE(){
    uint8_t value = read(HL.full);
    write(HL.full, value&0x7F);
    return 16;
}

uint8_t gameboy::PRE_xBF(){
    AF.high &= 0x7F;
    return 8;
}

uint8_t gameboy::PRE_xC0(){
    BC.high |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC1(){
    BC.low |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC2(){
    DE.high |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC3(){
    DE.low |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC4(){
    HL.high |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC5(){
    HL.low |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC6(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xFE));
    return 16;
}

uint8_t gameboy::PRE_xC7(){
    AF.high |= ~0xFE;
    return 8;
}

uint8_t gameboy::PRE_xC8(){
    BC.high |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xC9(){
    BC.low |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xCA(){
    DE.high |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xCB(){
    DE.low |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xCC(){
    HL.high |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xCD(){
    HL.low |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xCE(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xFD));
    return 16;
}

uint8_t gameboy::PRE_xCF(){
    AF.high |= ~0xFD;
    return 8;
}

uint8_t gameboy::PRE_xD0(){
    BC.high |= ~0xFB; //1011
    return 8;
}

uint8_t gameboy::PRE_xD1(){
    BC.low |= ~0xFB;
    return 8;
}

uint8_t gameboy::PRE_xD2(){
    DE.high |= ~0xFB;
    return 8;
}

uint8_t gameboy::PRE_xD3(){
    DE.low |= ~0xFB;
    return 8;
}

uint8_t gameboy::PRE_xD4(){
    HL.high |= ~0xFB;
    return 8;
}

uint8_t gameboy::PRE_xD5(){
    HL.low |= ~0xFB;
    return 8;
}

uint8_t gameboy::PRE_xD6(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xFB));
    return 16;
}

uint8_t gameboy::PRE_xD7(){
    AF.high |= ~0xFB;
    return 8;
}

uint8_t gameboy::PRE_xD8(){
    BC.high |= ~0xF7; //
    return 8;
}

uint8_t gameboy::PRE_xD9(){
    BC.low |= ~0xF7;
    return 8;
}

uint8_t gameboy::PRE_xDA(){
    DE.high |= ~0xF7;
    return 8;
}

uint8_t gameboy::PRE_xDB(){
    DE.low |= ~0xF7;
    return 8;
}

uint8_t gameboy::PRE_xDC(){
    HL.high |= ~0xF7;
    return 8;
}

uint8_t gameboy::PRE_xDD(){
    HL.low |= ~0xF7;
    return 8;
}

uint8_t gameboy::PRE_xDE(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xF7));
    return 16;
}

uint8_t gameboy::PRE_xDF(){
    AF.high |= ~0xF7;
    return 8;
}

uint8_t gameboy::PRE_xE0(){
    BC.high |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE1(){
    BC.low |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE2(){
    DE.high |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE3(){
    DE.low |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE4(){
    HL.high |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE5(){
    HL.low |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE6(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xEF));
    return 16;
}

uint8_t gameboy::PRE_xE7(){
    AF.high |= ~0xEF;
    return 8;
}

uint8_t gameboy::PRE_xE8(){
    BC.high |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xE9(){
    BC.low |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xEA(){
    DE.high |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xEB(){
    DE.low |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xEC(){
    HL.high |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xED(){
    HL.low |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xEE(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xDF));
    return 16;
}

uint8_t gameboy::PRE_xEF(){
    AF.high |= ~0xDF;
    return 8;
}

uint8_t gameboy::PRE_xF0(){
    BC.high |= ~0xBF; //1011
    return 8;
}

uint8_t gameboy::PRE_xF1(){
    BC.low |= ~0xBF;
    return 8;
}

uint8_t gameboy::PRE_xF2(){
    DE.high |= ~0xBF;
    return 8;
}

uint8_t gameboy::PRE_xF3(){
    DE.low |= ~0xBF;
    return 8;
}

uint8_t gameboy::PRE_xF4(){
    HL.high |= ~0xBF;
    return 8;
}

uint8_t gameboy::PRE_xF5(){
    HL.low |= ~0xBF;
    return 8;
}

uint8_t gameboy::PRE_xF6(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0xBF));
    return 16;
}

uint8_t gameboy::PRE_xF7(){
    AF.high |= ~0xBF;
    return 8;
}

uint8_t gameboy::PRE_xF8(){
    BC.high |= ~0x7F; //
    return 8;
}

uint8_t gameboy::PRE_xF9(){
    BC.low |= ~0x7F;
    return 8;
}

uint8_t gameboy::PRE_xFA(){
    DE.high |= ~0x7F;
    return 8;
}

uint8_t gameboy::PRE_xFB(){
    DE.low |= ~0x7F;
    return 8;
}

uint8_t gameboy::PRE_xFC(){
    HL.high |= ~0x7F;
    return 8;
}

uint8_t gameboy::PRE_xFD(){
    HL.low |= ~0x7F;
    return 8;
}

uint8_t gameboy::PRE_xFE(){
    uint8_t value = read(HL.full);
    write(HL.full, value|(~0x7F));
    return 16;
}

uint8_t gameboy::PRE_xFF(){
    AF.high |= ~0x7F;
    return 8;
}