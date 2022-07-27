#include <cstdio>
#include "6502_cpu.h"

//https://web.archive.org/web/20210604074847/http://obelisk.me.uk/6502/
using namespace MOS6502;

int main(){

    Memory mem{};
    CPU cpu{};
    cpu.Reset(mem);

    /*
     *
     * global _start            ; set reset vector to location of _start
     *       _start:            ; at 0x8000
     *              ;set value in memory which will be later loaded into A register
     *           LDY 0x15       ; {instruction at 0x8000}   load value into Y register
     *           STY #0x4312    ; {instruction at 0x8002}   store X register into 0x4312
     *
     *              ;set LSB for indirect address
     *           LDX 0x12       ; {instruction at 0x8005}   load 0x12 into X register
     *           STX 0x23       ; {instruction at 0x8007}   store X register into 0x23
     *
     *              ;set MSB for indirect address
     *           LDX 0x43       ; {instruction at 0x8009}   load 0x43 into X register
     *           STX 0x24       ; {instruction at 0x800B}   store X register int 0x24
     *
     *              ;indirect address is now stored at 2 cells: 0x23 (LSB) and 0x24 (MSB)
     *              ; load indirect address into A register
     *           LDX 0x13       ; {instruction at 0x800D}   load 0x13 into X register - offset for zero-page address
     *           LDA ($0x10, X) ; {instruction at 0x800F}   load Accumulator from indirect address X(0x13) + 0x10 -> 0x0023 and 0x0024
     *
     * Assuming that cell 0x0022 = 0x12 and 0x0023 = 0x83 with little endian conception we will get 16-bit absolute address 0x8312
     * Now assuming that cell 0x8312 = 0x15 we will load 0x15 into accumulator
     *
    * */

    //reset vector
    mem[0xFFFC] = 0x00;
    mem[0xFFFD] = 0x80; //location of the first instruction (little endian)

    //set value in memory which will be later loaded into A register
    mem[0x8000] = CPU::INSTRUCTIONS::INS_LDY_IM; // 2 cycles
    mem[0x8001] = 0x15; //this value will be stored in ACC register by indirect address
    mem[0x8002] = CPU::INSTRUCTIONS::INS_STY_ABS; //4 cycles
    mem[0x8003] = 0x12;
    mem[0x8004] = 0x43; //Y register will be stored at 0x4312

    //set LSB for indirect address
    mem[0x8005] = CPU::INS_LDX_IM; // 2 cycles
    mem[0x8006] = 0x12;     //set  LSB
    mem[0x8007] = CPU::INS_STX_ZP;  //3 cycles
    mem[0x8008] = 0x23;    //store LSB in 0x22

    //set MSB for indirect address
    mem[0x8009] = CPU::INS_LDX_IM; // 2 cycles
    mem[0x800A] = 0x43;     //set  MSB
    mem[0x800B] = CPU::INS_STX_ZP; //3 cycles
    mem[0x800C] = 0x24;    //store MSB in 0x23

    //now mem[0x0023] = 0x12 (LSB) and mem[0x0024] = 0x43 (MSB)

    //load indirect address into A register
    mem[0x800D] = CPU::INS_LDX_IM; // 2 cycles
    mem[0x800E] = 0x13; //zero-page offset
    mem[0x800F] = CPU::INS_LDA_IND_X; // 6 cycles
    mem[0x8010] = 0x10; // zero-page address for indirect address
                        // - address for A register will be read from 0x10+0x13

    //We specify number of CPU cycles we want to execute
    cpu.Execute(24, mem);

    printf("X: 0x%X\n", cpu.X); //0x12
    printf("A: 0x%X\n", cpu.A); //0x15
}