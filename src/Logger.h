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

// Set default log level based on build type
#ifdef NDEBUG
  // Release build: only show errors
  #define DEFAULT_LOG_LEVEL LOG_ERROR
#else
  // Debug build: show info and above
  #define DEFAULT_LOG_LEVEL LOG_INFO
#endif

// Simple logger function
void log_message(LogLevel level, const std::string& message);

// Set the minimum log level to display
void set_log_level(LogLevel level);

#endif // LOGGER_H
