#include <stdio.h>
#include <stdlib.h>

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
    printf("ERROR: Could not open %s\n", fp);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
