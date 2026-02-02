#include "intel8086_text.h"
#include <inttypes.h>

const char* reg_8[8]    = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* reg_16[8]   = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char* eff_addr[8] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

//helper functions
static void print_imm_to_reg(const FullInstructionData* instr_data)
{
  const char* dst = instr_data->w_bit ? reg_16[instr_data->reg] : reg_8[instr_data->reg];
  printf("%s, %" PRIi16 "\n", dst, instr_data->immediate);
}

static void print_acc_tofrom_imm(const FullInstructionData* instr_data)
{
  const char* reg = (instr_data->w_bit) ? reg_16[0] : reg_8[0];
  if (instr_data->d_bit)
    printf("[%" PRIi16"], %s\n", instr_data->immediate, reg);
  else
    printf("%s, [%" PRIi16"]\n", reg, instr_data->immediate);
}

static void format_rm(char* rm, size_t size, const FullInstructionData* instr_data)
{
  //this case, it is a register
  if (instr_data->mod == 3)
    snprintf(rm, size, "%s", instr_data->w_bit ? reg_16[instr_data->rm] : reg_8[instr_data->rm]);
  else 
  {
    if (instr_data->mod == 0 && instr_data->rm == 6)
      snprintf(rm, size, "[%" PRIi16 "]", instr_data->displacement);
    else if (instr_data->displacement == 0)
      snprintf(rm, size, "[%s]", eff_addr[instr_data->rm]);
    else {
      int16_t displ = instr_data->displacement;
      const char* add = eff_addr[instr_data->rm];
      if (displ > 0)
        snprintf(rm, size, "[%s + %" PRIi16 "]", add, displ);
      else
        snprintf(rm, size, "[%s - %" PRIi16 "]", add, -displ);
    }
  }
}

static void print_modrm_with_imm(const FullInstructionData* instr_data)
{
  char rm[64];
  format_rm(rm, sizeof(rm), instr_data);
  
  const char* size = instr_data->w_bit ? "word" : "byte";
  printf("%s, %s %" PRIi16 "\n", rm, size, instr_data->immediate);
}

static void print_reg_tofrom_rm(const FullInstructionData* instr_data)
{
  char rm[64];
  format_rm(rm, sizeof(rm), instr_data);

  const char* reg = instr_data->w_bit ? reg_16[instr_data->reg] : reg_8[instr_data->reg];
  if (instr_data->d_bit)
    printf("%s, %s\n", reg, rm);
  else
    printf("%s, %s\n", rm, reg);
}

void print_instruction(const FullInstructionData* instr_data) {
  printf("%i:  %s ", instr_data->instr.type, instr_data->instr.mnemonic);

  if (instr_data->instr.reg_in_opcode)
    print_imm_to_reg(instr_data);
  else if (!instr_data->instr.has_modrm && instr_data->instr.imm_type == 2)
    print_acc_tofrom_imm(instr_data);
  else if (instr_data->instr.has_modrm)
  {
    if (instr_data->instr.modrm_reg_is_opcode)
      print_modrm_with_imm(instr_data);
    else
      print_reg_tofrom_rm(instr_data);
  }

}
