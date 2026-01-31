#ifndef intel8086_h
#define intel8086_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//registers
typedef enum 
{
  AL, CL, DL, BL, AH, CH, DH, BH
} Reg8;
extern const char* reg_8_str[];

typedef enum 
{
  AX, CX, DX, BX, SP, BP, SI, DI
} Reg16;
extern const char* reg_16_str[];

//instructions
typedef enum 
{
  MOV
} InstructionType;

typedef struct 
{ 
  InstructionType type;
  const char* str;
  uint8_t opcode;
} OpcodeEntry;
extern const OpcodeEntry optcode_table[];

const OpcodeEntry* lookup_opcode(uint8_t opcode);

void process8086(const uint8_t* data, const size_t count);

#endif
