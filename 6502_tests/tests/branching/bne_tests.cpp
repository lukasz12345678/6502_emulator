//
// Created by Lukasz on 15.08.2022.
//
#include "6502_cpu.h"
#include <gtest/gtest.h>

using namespace MOS6502;

class M6502BNETest : public testing::Test {
public:
    Bus mem{};
    CPU cpu;

    virtual void SetUp(){
        CPU::Setup(mem, 0x8000);
        int c = 7;
        cpu.Reset( c , mem);
    }

    virtual void TearDown(){

    }
};

TEST_F(M6502BNETest, BNEUnsuccessfulBranch){
    //given:

    cpu.P.Z = 1;
    mem[0x8000] = INSTRUCTIONS::INS_BNE; //2 cycles (branch unsuccessful)
    mem[0x8001] = 0x1;

    constexpr int32_t NUM_OF_CYCLES = 2;
    CPU CPUCopy = cpu;

    //when:
    int32_t totalCycles = cpu.Execute(NUM_OF_CYCLES, mem);

    //then:
    EXPECT_EQ(totalCycles, NUM_OF_CYCLES);
    EXPECT_EQ(cpu.PC, 0x8002);

    EXPECT_EQ(cpu.P.PS, CPUCopy.P.PS);
}

TEST_F(M6502BNETest, BNECanBranchForward){
    //given:

    cpu.P.Z = 0;
    mem[0x8000] = INSTRUCTIONS::INS_BNE; //3 cycles (branch successful, same page)
    mem[0x8001] = 0x1;

    constexpr int32_t NUM_OF_CYCLES = 3;
    CPU CPUCopy = cpu;

    //when:
    int32_t totalCycles = cpu.Execute(NUM_OF_CYCLES, mem);

    //then:
    EXPECT_EQ(totalCycles, NUM_OF_CYCLES);
    EXPECT_EQ(cpu.PC, 0x8003);

    EXPECT_EQ(cpu.P.PS, CPUCopy.P.PS);
}

TEST_F(M6502BNETest, BNECanBranchForwardToNextPage){
    //given:

    cpu.P.Z = 0;
    mem[0x8000] = INSTRUCTIONS::INS_JMP_ABS; //3 cycles
    mem[0x8001] = 0xF0;
    mem[0x8002] = 0x80;
    mem[0x80F0] = INSTRUCTIONS::INS_BNE; //4 cycles (branch successful, new page)
    mem[0x80F1] = 0x55;

    constexpr int32_t NUM_OF_CYCLES = 7;
    CPU CPUCopy = cpu;

    //when:
    int32_t totalCycles = cpu.Execute(NUM_OF_CYCLES, mem);

    //then:
    EXPECT_EQ(totalCycles, NUM_OF_CYCLES);
    EXPECT_EQ(cpu.PC, 0x8147);

    EXPECT_EQ(cpu.P.PS, CPUCopy.P.PS);
}

TEST_F(M6502BNETest, BNECanBranchBackward){
    //given:

    cpu.P.Z = 0;
    mem[0x8000] = INSTRUCTIONS::INS_JMP_ABS; //3 cycles
    mem[0x8001] = 0xF0;
    mem[0x8002] = 0x80;
    mem[0x80F0] = INSTRUCTIONS::INS_BNE; //3 cycles (branch successful, same page)
    mem[0x80F1] = static_cast<uint8_t>(-0x3);

    constexpr int32_t NUM_OF_CYCLES = 6;
    CPU CPUCopy = cpu;

    //when:
    int32_t totalCycles = cpu.Execute(NUM_OF_CYCLES, mem);

    //then:
    EXPECT_EQ(totalCycles, NUM_OF_CYCLES);
    EXPECT_EQ(cpu.PC, 0x80EF);

    EXPECT_EQ(cpu.P.PS, CPUCopy.P.PS);
}

TEST_F(M6502BNETest, BNECanBranchBackwardToNextPage){
    //given:

    cpu.P.Z = 0;
    mem[0x8000] = INSTRUCTIONS::INS_BNE; //4 cycles (branch successful, new page)
    mem[0x8001] = static_cast<uint8_t>(-0x3);

    constexpr int32_t NUM_OF_CYCLES = 4;
    CPU CPUCopy = cpu;

    //when:
    int32_t totalCycles = cpu.Execute(NUM_OF_CYCLES, mem);

    //then:
    EXPECT_EQ(totalCycles, NUM_OF_CYCLES);
    EXPECT_EQ(cpu.PC, 0x7FFF);

    EXPECT_EQ(cpu.P.PS, CPUCopy.P.PS);
}