#include "Logger.h"
#include <iostream>
#include <ctime>

// Current minimum log level
static LogLevel current_log_level = DEFAULT_LOG_LEVEL;

// Set the minimum log level to display
void set_log_level(LogLevel level) {
  current_log_level = level;
}

// Simple logger function
void log_message(LogLevel level, const std::string& message) {
  // Only log if the message level is at or above the current log level
  if (level > current_log_level) {
    return;
  }
  
  const char* levelStr[] = {"[ERROR]", "[WARN]", "[INFO]", "[DEBUG]"};
  time_t now = time(nullptr);
  char timeStr[20];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
  
  std::cerr << timeStr << " " << levelStr[level] << " " << message << std::endl;
}
