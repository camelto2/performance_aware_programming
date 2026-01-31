#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[])
{

  if (argc != 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char* filename = argv[1];
  FILE* fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    printf("ERROR: Could not open file: %s. Exiting!\n", filename);
    return EXIT_FAILURE;
  }

  //2 8 bit vals
  uint8_t val[2];
  size_t bytes_read;

  while ( (bytes_read = fread(&val, sizeof(val[0]), sizeof(val)/sizeof(val[0]), fp)) == sizeof(val)) 
  {
    printf("Read %x %x\n", val[0], val[1]);
  }

  if (ferror(fp))
  {
    perror("Error reading file");
  }
  else if (bytes_read > 0 && bytes_read < sizeof(val[0]))
  {
    printf("Warning: end of file with incomplete 2byte chunk (%zu bytes read)\n", bytes_read);
  }

  fclose(fp);

  return EXIT_SUCCESS;
}
