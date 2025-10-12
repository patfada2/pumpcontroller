#ifndef fileutils_h
#define fileutils_h

#include <LittleFS.h>
#include <FS.h>
#include <Arduino.h> 
#include <String.h>

#define FORMAT_LITTLEFS_IF_FAILED true


//const char* FILE_WRITE="w";
#define FILE_READ "r"
#define FILE_WRITE "w"
#define FILE_APPEND "a"

void writeFile(fs::FS& fs, const char* path, const char* message);
String readDataFile(String path);
void appendFile(fs::FS& fs, const char* path, const char* message);
void listAllFilesInDir(String dir_path);
void readFile(fs::FS& fs, const char* path);
double percentFull();
String readLastNLFS(String path, int n);

#endif