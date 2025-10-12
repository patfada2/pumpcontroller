
#include "fileutils.h"
#include "common.h"
#include <vector>
#include "logutils.h"
#include <string>

void writeFile(fs::FS& fs, const char* path, const char* message) {
  logTrace("Writing file: " + String(path));

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    logInfo("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    logTrace("- file written");
  } else {
    logInfo("- write failed");
  }
  file.close();
}


//return data file as json array
String readDataFile(String path) {
  String s = "";
  int numLines = 0;
  String line = "";
  s = "[";
  logInfo("Reading file: " +  path);

  File file = LittleFS.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    logInfo("- failed to open file for reading");
    return "";
  }

  Serial.println("- read from file:");
  while (file.available()) {
    line = (file.readStringUntil('\n'));
    logTrace(line);
    //dont want the last line which is just a comma
    if (file.available()) {
      s += line;
      numLines += 1;
    }
  }
  file.close();
  logInfo("last line = " + line);

  //replace trailing comma with  ']'
  s[s.length() - 2] = ' ';
  s[s.length() - 1] = ']';
  //logInfo("!!!!!!!!!!!!!!!!!!!!!!!!!!" +s);

  logInfo("read " + String(numLines) + " lines");
  //Serial.println(s);
  if (numLines > 0) {
    return s;
  } else return "[]";
}


// Function to read the last n lines from a LittleFS file
  String readLastNLFS(String path, int n) {
  std::vector<std::string> lastLines;

  File file = LittleFS.open(path, FILE_READ);

  if (!file) {
    logInfo("Failed to open file for reading");
    return "[]";
  }

  while (file.available()) {
    std::string line = file.readStringUntil('\n').c_str();
    if (lastLines.size() >= n) {
      lastLines.erase(lastLines.begin());  // Remove oldest line
    }
    lastLines.push_back(line);  // Add new line
  }

  file.close();

  // wrap in brackets and return as string
  String s = "";
  int numLines = 0;
  String line = "";
  s = "[";

  for (int i = 0; i < (lastLines.size()); i++) {
    s += lastLines[i].c_str();
  }
  //replace trailing comma with  ']'
  s[s.length() - 2] = ' ';
  s[s.length() - 1] = ']';
  return s;
}

void appendFile(fs::FS& fs, const char* path, const char* message) {
  logTrace("Appending to file: " + String(path));


  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    logInfo("- failed to open file for appending");
    return;
  }
  //println
  if (file.println(message)) {
    logTrace("- message appended: " + String(message));
  } else {
    logInfo("- append failed");
  }
  file.close();
}

void listAllFilesInDir(String dir_path) {
  // Open the specified directory
  Dir dir = LittleFS.openDir(dir_path);
  logInfo("==================== listing dir " + dir_path + "=====================");
  // Iterate through the directory contents
  while (dir.next()) {
    if (dir.isFile()) {
      // If it's a file, print its name and size
      logInfo("File: ");
      logInfo(dir_path + dir.fileName() + " size: " + String(dir.fileSize()));
    } else if (dir.isDirectory()) {
      // If it's a directory, print its name
      logInfo("Dir: ");
      logInfo(dir_path + dir.fileName() + "/");
    }
  }
  logInfo("==================== FS Stats ================");
  FSInfo info;
  LittleFS.info(info);
  logInfo("FS used:  " + formatNumberWithCommas(info.usedBytes) + " bytes");
  logInfo("FS total: " + formatNumberWithCommas(info.totalBytes) + " bytes");
  logInfo("==================== FS Stats ================");
}



double percentFull() {
  FSInfo info;
  LittleFS.info(info);
  logTrace("FS used:  " + formatNumberWithCommas(info.usedBytes) + " bytes");
  logTrace("FS total: " + formatNumberWithCommas(info.totalBytes) + " bytes");
  double x = info.usedBytes;
  double y = info.totalBytes;
  double fraction = x / y;
  logTrace(String(x) + "/" + String(y) + "=" + String(fraction));
  double percent = 100 * fraction;
  return percent;
}



void readFile(fs::FS& fs, const char* path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    logInfo("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}