#include <windows.h>
#include <stdio.h>
#include "memmem.h"

int encode(char *inputName, char *outputName)
{
  char buffer[MAX_PATH];
  intptr_t result = (intptr_t)FindExecutable("AdpcmEncode.exe", NULL, buffer);
  if(result < 32) return printf("AdpcmEncode not found in system path\n");

  if(GetFileAttributes(inputName) == INVALID_FILE_ATTRIBUTES)
    return printf("Supplied audio file not found\n");

  sprintf(buffer, "-b 512 %s %s", inputName, outputName);
  result = (intptr_t)ShellExecute(NULL, NULL, "AdpcmEncode.exe",
    buffer, NULL, SW_HIDE);
  if(result < 32) return printf("Error trying to encode audio file\n");
  return 1;
}

int strip(char *outputName)
{
  // Wait for file availability
  int tryCount = 4;
  FILE *file = NULL;
  while(tryCount > 0)
  {
    Sleep(500);
    file = fopen(outputName, "rb");
    if(file == NULL) { tryCount--; } else { break; }
  }
  if(file == NULL) return printf("Could not load output file\n");

  // Get the total size of the audio file
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  // Read in all of the data to a buffer
  BYTE *fileData = HeapAlloc(GetProcessHeap(), 0, size);
  int bytesRead = fread(fileData, 1, size, file);
  if(bytesRead != size)
  {
    fclose(file);
    HeapFree(GetProcessHeap(), 0, fileData);
    return printf("Error reading output file\n");
  }

  // Locate the "fmt " chunk that stores the audio details
  BYTE *strippedData = HeapAlloc(GetProcessHeap(), 0, size);
  PVOID fmtChunk = memmem(fileData, size, "fmt ", sizeof(DWORD));
  if(fmtChunk == NULL)
  {
    fclose(file);
    HeapFree(GetProcessHeap(), 0, fileData);
    return printf("fmt chunk not found\n");
  }

  // Write out the fmt chunk to the new buffer
  DWORD fmtSize = 0;
  CopyMemory(&fmtSize, fmtChunk + sizeof(DWORD), sizeof(DWORD));
  DWORD totalFmtSize = sizeof(DWORD) * 2 + fmtSize;
  CopyMemory(strippedData, fmtChunk, totalFmtSize);

  // Locate the "data" chunk that contains the raw audio
  PVOID dataChunk = memmem(fileData, size, "data", sizeof(DWORD));
  if(dataChunk == NULL)
  {
    fclose(file);
    HeapFree(GetProcessHeap(), 0, fileData);
    HeapFree(GetProcessHeap(), 0, strippedData);
    return printf("data chunk not found\n");
  }

  // Write out the data chunk to the new buffer
  DWORD dataSize = 0;
  CopyMemory(&dataSize, dataChunk + sizeof(DWORD), sizeof(DWORD));
  DWORD totalDataSize = sizeof(DWORD) * 2 + dataSize;
  CopyMemory(strippedData + totalFmtSize, dataChunk, totalDataSize);

  // Close output file and re-open for writing
  fclose(file);
  file = fopen(outputName, "wb");
  int newFileSize = sizeof(DWORD) * 4 + fmtSize + dataSize;
  int bytesWritten = fwrite(strippedData, 1, newFileSize, file);
  if(bytesWritten != newFileSize)
  {
    fclose(file);
    HeapFree(GetProcessHeap(), 0, fileData);
    HeapFree(GetProcessHeap(), 0, strippedData);
    return printf("Could not write new stripped audio file\n");
  }

  // Clean up
  fclose(file);
  HeapFree(GetProcessHeap(), 0, fileData);
  HeapFree(GetProcessHeap(), 0, strippedData);
  return 1;
}

int main(int argc, char *argv[])
{
  if(argc < 3) return printf("Usage: main.exe input.wav output.wav\n");
  encode(argv[1], argv[2]);
  strip(argv[2]);
  return 0;
}
