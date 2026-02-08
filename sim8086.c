#include "sim8086.h"
#include "intel8086_text.h"
#include <stdlib.h>
#include <stdio.h>

const char* getReg16Name(Register16Type reg) {
  if (reg < NUM_REG)
    return reg_16[reg];
  return "Unknown Register";
}

const char* getReg8Name(Register8Type reg) {
  if (reg < NUM_REG)
    return reg_8[reg];
  return "Unknown Register";
}

uint16_t getReg16(CPUState* cpu, Register16Type reg) {
  switch(reg) {
    case REG_AX: return cpu->ax.x;
    case REG_BX: return cpu->bx.x;
    case REG_CX: return cpu->cx.x;
    case REG_DX: return cpu->dx.x;
    case REG_SP: return cpu->sp.x;
    case REG_BP: return cpu->bp.x;
    case REG_SI: return cpu->si.x;
    case REG_DI: return cpu->di.x;
    default: return 0;
  };
}

void setReg16(CPUState* cpu, Register16Type reg, const uint16_t val) {
  switch(reg) {
    case REG_AX: cpu->ax.x = val; break; 
    case REG_BX: cpu->bx.x = val; break;
    case REG_CX: cpu->cx.x = val; break;
    case REG_DX: cpu->dx.x = val; break;
    case REG_SP: cpu->sp.x = val; break; 
    case REG_BP: cpu->bp.x = val; break;
    case REG_SI: cpu->si.x = val; break;
    case REG_DI: cpu->di.x = val; break;
    default: break;
  };
}

uint8_t getReg8(CPUState* cpu, Register8Type reg) {
  switch(reg) {
    case REG_AL: return cpu->ax.l;
    case REG_BL: return cpu->bx.l;
    case REG_CL: return cpu->cx.l;
    case REG_DL: return cpu->dx.l;
    case REG_AH: return cpu->ax.h;
    case REG_BH: return cpu->bx.h;
    case REG_CH: return cpu->cx.h;
    case REG_DH: return cpu->dx.h;
    default: return 0;
  };
}

void setReg8(CPUState* cpu, Register8Type reg, const uint8_t val) {
  switch(reg) {
    case REG_AL: cpu->ax.l = val; break; 
    case REG_BL: cpu->bx.l = val; break;
    case REG_CL: cpu->cx.l = val; break;
    case REG_DL: cpu->dx.l = val; break;
    case REG_AH: cpu->ax.h = val; break; 
    case REG_BH: cpu->bx.h = val; break;
    case REG_CH: cpu->cx.h = val; break;
    case REG_DH: cpu->dx.h = val; break;
    default: break;
  };
}

void setFlags(CPUState* cpu, const uint16_t val) {
  //ZF
  if (val == 0)
    cpu->flags |= (1 << FLAG_ZF);
  else
    cpu->flags &= ~(1 << FLAG_ZF);

  uint16_t mask = val & (1 << 15);
  if (mask != 0)
    cpu->flags |= (1 << FLAG_SF);
  else
    cpu->flags &= ~(1 << FLAG_SF);
}

void executeInstruction(CPUState* cpu, FullInstructionData* data, uint8_t* mem) {
  cpu->ip += data->size;
#ifdef DEBUG
  printf(";  DEBUG instr: %i   ", data->instr.type);
#endif
  switch (data->instr.type) {
    case MOV_T_R: {
      if (data->w_bit)
        setReg16(cpu, (Register16Type)data->reg, (uint16_t)data->immediate);
      else
        setReg8(cpu, (Register8Type)data->reg, (uint8_t)data->immediate);
      break;
    }
    case MOV_R_TF_RM: {
      if (data->mod == 3) {
        if (data->w_bit) {
          uint16_t val = data->d_bit ? getReg16(cpu, (Register16Type)data->rm) : getReg16(cpu, (Register16Type)data->reg);
          data->d_bit ? setReg16(cpu, (Register16Type)data->reg, val) : setReg16(cpu, (Register16Type)data->rm, val); 
        }
        else {
          uint8_t val = data->d_bit ? getReg8(cpu, (Register8Type)data->rm) : getReg16(cpu, (Register8Type)data->reg);
          data->d_bit ? setReg8(cpu, (Register8Type)data->reg, val) : setReg8(cpu, (Register8Type)data->rm, val); 
        }
      }
      else {
        uint16_t idx = 0;
        switch (data->rm) {
          case 0: {
            idx  = getReg16(cpu, REG_BX);
            idx += getReg16(cpu, REG_SI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            break;
          }
          case 1: {
            idx  = getReg16(cpu, REG_BX);
            idx += getReg16(cpu, REG_DI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            break;
          }
          case 2: {
            idx  = getReg16(cpu, REG_BP);
            idx += getReg16(cpu, REG_SI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            break;
          }
          case 3: {
            idx  = getReg16(cpu, REG_BP);
            idx += getReg16(cpu, REG_DI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            break;
          }
          case 4: {
            idx  = getReg16(cpu, REG_SI);
            idx += data->displacement;
            break;
          }
          case 5: {
            idx  = getReg16(cpu, REG_DI);
            idx += data->displacement;
            break;
          }
          case 6: {
            if (data->mod == 0)
              idx = data->displacement;
            else
              idx  = getReg16(cpu, REG_BP) + data->displacement;
            break;
          }
          case 7: {
            idx  = getReg16(cpu, REG_BX);
            idx += data->displacement;
            break;
          }
          default:
            printf("Unknown R/M\n");
            exit(0);
            break;
        }
        if (data->d_bit) 
          (data->w_bit) ? setReg16(cpu, (Register16Type)data->reg, mem[idx]) : setReg16(cpu, (Register16Type)data->reg, mem[idx]);
        else {
          uint16_t val = (data->w_bit) ?  getReg8(cpu, (Register8Type)data->reg) : getReg8(cpu, (Register8Type)data->reg);
          mem[idx] = val;
        }
      }
      break;
    }
    case MOV_T_RM: {
      if (data->mod == 3) {
        if (data->w_bit) {
          uint16_t val = data->immediate;
          setReg16(cpu, (Register16Type)data->rm, val);
        }
        else {
          uint8_t val = data->immediate;
          setReg8(cpu, (Register8Type)data->rm, val);
        }
      }
      else {
        uint16_t idx = 0;
        switch (data->rm) {
          case 0: {
            idx  = getReg16(cpu, REG_BX);
            idx += getReg16(cpu, REG_SI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            mem[idx] = data->immediate;
            break;
          }
          case 1: {
            idx  = getReg16(cpu, REG_BX);
            idx += getReg16(cpu, REG_DI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            mem[idx] = data->immediate;
            break;
          }
          case 2: {
            idx  = getReg16(cpu, REG_BP);
            idx += getReg16(cpu, REG_SI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            mem[idx] = data->immediate;
            break;
          }
          case 3: {
            idx  = getReg16(cpu, REG_BP);
            idx += getReg16(cpu, REG_DI);
            idx += data->displacement;  //displacement is 0 in data if not needed
            mem[idx] = data->immediate;
            break;
          }
          case 4: {
            idx  = getReg16(cpu, REG_SI);
            idx += data->displacement;
            mem[idx] = data->immediate;
            break;
          }
          case 5: {
            idx  = getReg16(cpu, REG_DI);
            idx += data->displacement;
            mem[idx] = data->immediate;
            break;
          }
          case 6: {
            if (data->mod == 0)
              idx = data->displacement;
            else
              idx  = getReg16(cpu, REG_BP) + data->displacement;
            mem[idx] = data->immediate;
            break;
          }
          case 7: {
            idx  = getReg16(cpu, REG_BX);
            idx += data->displacement;
            mem[idx] = data->immediate;
            break;
          }
        }
      }
      break;
    }
    case ADD: {
      if (data->w_bit) {
        uint16_t dst = data->d_bit ? getReg16(cpu, (Register16Type)data->reg) : getReg16(cpu, (Register16Type)data->rm);
        uint16_t src = data->d_bit ? getReg16(cpu, (Register16Type)data->rm) : getReg16(cpu, (Register16Type)data->reg);
        uint16_t val = dst + src;
        data->d_bit ? setReg16(cpu, (Register16Type)data->reg, val) : setReg16(cpu, (Register16Type)data->rm, val); 
        setFlags(cpu, val);
      }
      else {
        uint8_t dst = data->d_bit ? getReg8(cpu, (Register8Type)data->reg) : getReg8(cpu, (Register8Type)data->rm);
        uint8_t src = data->d_bit ? getReg8(cpu, (Register8Type)data->rm) : getReg8(cpu, (Register8Type)data->reg);
        uint8_t val = dst + src;
        data->d_bit ? setReg8(cpu, (Register8Type)data->reg, val) : setReg8(cpu, (Register8Type)data->rm, val); 
        setFlags(cpu, val);
      }
      break;
    }
    case ADD_T_RM: {
      if (data->w_bit) {
        uint16_t dst = getReg16(cpu, (Register16Type)data->rm);
        uint16_t val = dst + data->immediate;
        setReg16(cpu, (Register16Type)data->rm, val); 
        setFlags(cpu, val);
      }
      else {
        uint8_t dst = getReg16(cpu, (Register8Type)data->rm);
        uint8_t val = dst + data->immediate;
        setReg8(cpu, (Register8Type)data->rm, val); 
        setFlags(cpu, val);
      }
      break;
    }
    case SUB: {
      if (data->w_bit) {
        uint16_t dst = data->d_bit ? getReg16(cpu, (Register16Type)data->reg) : getReg16(cpu, (Register16Type)data->rm);
        uint16_t src = data->d_bit ? getReg16(cpu, (Register16Type)data->rm) : getReg16(cpu, (Register16Type)data->reg);
        uint16_t val = dst - src;
        data->d_bit ? setReg16(cpu, (Register16Type)data->reg, val) : setReg16(cpu, (Register16Type)data->rm, val); 
        setFlags(cpu, val);
      }
      else {
        uint8_t dst = data->d_bit ? getReg8(cpu, (Register8Type)data->reg) : getReg8(cpu, (Register8Type)data->rm);
        uint8_t src = data->d_bit ? getReg8(cpu, (Register8Type)data->rm) : getReg8(cpu, (Register8Type)data->reg);
        uint8_t val = dst - src;
        data->d_bit ? setReg8(cpu, (Register8Type)data->reg, val) : setReg8(cpu, (Register8Type)data->rm, val); 
        setFlags(cpu, val);
      }
      break;
    }
    case SUB_F_RM: {
      if (data->w_bit) {
        uint16_t dst = getReg16(cpu, (Register16Type)data->rm);
        uint16_t val = dst - data->immediate;
        setReg16(cpu, (Register16Type)data->rm, val); 
        setFlags(cpu, val);
      }
      else {
        uint8_t dst = getReg8(cpu, (Register8Type)data->rm);
        uint8_t val = dst - data->immediate;
        setReg8(cpu, (Register8Type)data->rm, val); 
        setFlags(cpu, val);
      }
      break;
    }
    case CMP_R_RM: {
      if (data->w_bit) {
        uint16_t dst = data->d_bit ? getReg16(cpu, (Register16Type)data->reg) : getReg16(cpu, (Register16Type)data->rm);
        uint16_t src = data->d_bit ? getReg16(cpu, (Register16Type)data->rm) : getReg16(cpu, (Register16Type)data->reg);
        uint16_t val = dst - src;
        setFlags(cpu, val);
      }
      else {
        uint8_t dst = data->d_bit ? getReg8(cpu, (Register8Type)data->reg) : getReg8(cpu, (Register8Type)data->rm);
        uint8_t src = data->d_bit ? getReg8(cpu, (Register8Type)data->rm) : getReg8(cpu, (Register8Type)data->reg);
        uint8_t val = dst - src;
        setFlags(cpu, val);
      }
      break;
    }
    case CMP_W_RM: {
      if (data->mod == 3) {
        if (data->w_bit) {
          uint16_t dst = getReg16(cpu, (Register16Type)data->rm);
          uint16_t val = dst - data->immediate;
          setFlags(cpu, val);
        }
        else {
          uint8_t dst = getReg8(cpu, (Register8Type)data->rm);
          uint8_t val = dst - data->immediate;
          setFlags(cpu, val);
        }
      }
      else {
        printf("Need to implement\n");
        exit(0);
      }
      break;
    }
    case JNE: {
      if (!((cpu->flags >> FLAG_ZF) & 1))
        cpu->ip += data->immediate;
      break;
    }
    default:
      printf("Unknown instruction type\n");
      exit(0);
      break;
  }
}

void printCPUChange(CPUState* before, CPUState* after) {
  printf(" ;\t");
  int count = 0;

  if (before->ip != after->ip) {
    if (count) printf("  ");
    printf("ip: 0x%x -> 0x%x", before->ip, after->ip);
    count++;
  }
  //split registers, show only the part that count
  if (before->ax.x != after->ax.x) {
    if (count) printf("  ");
    if (before->ax.h == after->ax.h)
      printf("al: 0x%x -> 0x%x", before->ax.l, after->ax.l);
    else if (before->ax.l == after->ax.l)
      printf("ah: 0x%x -> 0x%x", before->ax.h, after->ax.h);
    else 
      printf("ax: 0x%x -> 0x%x", before->ax.x, after->ax.x);
    count++;
  }

  if (before->bx.x != after->bx.x) {
    if (count) printf("  ");
    if (before->bx.h == after->bx.h)
      printf("bl: 0x%x -> 0x%x", before->bx.l, after->bx.l);
    else if (before->bx.l == after->bx.l)
      printf("bh: 0x%x -> 0x%x", before->bx.h, after->bx.h);
    else 
      printf("bx: 0x%x -> 0x%x", before->bx.x, after->bx.x);
    count++;
  }

  if (before->cx.x != after->cx.x) {
    if (count) printf("  ");
    if (before->cx.h == after->cx.h)
      printf("cl: 0x%x -> 0x%x", before->cx.l, after->cx.l);
    else if (before->cx.l == after->cx.l)
      printf("ch: 0x%x -> 0x%x", before->cx.h, after->cx.h);
    else 
      printf("cx: 0x%x -> 0x%x", before->cx.x, after->cx.x);
    count++;
  }

  if (before->dx.x != after->dx.x) {
    if (count) printf("  ");
    if (before->dx.h == after->dx.h)
      printf("dl: 0x%x -> 0x%x", before->dx.l, after->dx.l);
    else if (before->dx.l == after->dx.l)
      printf("dh: 0x%x -> 0x%x", before->dx.h, after->dx.h);
    else 
      printf("dx: 0x%x -> 0x%x", before->dx.x, after->dx.x);
    count++;
  }

  if (before->sp.x != after->sp.x) {
    if (count) printf("  ");
    printf("sp: 0x%x -> 0x%x", before->sp.x, after->sp.x);
    count++;
  }

  if (before->bp.x != after->bp.x) {
    if (count) printf("  ");
    printf("bp: 0x%x -> 0x%x", before->bp.x, after->bp.x);
    count++;
  }

  if (before->si.x != after->si.x) {
    if (count) printf("  ");
    printf("si: 0x%x -> 0x%x", before->si.x, after->si.x);
    count++;
  }

  if (before->di.x != after->di.x) {
    if (count) printf("  ");
    printf("di: 0x%x -> 0x%x", before->di.x, after->di.x);
    count++;
  }

  uint16_t diff = before->flags^after->flags;
  uint16_t mask = 1 << FLAG_ZF;
  if ((diff & mask) != 0) {
    if (count) printf("  ");
    printf("ZF: %i -> %i", (before->flags & mask) ? 1 : 0, (after->flags & mask) ? 1 : 0);
    count++;
  }

  mask = 1 << FLAG_SF;
  if ((diff & mask) != 0) {
    if (count) printf("  ");
    printf("SF: %i -> %i", (before->flags & mask) ? 1 : 0, (after->flags & mask) ? 1 : 0);
    count++;
  }
}

void printCPUState(CPUState* cpu)
{
  printf("ax: 0x%x (%i)\n", cpu->ax.x, cpu->ax.x);
  printf("bx: 0x%x (%i)\n", cpu->bx.x, cpu->bx.x);
  printf("cx: 0x%x (%i)\n", cpu->cx.x, cpu->cx.x);
  printf("dx: 0x%x (%i)\n", cpu->dx.x, cpu->dx.x);
  printf("sp: 0x%x (%i)\n", cpu->sp.x, cpu->sp.x);
  printf("bp: 0x%x (%i)\n", cpu->bp.x, cpu->bp.x);
  printf("si: 0x%x (%i)\n", cpu->si.x, cpu->si.x);
  printf("di: 0x%x (%i)\n", cpu->di.x, cpu->di.x);

  printf("ip: 0x%x (%i)\n", cpu->ip, cpu->ip);

  printf("ZF: %i\n", (cpu->flags & 1<<FLAG_ZF) ? 1 : 0);
  printf("SF: %i\n", (cpu->flags & 1<<FLAG_SF) ? 1 : 0);

}
