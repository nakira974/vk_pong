//
// Created by maxim on 24/02/2024.
//
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "log.h"
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
            }
        }
    }
}

void log_write(const char *message){
    console = fopen("console.log", "a");
    if(console != NULL){
        time_t  dateTimeNow;
        time(&dateTimeNow);
        struct tm *localTime = localtime(&dateTimeNow);
        char dateTimeString[80];
        strftime(dateTimeString, sizeof(dateTimeString), "%Y-%m-%d %H:%M:%S", localTime);
        fprintf(console,  "[%s] \t%s\n",dateTimeString, message);
        fclose(console);
    }
}