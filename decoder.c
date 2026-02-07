#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "intel8086.h"
#include "intel8086_text.h"
#include "sim8086.h"

#include "intel8086.c"
#include "intel8086_text.c"
#include "sim8086.c"

uint8_t* read_binary_file(const char* filename, size_t* count_out)
{
  FILE* fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    printf("ERROR: Could not open file: %s", filename);
    return NULL;
  }

  //Seek to the end of the file to determine the size
  if (fseek(fp, 0L, SEEK_END) != 0) 
  {
    printf("Error seeking to end of file");
    fclose(fp);
    return NULL;
  }

  //Get file size
  size_t file_size = ftell(fp);
  if (file_size == -1L)
  {
    printf("Error getting file size");
    fclose(fp);
    return NULL;
  }

  //rewind  to the beginning of the file
  if (fseek(fp, 0L, SEEK_SET) != 0) 
  {
    printf("Error seeking to beginning of file");
    fclose(fp);
    return NULL;
  }

  uint8_t* data = malloc(file_size);
  if (data == NULL)
  {
    printf("Could not allocate data buffer");
    fclose(fp);
    return NULL;
  }

  size_t count = file_size / sizeof(uint8_t);
  size_t bytes_read = fread(data, sizeof(uint8_t), count, fp);
  if (bytes_read != file_size)
  {
    printf("Error reading file: expected %zu bytes, read %zu bytes\n", file_size, bytes_read);
    fclose(fp);
    free(data);
    return NULL;
  }

  fclose(fp);
  *count_out = count;
  return data;
}

void decode(const uint8_t* data, const size_t count)
{
  printf("bits 16\n");
  size_t idx = 0;
  while (idx < count) 
  {
    FullInstructionData instr = decode8086Instruction(data, idx, count);
    printInstruction(&instr);
    printf("\n");
    idx += instr.size;
  }
}

void decodeAndSim(const uint8_t* data, const size_t count)
{
  CPUState cpu = {0};
  uint8_t mem[1024*1024] = {0};
  printf("bits 16\n");
  while (cpu.ip < count) 
  {
    CPUState before = cpu;
    FullInstructionData instr = decode8086Instruction(data, cpu.ip, count);
    printInstruction(&instr);
    executeInstruction(&cpu, &instr, mem);
    printCPUChange(&before, &cpu);
    printf("\n");
  }
  printf("\n\n");
  printf("Final CPU State:\n");
  printCPUState(&cpu);
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s [args] <filename>\n", argv[0]);
    printf("arg list\n");
    printf("  --sim : simulate and print what 8086 would do\n");
      
    return EXIT_FAILURE;
  }

  char* arg;
  char* filename;
  uint8_t sim = 0;
  for (int i = 1; i < argc; i++)
  {
    arg = argv[i];
    if (strcmp(arg, "--sim") == 0)
      sim = 1;
    else 
      filename = arg;
  }

  size_t count;
  uint8_t* data = read_binary_file(filename, &count);

  if (data != NULL)
  {
    sim ? decodeAndSim(data, count) : decode(data, count);
    free(data);
  }
  else
  {
    printf("Error reading file\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
