#ifndef intel8086_h
#define intel8086_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Structure to represent the first byte of an 8086 instruction
typedef struct {
    uint8_t w     : 1; // Width bit (least significant bit)
    uint8_t d     : 1; // Direction bit
    uint8_t opcode: 6; // Opcode (most significant 6 bits)
} InstructionByte;

// Structure to represent the second byte (ModR/M)
typedef struct {
    uint8_t rm  : 3; // R/M field (least significant 3 bits)
    uint8_t reg : 3; // REG field
    uint8_t mod : 2; // MOD field (most significant 2 bits)
} ModRMByte;

// Union to easily cast raw bytes to the structured format
typedef union {
    uint8_t raw_byte;
    InstructionByte fields;
} InstructionUnion;

typedef union {
    uint8_t raw_byte;
    ModRMByte fields;
} ModRMUnion;

//registers
extern const char* reg_8[];
extern const char* reg_16[];

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
extern const OpcodeEntry opcode_table[];
const OpcodeEntry* lookup_opcode(uint8_t opcode);

void process8086(const uint8_t* data, const size_t count);

#endif
