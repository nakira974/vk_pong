//
// Created by maxim on 24/02/2024.
//
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include "log.h"



const char * log_level_toString(LogLevel level){
    const char * log_levels[] = {"Quiet", "Fatal", "Error", "Warning", "Info", "Debug", "Trace"};
    return log_levels[level];
}


void to_lower_case(char* str){
    size_t length = strlen(str);

    for (int i = 0; i < length; i++)
        str[i] = (char) tolower((int)str[i]);

}
void log_process_args(int argc, char * argv[]){
    for(int i =1;i<argc;i++){
        if(strcmp(argv[i], "--verbosity") == 0){
            // Check if next argument exists
            if(i+1 < argc){
                // map the next argument to the corresponding log level
                to_lower_case(argv[i+1]);
                if(strcmp(argv[i+1], "trace") == 0){
                    logLevel = TRACE;
                }
                else if(strcmp(argv[i+1], "quiet") == 0){
                    return;
                } else if (strcmp(argv[i+1], "info") == 0){
                    logLevel = INFO;
                }else if (strcmp(argv[i+1], "warn") == 0){
                    logLevel = WARN;
                }
                else if (strcmp(argv[i+1], "err") == 0){
                    logLevel = ERR;
                }
                else if (strcmp(argv[i+1], "fatal") == 0){
                    logLevel = FATAL;
                }
                else if (strcmp(argv[i+1], "debug") == 0){
                    logLevel = DEBUG;
                }
            }
        }
    }
}

void log_write(LogLevel level, const char *caller, const char *message){
    if(level<=logLevel){
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        console = fopen("console.log", "a");
        if(console != NULL){
            time_t  dateTimeNow;
            time(&dateTimeNow);
            struct tm *localTime = localtime(&dateTimeNow);
            char dateTimeToString[80];
            strftime(dateTimeToString, sizeof(dateTimeToString), "%Y-%m-%d %H:%M:%S", localTime);

            // Critical section
            pthread_mutex_lock(&mutex);
            const char* levelToString = log_level_toString(level);
            fprintf(console, "[%s]\t [%s]\t %s\t %s\n", dateTimeToString, levelToString, caller, message);
            // Critical section End
            pthread_mutex_unlock(&mutex);

            fclose(console);
        }
    }
}
