#include "intel8086_text.h"
#include <inttypes.h>

const char* reg_8[8]    = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* reg_16[8]   = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char* eff_addr[8] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

//helper functions

static void print_ip_increment(const FullInstructionData* instr_data)
{
  int16_t offset = instr_data->immediate + 2;
  const char* sign = offset >= 0 ? "+" : "-";
  printf("$%s%" PRIi16, sign, offset >= 0 ? offset : -offset);
}

static void print_imm_to_reg(const FullInstructionData* instr_data)
{
  const char* dst = instr_data->w_bit ? reg_16[instr_data->reg] : reg_8[instr_data->reg];
  printf("%s, %" PRIi16, dst, instr_data->immediate);
}

static void print_acc_tofrom_imm(const FullInstructionData* instr_data)
{
  const char* reg = (instr_data->w_bit) ? reg_16[0] : reg_8[0];
  char imm[64];
  if (instr_data->instr.imm_is_mem)
    snprintf(imm, sizeof(imm), "[%"PRIi16"]", instr_data->immediate);
  else
    snprintf(imm, sizeof(imm), "%"PRIi16, instr_data->immediate);
  if (instr_data->d_bit)
    printf("%s, %s", imm, reg);
  else
    printf("%s, %s", reg, imm);
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
  if (instr_data->mod != 3)
    printf("%s %s, %" PRIi16, size, rm, instr_data->immediate);
  else
    printf("%s, %" PRIi16, rm, instr_data->immediate);
}

static void print_reg_tofrom_rm(const FullInstructionData* instr_data)
{
  char rm[64];
  format_rm(rm, sizeof(rm), instr_data);

  const char* reg = instr_data->w_bit ? reg_16[instr_data->reg] : reg_8[instr_data->reg];
  if (instr_data->d_bit)
    printf("%s, %s", reg, rm);
  else
    printf("%s, %s", rm, reg);
}

void printInstruction(const FullInstructionData* instr_data) {
#ifdef DEBUG
  printf("DEBUG op modrm displ imm: 0x%2x 0x%2x 0x%4x 0x%4x | size = %i\n", instr_data->raw_op_byte, instr_data->raw_modrm_byte, instr_data->displacement, instr_data->immediate, instr_data->size);
  printf("%i:  ", instr_data->instr.type);
#endif
  printf("%s ", instr_data->instr.mnemonic);

  if (instr_data->instr.ip_increment)
    print_ip_increment(instr_data);
  else if (instr_data->instr.reg_in_opcode)
    print_imm_to_reg(instr_data);
  else if (!instr_data->instr.has_modrm && instr_data->instr.imm_type > 0)
    print_acc_tofrom_imm(instr_data);
  else if (instr_data->instr.has_modrm)
  {
    if (instr_data->immediate)
      print_modrm_with_imm(instr_data);
    else
      print_reg_tofrom_rm(instr_data);
  }

}
