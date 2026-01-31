#include "intel8086.h"

//define tables
const char* reg_8_str[8] = { "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH" };
const char* reg_16_str[8] = { "AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI" };
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
    //only leftmost 6 bits define opcode
    const OpcodeEntry* op = lookup_opcode(data[idx] >> 2);
    if (op)
    {
      printf("found instruction: %s\n", op->str);
      uint8_t d = data[idx] & (1 << 1);
      uint8_t w = data[idx] & (1 << 0);
      printf("d = %x, w = %x\n", d, w);
    }
    else
    {
      printf("unknown instruction. exiting\n");
      exit(0);
    }
  }
}
