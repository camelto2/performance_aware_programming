#ifndef intel8086_h
#define intel8086_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//registers
//extern const char* reg_8[];
//extern const char* reg_16[];
//extern const char* eff_addr[];

//instructions 
typedef enum 
{
  MOV_R_TF_RM,
  MOV_T_RM,
  MOV_T_R,
  MOV_M_T_A,
  MOV_A_T_M,
  NUM_INSTR
} InstructionType;

typedef struct
{
    InstructionType type;
    const char* mnemonic;
    uint8_t opcode;           // Base opcode
    uint8_t opcode_mask;      // Which bits must match 
                              
    // Flags
    uint8_t has_modrm     : 1; // Needs ModR/M byte
    uint8_t has_d_bit     : 1;    // Direction bit is in opcode
    uint8_t has_w_bit     : 1;    // Width bit is in opcode
    uint8_t has_s_bit     : 1;    // Sign-extend bit (for immediate data)
    uint8_t imm_type      : 2;     // 0=none, 1=byte, 2=word, 3=depends on W bit
    
    //reg can be in opcode for some instructions, and can also be necessary to actually decode instruction
    uint8_t modrm_reg           : 3; // Actually store the modrm register
    uint8_t modrm_reg_is_opcode : 1; // Modrm reg used to define actual opcode
    uint8_t reg_in_opcode       : 1; // Register in first opcode byte
} Instruction;

typedef struct {
  Instruction instr;
  uint8_t w_bit;
  uint8_t d_bit;
  uint8_t s_bit;
  uint8_t reg;
  uint8_t mod;
  uint8_t rm;
} FullInstructionData;

extern const Instruction instruction_table[];

const Instruction* lookup_instruction(uint8_t opcode, uint8_t modrm);

void process8086(const uint8_t* data, const size_t count);

void print_instruction(const FullInstructionData* instr);

#endif
