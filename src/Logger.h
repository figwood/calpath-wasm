#ifndef LOGGER_H
#define LOGGER_H

#include <string>

// Simple logger levels
enum LogLevel {
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG
};

// Simple logger function
void log_message(LogLevel level, const std::string& message);

#endif // LOGGER_H
