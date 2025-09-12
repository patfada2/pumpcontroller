
#include "./fileutils.h"
#include "common.h"

void writeFile(fs::FS& fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}


//return data file as json array
String readDataFile(String path) {
  String s = "";
  int numLines = 0;
  String line = "";
  s = "[";
  Serial.printf("Reading file: %s\r\n", path);

  File file = LittleFS.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return "";
  }

  Serial.println("- read from file:");
  while (file.available()) {
    line = (file.readString());
    Serial.println(line);
    s += line;
    numLines += 1;
  }
  file.close();
  //replace trailig comma with ']'
  s[s.length() - 3] = ' ';
  s[s.length() - 2] = ' ';
  s[s.length() - 1] = ']';

  Serial.println("read " + String(numLines) + " lines");
  Serial.println(s);
  if (numLines > 0) {
    return s;
  } else return "[]";
}


void appendFile(fs::FS& fs, const char* path, const char* message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  //println
  if (file.println(message)) {
   
    Serial.printf("- message appended: %s\r\n", message);
  } else {
    Serial.println("- append failed");
  }
  file.close();
}

void listAllFilesInDir(String dir_path) {
  // Open the specified directory
  Dir dir = LittleFS.openDir(dir_path);
  logInfo("listing dir " +  dir_path);
  // Iterate through the directory contents
  while (dir.next()) {
    if (dir.isFile()) {
      // If it's a file, print its name
      logInfo("File: ");
      logInfo(dir_path + dir.fileName() + " size: "+ String(dir.fileSize()));
    } else if (dir.isDirectory()) {
      // If it's a directory, print its name and recursively list its contents
      logInfo("Dir: ");
      logInfo(dir_path + dir.fileName() + "/");
      listAllFilesInDir(dir_path + dir.fileName() + "/"); // Recursive call for subdirectories
    }
  }
}

void readFile(fs::FS& fs, const char* path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}