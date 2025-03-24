#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"


//STRUCTS

typedef struct String String;
struct String{
    int length;
    char* str;
};

// theorizin' that it's better to create pointers to large structs rather than
// copying them over to stack memory
String* makeStringHeap(int len, char* s){
    //allocate memory for a new String struct
    String* newStr = (String*) malloc(sizeof(String));

    //assign the values
    newStr->str = s;
    newStr->length = len;

    return newStr;
}

// This returns the value of the struct rather than a pointer to it, so you don't need to free it
String makeStringStack(int len, char* s){
    return (String){len, s};
}

// Sometimes I have a function that returns a pointer but might also have an error. Since there aren't exceptions,
// it might be difficult to discern where in the function the error occurred if I just return NULL. So, this struct
// allows me to return a pointer and also a status code, which will help with debugging.
typedef struct PointerOutput pOut;
struct PointerOutput{
    int status;
    void* ptr;
};

//note that this is in stack memory, not heap memory
pOut makepOut(int status, void* ptr){
    return (pOut) {status, ptr};
}






//HELPER FUNCTIONS

int isNumeric(char* str, int len){
    for (int i = 0; i < len; i++){
        if (isdigit(str[i]) == 0){
            return 0;
        }
    }
    return 1;
}

//returns the index of the first character of the substr, or -1 if it doesn't contain the substr
int hasSubStr(char* str, int strLen, char* subStr, int subStrLen){
    int foundSubStr = 0;
    int strIndex = 0;

    //for each char in the string, iterate forward through both the string and substring until we run out of string, run out of
    // substring, or we find a char in the string that doesn't match
    for (strIndex; strIndex <= strLen - subStrLen; strIndex++){
        printf("Checking str char: %c\n", str[strIndex]);

        //iterate ahead through the url (substring)
        for (int i = 0; i < subStrLen; i++){
            printf("Checking subStr char: %c against str char: %c\n", subStr[i], str[strIndex + i]);
            if (subStr[i] != str[i + strIndex]){ //if the strings don't match, break the loop
                printf("They don't match\n");
                break;
            }

            else { //if a match is found and it's the last char of the substring, we're good
                printf("They match!\n");
                if (i == subStrLen - 1){
                    printf("Found the full string!\n");
                    foundSubStr = 1;
                    break;
                }
            }
        }

        if (foundSubStr){
            break;
        }
    }
    if (foundSubStr){
        return strIndex;
    }
    else {
        return -1;
    }
}




//return the last line of a file
pOut readLastLine(char* fileName){


    //ALLOCATE HEAP MEMORY FOR LINE


    FILE* file = fopen(fileName, "r"); // Attempt to open the file
    if (file == NULL){
        printf("Error opening file");
        return makepOut(1, NULL);
    }


    char* line = (char*) malloc(256 * sizeof(char));
    // 255 char max per line
    //char line[256]; // store the current line

    while (fgets(line, 256, file) != NULL){ // navigate to the end of the file
    }

    fclose(file);

    printf("Last line from %s: %s\n", fileName, line);

    //return makepOut(0, line);
    return makepOut(0, line);
}


// Accepts a string and a substring to parse out. The substring and everything following it will be returned, or NULL if there's an error.
pOut parseURL(char* str, int strLength, char* url, int urlLength){
    int urlStartIndex = hasSubStr(str, strLength, url, urlLength);

    //Check if the URL was contained in the string
    if (urlStartIndex == -1){
        return makepOut(1, NULL);
    }
}



//STRUCTURE MAIN WITH ALL FUNCTIONS BEFORE WRITING THEM SO I REMEMBER THE WORKFLOW
int main(int argc, char* argv[]){
    if (argc == 1){
        printf("The URL of the site on the wayback machine must be provided as the first argument.");
        return 1;
    }

    //get the URL argument
    char* url = argv[1];
    char* date = "";
   
    //if the date arg is given...
    printf("Argc = %d\n", argc);
    if (argc > 2){
        if (!isNumeric(argv[2], strlen(argv[2])) || strlen(argv[2]) > 14){
            printf("The second argument must be the latest date to accept snapshots from, in the format:  YYYYMMDDhhmmss, leaving out as many trailing digits as you'd like");
            return 2;
        }

        date = argv[2];
        printf("Date accepted: %s\n", date);
        fflush(stdout);
    }
    

    // GET THE MOST RECENT DATE OF THE FILE
    system("bash -c 'touch SnapshotList.txt;'");

    char command[256] = "bash -c 'waybackpack "; // concatenate the command "bash -c 'waybackpack URL (--to-date DATE) --list >> SnapshotList.txt'"
    strcat(command, url); // "waybackpack URL"
    
    if (date != ""){ 
        strcat(command, " --to-date ");
        strcat(command, date); // "bash -c 'waybackpack URL --to-date DATE"
    }

    strcat(command, " --list >> SnapshotList.txt;'"); //"bash -c 'waybackpack URL (--to-date DATE) --list >> SnapshotList.txt'"


    // Fill ./SnapshotList.txt with the url of every snapshot instance of the site (so we can get the dates later)
    printf("\n Calling command: %s\nWaiting for response...\n", command);
    fflush(stdout);
    system(command);
    printf("Response recieved!\n");
    
    // TODO: Consider switching to perror() instead of using pOut for output validation
    // Get the date from the last line of SnapshotList.txt (most recent snapshot)
    pOut lastLineOut = readLastLine("SnapshotList.txt"); // throw an error if the last line is invalid
    if (lastLineOut.status != 0){
        printf("Error reading last line of SnapshotList.txt");
        return 3;
    }
    char* lastLine = (char*) lastLineOut.ptr;
    fflush(stdout);


    command[0] = '\0'; // reset the command array so I can fill it again (only need to reset index 0)
    strcat(command, "bash -c '"); // bash -c '
    strcat(command, "./ParseDateV.exe "); // bash -c './ParseDateV.exe 
    strcat(command, lastLine); // bash -c './ParseDateV.exe https://web.archive.org/web/NUMBER_GOES_HERE/...
    strcat(command, ";'"); // bash -c './ParseDateV.exe https://web.archive.org/web/NUMBER_GOES_HERE/...;'

    printf("\n%s\n", command);
    fflush(stdout);

    FILE* pipe = popen(command, "r"); // I can open a pipe to capture the output of ParseDateV.exe
    char dateStr[15] = {0}; // This will store the date of the closest snapshot to the date arg (or most recent, if none was supplied). (the 15th char is '\0')

    if (fgets(dateStr, sizeof(dateStr), pipe) == NULL){ // capture the output of ParseDateV.exe
        printf("Error parsing date from last line of SnapshotList");
        return 4;
    }
    printf("Recieved target date: %s\n", dateStr);
    fflush(stdout);
    fclose(pipe); // We have our str, close the pipe


    // TODO: Sometimes, a snapshot of a file can be gathered successfully but is empty. 
    // Sometimes a snapshot can be found but not downloaded. 
    // This seems to be dependent on the year we accept. 
    // Maybe implement some sort of checker that tries the previous snapshot 
    // if this one had an error or returned an empty file?

    
    // NEXT I NEED TO CREATE THE COMMAND TO GRAB THE FIRST SITE
    char getFileCommand[256] = "bash -c 'waybackpack ";
    strcat(getFileCommand, url); // "bash -c 'waybackpack URL"
    strcat(getFileCommand, " --raw -d . --from-date "); // "bash -c 'waybackpack URL --raw -d . --from-date "
    strcat(getFileCommand, dateStr); // "bash -c 'waybackpack URL --raw -d . --from-date DATE"
    strcat(getFileCommand, " --to-date "); // "bash -c 'waybackpack URL --raw -d . --from-date DATE --to-date "
    strcat(getFileCommand, dateStr); // "bash -c 'waybackpack URL --raw -d . --from-date DATE --to-date DATE"
    strcat(getFileCommand, "'"); // "bash -c 'waybackpack URL --raw -d . --from-date DATE --to-date DATE'"


    printf("Running command: %s\nWaiting for response...\n", getFileCommand);
    fflush(stdout);
    system(getFileCommand);
    printf("Response recieved!\n");

    // need to figure out directory management...

    free(lastLine);

    //Now I parse out everything before the website

    


    return 0;
}