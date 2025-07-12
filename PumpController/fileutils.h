#include <LittleFS.h>
#include <FS.h>


#define FORMAT_LITTLEFS_IF_FAILED true


//const char* FILE_WRITE="w";
#define FILE_READ "r"
#define FILE_WRITE "w"
#define FILE_APPEND "a"


void listDir(fs::FS& fs, const char* dirname) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname, FILE_READ);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());

      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());

      file = root.openNextFile();
    }
  }
}

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
