#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "intel8086.h"
#include "intel8086_text.h"

#include "intel8086.c"
#include "intel8086_text.c"

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

int main(int argc, char* argv[])
{

  if (argc != 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char* filename = argv[1];

  size_t count;
  uint8_t* data = read_binary_file(filename, &count);

  if (data != NULL)
  {
    process8086(data, count);
    free(data);
  }
  else
  {
    printf("Error reading file\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
