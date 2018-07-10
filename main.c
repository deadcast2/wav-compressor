#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("Hello, you typed - %s\n", argc > 1 ? argv[1] : "");
  return 0;
}
