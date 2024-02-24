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
    TRACE,
    INFO,
    WARN,
    ERR,
    FATAL
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
 * @brief Map the log level with program arguments
 * @param argc Arguments size
 * @param argv Program arguments
 */
void log_process_args(int argc, char * argv[]);

/**
 * @brief write a message to the console
 * @param console Target console where to write the message
 * @param message Message to be written
 */
void log_write(const char *message);
#endif //VULKAN_TRIANGLE_LOG_H
