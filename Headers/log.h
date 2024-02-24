/**
 * @file log.h
 * @brief This file contains the API for application logs
 * @authors lonelydevil nakira974
 * @date 24/02/2024
 */

#ifndef VULKAN_TRIANGLE_LOG_H
#define VULKAN_TRIANGLE_LOG_H

#include "stdio.h"

/**
 * Enum to define the application verbosity
 */
typedef enum {
    QUIET,
    FATAL,
    ERR,
    WARN,
    INFO,
    DEBUG,
    TRACE,
} LogLevel;




/**
 * Global log level variable for our app, MUST be called in your main program file
 */
extern LogLevel logLevel;

/**
 * Main program output rolling file
 */
extern FILE *   console;

/**
 * @brief Convert a LogLevel to string
 * @param level The log level to be converted into string
 * @return The given log level as string
 */
const char * log_level_toString(LogLevel);

/**
 * @brief Map the log level with program arguments
 * @param argc Arguments size
 * @param argv Program arguments
 */
void log_process_args(int argc, char * argv[]);

/**
 * @brief write a message to the console
 * @param caller Caller name
 * @param console Target console where to write the message
 * @param message Message to be written
 */
void log_write(LogLevel level, const char *caller, const char *message);
#endif //VULKAN_TRIANGLE_LOG_H
