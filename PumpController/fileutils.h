#include <LittleFS.h>
#include <FS.h>


#define FORMAT_LITTLEFS_IF_FAILED true


//const char* FILE_WRITE="w";
#define FILE_READ "r"
#define FILE_WRITE "w"
#define FILE_APPEND "a"

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

void listAllFilesInDir(String dir_path) {
  // Open the specified directory
  Dir dir = LittleFS.openDir(dir_path);

  // Iterate through the directory contents
  while (dir.next()) {
    if (dir.isFile()) {
      // If it's a file, print its name
      Serial.print("File: ");
      Serial.println(dir_path + dir.fileName());
    } else if (dir.isDirectory()) {
      // If it's a directory, print its name and recursively list its contents
      Serial.print("Dir: ");
      Serial.println(dir_path + dir.fileName() + "/");
      //listAllFilesInDir(dir_path + dir.fileName() + "/"); // Recursive call for subdirectories
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
