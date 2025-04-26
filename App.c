#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "sys/stat.h" //for doesDirExist()


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


    // 255 char max per line
    char* line = (char*) malloc(256 * sizeof(char));

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


// Checks if a file is empty or not
int isFileEmpty(char* fileName){
    FILE* file = fopen(fileName, "r");

    // If there is a char in the file, it isn't empty
    if (fgetc(file) != EOF){
        fclose(file);
        return 0;
    }

    // Otherwise, the file is empty
    fclose(file);
    return 1;
}

// Checks in the current dir for a dir with a name matching dirName. This also works for nested directories, so I can say dirExists("skib/skibidi")
int dirExists(char* dirName){
    struct stat buffer;
    if (stat(dirName, &buffer) == 0 && S_ISDIR(buffer.st_mode)){ // if the name exists and it's a directory...
        return 1;
    }
    return 0;
}

// Checks in the current dir for a regular file with a name matching fileName
int fileExists(char* fileName){
    struct stat buffer;
    if (stat(fileName, &buffer) == 0 && S_ISREG(buffer.st_mode)){ // if the name exists and it's a directory...
        return 1;
    }
    return 0;
}

int removeLastLine(char* fileName){
    if (!fileExists(fileName)){
        return 1;
    }
    
    //remove the last line of the file
    char command[256] = "bash -c 'head -n -1 ";
    strcat(command, fileName); // "bash -c 'head -n -1 FILENAME"
    strcat(command, " > temp.txt; mv temp.txt ");

    return 0;
}

//Changes everything after \n in a string to \n. Probably useless.
void cleanAfterNull(char* str, int length){
    int foundNull = 0;
    for (int i = 0; i < length; i++){
        if (!foundNull && str[i] == '\0'){
            foundNull = 1;
        }
        if (foundNull){
            str[i] = '\0';
        }
    }
}

// Changes the last character before the \n in a str to \n
void removeLastChar(char* str){
    int len = strlen(str);
    if (len >= 1){
        str[len - 1] = '\0';
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
    printf("\n-- Calling command: %s\n-- Waiting for response...\n\n", command);
    fflush(stdout);
    system(command);
    printf("-- Response recieved!\n\n");
    
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

    printf("\n-- %s\n\n", command);
    fflush(stdout);

    FILE* pipe = popen(command, "r"); // I can open a pipe to capture the output of ParseDateV.exe
    char dateStr[15] = {0}; // This will store the date of the closest snapshot to the date arg (or most recent, if none was supplied). (the 15th char is '\0')

    if (fgets(dateStr, sizeof(dateStr), pipe) == NULL){ // capture the output of ParseDateV.exe
        printf("Error parsing date from last line of SnapshotList");
        return 4;
    }
    printf("-- Recieved target date: %s\n\n", dateStr);
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


    printf("-- Running command: %s\n-- Waiting for response...\n\n", getFileCommand);
    fflush(stdout);
    system(getFileCommand);
    printf("-- Response recieved!\n");

    
    //TODO: Here I need to:   
    //TODO: 1. cd into timestamp dir // Is this necessary/possible?


    //TODO: 2. Create a string to store the dir structure. It will start as the timestamp dir
    char dirStructure[256] = {'\0'};
    memcpy(dirStructure, dateStr, sizeof(dateStr)); // Copy dateStr over to dirStructure since the dateStr will be the folder containing the stuff we requested
    char lsOutput[256] = {'\0'};

    while (dirExists(dirStructure)){
        //TODO: 3. Open a pipe and cd into that string UNLESS THE STRING IS "", in which case you can stay in the current dir
        char lsCommand[256] = "bash -c 'cd "; // This command will give me the next nested file/dir in the dir structure
        strcat(lsCommand, dirStructure); // bash -c 'cd DIRSTRUCTURE
        strcat(lsCommand, "; ls;'"); // bash -c 'cd DIRSTRUCTURE; ls;'

        // Collect the next file/dir using a pipe
        FILE* lsPipe = popen(lsCommand, "r");
        if (fgets(lsOutput, sizeof(lsOutput), lsPipe) == NULL){ // capture the output of lsCommand
            printf("Error getting the output of the command: %s", lsCommand);
            return 5;
        }
        fclose(lsPipe); // close the pipe

        removeLastChar(lsOutput); // ls prints \n after every result, so we have to remove this \n

        //Add the new dir/file to dirStructure
        strcat(dirStructure, "/"); // CURRENT_STRUCTURE/
        strcat(dirStructure, lsOutput); // CURRENT_STRUCTURE/NEW_DIR_OR_FILE

        // Check if dirStructure is still the path to a dir
        if (dirExists(dirStructure)){
            printf("\nFound dir: %s\n", dirStructure);
        }
    }

    // If it is now the path to a file, then we've found our file and can break the loop to extract it
    if (fileExists(dirStructure)){
        printf("\nFound file: %s\n", dirStructure);
    }
    else {
        printf("Ruh roh");
    }




    //TODO: 4. Store the output of "ls ." in a temp string variable
    //TODO: 5. Check if dirExists(<the temp string variable>)

    //TODO: 6. If it does, add the temp string variable to the string containing the dir structure.
    //TODO: 7. Repeat steps 3-6 until dirExists(<temp string variable>) is false. This means we have reached the file.
    //TODO: 8. Run 'mkdir -p DIR_STRUCTURE_STRING' from a new console to ensure we're calling it from "." This will create any directories in that path that don't already exist.
    //TODO: 9. Move the file from timestamp/DIR_STRUCTURE

    //TODO: If you reach the end of the dir structure and there is no file, we need to delete the timestamp dir and everything inside and then roll back the timestamp to the next most recent line in SnapshotList.txt and try getting it again

    // need to figure out directory management...

    free(lastLine);

    //Now I parse out everything before the website

    


    return 0;
}