#ifndef INTEL_8086_TEXT_H
#define INTEL_8086_TEXT_H

#include "intel8086.h"

//registers
extern const char* reg_8[];
extern const char* reg_16[];
extern const char* eff_addr[];

void printInstruction(const FullInstructionData* instr);


#endif
