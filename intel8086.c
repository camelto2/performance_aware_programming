#include "intel8086.h"

//define tables
const OpcodeEntry opcode_table[] = {
  {MOV, "mov", 0b00100010}
};

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
    const OpcodeEntry* op = lookup_opcode(instr.data.opcode);
    if (!op)
    {
      printf("unknown instruction. exiting\n");
      exit(0);
    }
    idx += 2;
  }
}
