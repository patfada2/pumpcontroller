
#ifndef LOG_H
#define LOG_H
void setupLogFile();
void logInfo(String msg);
void logTrace(String msg);
void writeLog(String msg);
const String LOG_FILE = "/log.txt";

#endif  