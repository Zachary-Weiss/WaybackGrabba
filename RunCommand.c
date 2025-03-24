#include "stdio.h"
#include "string.h"
#include "stdlib.h"

// ARGS FOR THIS FUNCTION MUST BE IN '', NOT DOUBLE QUOTES


// Calls a string of bash commands separated by ; or && or || in a subshell (for linux, mac, and windows with git bash)
int bashShell(const char *command){
    char combined[strlen(command) + 12]; //not sure exactly how much space I need;
    sprintf(combined, "%s%s%s", "bash -c \"", command, "\"");
    printf("%s", combined);
    return system(combined);
}


// same as bashShell but for windows cmd
// int windowsShell(const char *command){
//     char combined[strlen(command) + 11]; //same space question as above
//     sprintf(combined, "%s%s%s", "cmd /c \"", command, "\"");
//     return system(combined);
// }

int main(int argc, char* argv[]) {
    //roll the args into one string
    char command[1024] = "";
    for (int i = 1; i < argc; i++) {
        strcat(command, argv[i]);
        strcat(command, " ");
    }

    bashShell(command);

    return 0;
}