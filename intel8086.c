#include "intel8086.h"
#include "intel8086_text.h"

//define instructions
const Instruction instruction_table[] = {
  { MOV_R_TF_RM, "mov", 0b10001000, 0b11111100, .has_modrm=1, .has_d_bit=1, .has_w_bit=1 },
  { MOV_T_RM,    "mov", 0b11000110, 0b11111110, .has_modrm=1, .imm_type=3,  .has_w_bit=1 },
  { MOV_T_R,     "mov", 0b10110000, 0b11110000, .has_w_bit=1, .imm_type=3,  .reg_in_opcode=1},
  { MOV_M_T_A,   "mov", 0b10100000, 0b11111110, .has_w_bit=1, .imm_type=2,  .has_d_bit=1}, 
  { MOV_A_T_M,   "mov", 0b10100010, 0b11111110, .has_w_bit=1, .imm_type=2,  .has_d_bit=1}, 
  { ADD,         "add", 0b00000000, 0b11111100, .has_w_bit=1, .has_d_bit=1, .has_modrm=1},
  { ADD_T_RM,    "add", 0b10000000, 0b11111100, .has_s_bit=1, .has_w_bit=1, .has_modrm=1, .modrm_reg_is_opcode=1, .modrm_reg=0, .imm_type=3},
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
    if (!instr)
    {
      printf("unknown instruction\n");
      exit(0);
    }
    FullInstructionData instr_data;
    instr_data.instr = *instr;

    //extract actual data
    instr_data.reg   = (instr->reg_in_opcode) ? (opcode & 0b00000111)      : 0;
    instr_data.d_bit = (instr->has_d_bit)     ? (opcode >> 1 & 0b00000001) : 0;
    instr_data.w_bit = (instr->has_w_bit) ? ( instr->reg_in_opcode ? (opcode >> 3 & 0b00000001) : (opcode & 0b00000001 )) : 0;
    instr_data.s_bit = (instr->has_s_bit)     ? (opcode >> 1 & 0b00000001) : 0;

    //now consume next bytes, if needed
    instr_data.rm  = 0;
    instr_data.mod = 0;
    instr_data.displacement = 0;
    if (instr->has_modrm)
    {
      uint8_t modrm = data[idx++];
      instr_data.mod = modrm >> 6 & 0b00000011;
      instr_data.reg = modrm >> 3 & 0b00000111;
      instr_data.rm  = modrm & 0b00000111;

      if ((instr_data.mod == 0 && instr_data.rm == 6 ) || instr_data.mod == 2) {
        uint16_t value = (uint16_t)data[idx] | (uint16_t)data[idx + 1] << 8;
        instr_data.displacement = (int16_t)value;
        idx += 2;
      }
      else if (instr_data.mod == 1) {
        instr_data.displacement = (int8_t)data[idx++];
      }
    }

    instr_data.immediate = 0;
    if (instr->imm_type == 1)
      instr_data.immediate = data[idx++];
    else if (instr->imm_type == 2) {
      uint16_t value = (uint16_t)data[idx] | (uint16_t)data[idx + 1] << 8;
      instr_data.immediate = (int16_t)value;
      idx += 2;
    }
    else if (instr->imm_type == 3) {
      if (instr_data.instr.has_s_bit & instr_data.s_bit & instr_data.w_bit) // s1, w1
        instr_data.immediate = (int8_t)data[idx++];
      else if (instr_data.w_bit) { // w1, and s0 or no s
        uint16_t value = (uint16_t)data[idx] | (uint16_t)data[idx + 1] << 8;
        instr_data.immediate = (int16_t)value;
        idx += 2;
      }
      else
        instr_data.immediate = (int8_t)data[idx++];
    }
    print_instruction(&instr_data);
  }
}

