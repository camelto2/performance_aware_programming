#ifndef SIM8086_H
#define SIM8086_H

#include <stdint.h>
#include "intel8086.h"

typedef enum {
  REG_AX, 
  REG_CX, 
  REG_DX,
  REG_BX,
  REG_SP,
  REG_BP,
  REG_SI,
  REG_DI,
  NUM_REG
} Register16Type;

typedef enum {
  REG_AL, 
  REG_CL, 
  REG_DL,
  REG_BL,
  REG_AH, 
  REG_CH, 
  REG_DH,
  REG_BH,
} Register8Type;

typedef union {
  uint16_t x; //full 16 bit
  struct {
    uint8_t  l; //low 8 bits
    uint8_t  h; //high 8 bits
  };
} SplitRegister;

typedef struct {
  uint16_t x; //full 16 bit, for use consistency with SplitRegister
} SimpleRegister;


typedef enum {
  FLAG_CF,
  FLAG_PF, 
  FLAG_AF,
  FLAG_ZF, 
  FLAG_SF, 
  FLAG_TF, 
  FLAG_IF, 
  FLAG_DF, 
  FLAG_OF,
  NUM_FLAGS
} FlagType; 

typedef struct {
  SplitRegister ax;
  SplitRegister cx;
  SplitRegister dx;
  SplitRegister bx;
  SimpleRegister sp;
  SimpleRegister bp;
  SimpleRegister si;
  SimpleRegister di;

  uint16_t flags;
  uint16_t ip;
} CPUState;

//printing related functions
const char* getReg16Name(Register16Type reg);
const char* getReg8Name(Register8Type reg);

//getter/setter
uint16_t getReg16(CPUState* cpu, Register16Type reg);
void setReg16(CPUState* cpu, Register16Type reg, const uint16_t val);
uint8_t getReg8(CPUState* cpu, Register8Type reg);
void setReg8(CPUState* cpu, Register8Type reg, const uint8_t val);

void setFlags(CPUState* cpu, const uint16_t val);
void executeInstruction(CPUState* cpu, FullInstructionData* instr);

void printCPUChange(CPUState* before, CPUState* after);

void printCPUState(CPUState* cpu);

#endif
