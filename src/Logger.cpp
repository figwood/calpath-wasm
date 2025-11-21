#include "Logger.h"
#include <iostream>
#include <ctime>

// Simple logger function
void log_message(LogLevel level, const std::string& message) {
  const char* levelStr[] = {"[ERROR]", "[WARN]", "[INFO]", "[DEBUG]"};
  time_t now = time(nullptr);
  char timeStr[20];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
  
  std::cerr << timeStr << " " << levelStr[level] << " " << message << std::endl;
}
