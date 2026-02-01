#include "intel8086.h"

const char* reg_8[8]    = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* reg_16[8]   = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char* eff_addr[8] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

//define instructions
const Instruction instruction_table[] = {
{ MOV_R_TF_RM, "mov", 0b10001000, 0b11111100, .has_modrm=1, .has_d_bit=1, .has_w_bit=1 },
};

const Instruction* lookup_instruction(uint8_t opcode, uint8_t modrm) {

  const uint8_t num_instructions = sizeof(instruction_table) / sizeof(instruction_table[0]);
  for (uint8_t i = 0; i < num_instructions; i++)
  {
    const Instruction* instr = &instruction_table[i];
    //check if opcode is match, otherwise move on
    if ((opcode & instr->opcode_mask) != instr->opcode)
      continue;

    //now check modrm as part of opcode
    if (instr->modrm_reg_is_opcode) {
      uint8_t reg = (modrm >> 3) & 0b00000111; //bitshift and read right 3 bits
      if (reg != instr->modrm_reg)
        continue;
    }
    //found instruction
    return instr;
  }
  return NULL;
}

void process8086(const uint8_t* data, const size_t count) {
  //start decoding asm from binary
  printf("bits 16\n\n");
  size_t idx = 0;
  while (idx < count)
  {

    uint8_t opcode = data[idx++]; // consume opcode
    //lookup instruction, just peek into data[idx] don't consume
    const Instruction* instr = lookup_instruction(opcode, (idx < count) ? data[idx] : 0);
    FullInstructionData instr_data;
    instr_data.instr = *instr;

    if (!instr)
    {
      printf("unknown instruction\n");
      idx++;
      continue;
    }
    //extract actual data
    instr_data.reg   = (instr->reg_in_opcode) ? (opcode & 0b00000111)      : 0;
    instr_data.w_bit = (instr->has_w_bit)     ? (opcode & 0b00000001)      : 0;
    instr_data.d_bit = (instr->has_d_bit)     ? (opcode >> 1 & 0b00000001) : 0;

    //now consume next bytes, if needed
    instr_data.rm  = 0;
    instr_data.mod = 0;
    if (instr->has_modrm)
    {
      uint8_t modrm = data[idx++];
      instr_data.mod = modrm >> 6 & 0b00000011;
      instr_data.reg = modrm >> 3 & 0b00000111;
      instr_data.rm  = modrm & 0b00000111;
    }
    print_instruction(&instr_data);
  }
}

void print_instruction(const FullInstructionData* instr_data) {
  printf("%s ", instr_data->instr.mnemonic);

  const char* src = (instr_data->w_bit) ? reg_16[instr_data->reg] : reg_8[instr_data->reg];
  const char* dst = (instr_data->w_bit) ? reg_16[instr_data->rm] : reg_8[instr_data->rm];
  printf("%s, %s\n", dst, src);
}
