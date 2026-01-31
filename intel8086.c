#include "intel8086.h"
#include <assert.h>

//define tables
const OpcodeEntry opcode_table[] = {
  {MOV, "mov", 0b00100010}
};

const char* reg_8[8]  = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* reg_16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

const OpcodeEntry* lookup_opcode(uint8_t opcode)
{
  for (size_t i = 0; i < sizeof(opcode_table) / sizeof(OpcodeEntry); i++) 
  {
    if (opcode_table[i].opcode == opcode) {
        return &opcode_table[i];
    }
  }
  return NULL;
}

void process8086(const uint8_t* data, const size_t count)
{
  //start decoding asm from binary
  printf("bits 16\n\n");
  size_t idx = 0;
  while (idx < count)
  {
    InstructionUnion instr;
    ModRMUnion modrm;
    instr.raw_byte = data[idx];
    modrm.raw_byte = data[idx + 1];
    const OpcodeEntry* op = lookup_opcode(instr.fields.opcode);
    if (!op)
    {
      printf("unknown instruction. exiting\n");
      exit(0);
    }
    assert(modrm.fields.mod == 0b00000011); // only allow reg to reg for now
    const char* arg1 = (instr.fields.w) ? reg_16[modrm.fields.rm] : reg_8[modrm.fields.rm];
    const char* arg2 = (instr.fields.w) ? reg_16[modrm.fields.reg] : reg_8[modrm.fields.reg];
    printf("%s %s, %s\n", op->str, arg1, arg2);
    idx += 2;
  }
}
