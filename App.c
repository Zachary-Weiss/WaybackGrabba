#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "sys/stat.h" //for doesDirExist()


//GLOBAL VARIABLES
const int MAX_URLS = 512; // We can track 511 urls per file



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

// Searches for a file name (not a path) recursively in the current dir
int fileExistsRec(char* fileName){
    char command[256] = "bash -c 'find . -type f -name ";
    strcat(command, fileName);
    strcat(command, " -print -quit'");

    FILE* pipe = popen(command, "r");
    if (pipe == NULL){
        printf("\n-- Couldn't open pipe.");
        return -1;
    }

    if (fgetc(pipe) != EOF){ // If we get even a single char of output, it means the file was found
        pclose(pipe);
        return 1;
    }
    return 0;
}


// Removes the last line of a file. Use this to remove the last line of SnapshotList so that if we have an issue grabbing a file from a specific timestamp, we can grab the next most recent one using the new last line of SnapshotList.txt (because the old last one was removed after we grabbed it)
int removeLastLine(char* fileName){
    if (!fileExists(fileName)){
        return 1;
    }
    
    // I had to separate this into two different command calls because of some weird issue where temp was marked as busy if I tried to create and move it in one command

    //remove the last line of the file
    char command[256] = "bash -c 'head -n -1 ";
    strcat(command, fileName); // "bash -c 'head -n -1 FILENAME"
    strcat(command, " > temp.txt;'");
    system(command);
    
    char command2[256] = "bash -c 'mv temp.txt ";
    strcat(command2, fileName); // "bash -c 'head -n -1 FILENAME temp.txt; mv temp.txt FILENAME"
    strcat(command, ";'");
    system(command2);

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


// Removes the first n chars of a string
void removeFirstNChars(char* str, int n){
    int len = strlen(str);
    int outIndex = 0;

    for (int i = n; i < len; i++){
        str[outIndex] = str[i];
        outIndex++;
    }

    for (outIndex; outIndex < len; outIndex++){
        str[outIndex] = '\0';
    }
}

//Doesn't clean the chars after the new null terminator. Call cleanAfterNull on the string afterwards if you want it to be clean.
void removeLastNChars(char* str, int n){
    // If they want to clean the whole string, do that instead
    if (n >= strlen(str)){
        for (int i = 0; i < strlen(str); i++){
            str[i] = '\0';
        }
        return;
    }

    str[(strlen(str)) - n] = '\0';
}




// WEBSITE LINK PARSING

// Returns a list of strings. The last pointer points to null, like a \0 in a string. The list can store up to 511 links. Don't forget to free the output!
char** getLinks(char* filePath){
    printf("hi");
    //First, attempt to open the file
    FILE* file = fopen(filePath, "r");
    if (!file){
        printf("Error opening file: %s", filePath);
        return NULL;
    }

    //Prepare to store the output
    char** urls = calloc(MAX_URLS, sizeof(char*)); // Room for 511 links and a null terminator
    if (!urls){
        printf("Can't make urls");
        return NULL;
    }

    int urlsPos = 0; // Current index in urls. Increment it every time you add a new url
    char c; // Stores the current char from the file
    char temp[512] = {'\0'}; // Temp storage to collect links char by char
    int tempPos = 0; // The first index in temp AFTER the string it's storing. This is the index we place a new char in when we want to append one to the str

    // For phase 1: We're searching for instances of "href" and "src"
    int currentPhase = 1;
    char href[5] = "href";
    char src[4] = "src";
    int hrefIndex = 0;
    int srcIndex = 0;

    // For phase 2: We're searching for open " or '. The char stores which one we found first, so we know that
    // the second instance of that one is the end of our url
    char quoteChar;

    // In phase 3, we're just searching for a second instance of quoteChar. For every char that isn't one, add it to
    // temp.

    
    //Begin to scan the file, char by char
    while ((c = fgetc(file)) != EOF){ // EOF is the end of file char
        putchar(c);
        //Need to check for "href" and "src". After I find one of the two, disregard every following character until I hit a 
        //double quote or a single quote. Remember whether it was a double or single quote. After we hit it,
        //append every following char to temp. Once we hit a second double/single quote:
        //1. Allocate space for temp and copy temp over to it. Then, set urls[urlsPos] = pointer to the copy of temp. Don't forget to increment urlsPos!
        //2. Clear temp (maybe by setting temp[0] = \0, maybe some other way)
        //3. Start listening for href and src again

        
        
        switch (currentPhase){
            case 1: // PHASE 1: Searching for "href" and "src"
                if (hrefIndex == 0 && srcIndex == 0){ // Search for both 's' and 'h' if we haven't found either yet
                    if (tolower(c) == href[hrefIndex]) hrefIndex++;
                    else if (tolower(c) == src[srcIndex]) srcIndex++;
                }
                else if (hrefIndex > 0){ // If href index is not 0, only search for the other chars to complete "href"
                    if (tolower(c) == href[hrefIndex]){
                        if (hrefIndex + 1 == 4){ // If c is the last char in "href", switch to phase 2
                            hrefIndex = 0;
                            srcIndex = 0; // Probably not needed, but just in case
                            currentPhase = 2;
                        }
                        else hrefIndex++;
                    }
                    else hrefIndex = 0; // Reset the index if a char doesn't match
                }
                else if (srcIndex > 0){
                    if (tolower(c) == src[srcIndex]){
                        if (srcIndex + 1 == 3){ // If c is the last char in "src", switch to phase 2
                            hrefIndex = 0; // Probably not needed, but just in case
                            srcIndex = 0;
                            currentPhase = 2;
                        }
                        else srcIndex++;
                    }
                    else srcIndex = 0;
                }
                else{
                    printf("\n-- Something is wrong. srcIndex = %d and hrefIndex = %d", srcIndex, hrefIndex);
                }
                break;


            case 2: // PHASE 2: Searching for a ' or ", signifying the beginning of the url. 
                if (c == '\''){
                    quoteChar = '\'';
                    currentPhase = 3;
                }
                else if (c == '\"'){
                    quoteChar = '\"';
                    currentPhase = 3;
                }
                break;
            

            case 3: // PHASE 3: Collecting chars until we hit a second instance of quoteChar, signifying the end of the url
                
            //Also break on #, in the case of links to html files that also link to a certain ID, because that could cause endless loops. Ex: without this fix in place, the system would treat skib.html and skib.html#paragraph as two different files, and we would get stuck in an endless loop because if we try to download skib.html#paragraph, we just end up with skib.html
            if (c == '#' || c == quoteChar){ // Save the url to urls when we hit another quoteChar
                    // save temp to urls
                    urls[urlsPos] = calloc(strlen(temp) + 1, sizeof(char)); // Allocate space for the new URL (we only need enough space to store the characters preceding the \0, plus one extra slot for a \0)
                    // Now we can copy temp over
                    for (int i = 0; i < sizeof(temp); i++){
                        urls[urlsPos][i] = temp[i];
                        if (temp[i] == '\0') break; // When we hit a \0, break after we add it to the end
                    }
                    printf("\n-- urlPos: %d\n", urlsPos);
                    printf("\n-- tempPos: %d", tempPos);
                    printf("\n-- Stored a url: %s\n", temp);
                    memset(temp, '\0', sizeof(temp)); // Reset temp
                    tempPos = 0;

                    urlsPos++;
                    if (urlsPos == MAX_URLS - 1){ // If we're out of space to store links, stop reading chars from the file.
                        goto doneReadingFile;
                    }
                    currentPhase = 1; // Back to phase 1!
                }
                else{
                    if (tempPos == sizeof(temp) - 1){ // If we have no more room in temp, print an error and continue
                        printf("\n-- Ran out of room storing url: %s", temp);
                    }
                    else{
                        temp[tempPos] = c;
                        tempPos++;
                    }
                }
                break;
        }
    }
    doneReadingFile: // If we run out of space to store links while reading the file, we jump here
    fclose(file); // Don't forget to close the file!
    urls[urlsPos] = NULL; // The first index after all of our links in urls will be NULL. The idea is the NULL pointer will be like a \0 in a string; signifying the end

    for (int i = 0; i < MAX_URLS; i++){
        if (urls[i] == NULL) break;
        printf("\n-- Stored url: index = %d, value = %s", i, urls[i]);
    }

    return urls;
}

// ONLY USE WITH THE char** CREATED BY getLinks(). OTHERWISE THE LOOP COULD BE ENDLESS
int freeUrlList(char** urls){
    for (int i = 0; i < MAX_URLS; i++){
        free(urls[i]);
    }
    free(urls);
}












//STRUCTURE MAIN WITH ALL FUNCTIONS BEFORE WRITING THEM SO I REMEMBER THE WORKFLOW
int main(int argc, char* argv[]){
    if (argc == 1){
        printf("-- The URL of the site on the wayback machine must be provided as the first argument.");
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
        printf("-- Date accepted: %s\n", date);
        fflush(stdout);
    }
    

    // GET THE MOST RECENT DATE OF THE FILE
    system("bash -c 'touch SnapshotList.txt;'"); // Note that if SnapshotList.txt already exists, this won't delete the contents

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


    int retrievedNonEmptyFile = 0;

    // When a valid, nonempty file is grabbed, this loop will be broken. If not, it will attempt to get earlier and earlier snapshots of the same file, until either a valid one is found or we run out of snapshots
    while (!isFileEmpty("SnapshotList.txt")){

        // TODO: Consider switching to perror() instead of using pOut for output validation
        // Get the date from the last line of SnapshotList.txt (most recent snapshot)
        pOut lastLineOut = readLastLine("SnapshotList.txt"); // throw an error if the last line is invalid
        if (lastLineOut.status != 0){
            printf("-- Error reading last line of SnapshotList.txt");
            return 3;
        }
        char* lastLine = (char*) lastLineOut.ptr;
        fflush(stdout);


        // Remove the last line of SnapshotList, so if we need to grab the next most recent snapshot (because there was an issue getting this one), it will be super easy
        removeLastLine("SnapshotList.txt");


        command[0] = '\0'; // reset the command array so I can fill it again (only need to reset index 0)
        strcat(command, "bash -c '"); // bash -c '
        strcat(command, "./ParseDateV.exe "); // bash -c './ParseDateV.exe 
        strcat(command, lastLine); // bash -c './ParseDateV.exe https://web.archive.org/web/NUMBER_GOES_HERE/...
        strcat(command, ";'"); // bash -c './ParseDateV.exe https://web.archive.org/web/NUMBER_GOES_HERE/...;'

        fflush(stdout);

        free(lastLine); // Don't need this no more

        FILE* pipe = popen(command, "r"); // I can open a pipe to capture the output of ParseDateV.exe
        char dateStr[15] = {0}; // This will store the date of the closest snapshot to the date arg (or most recent, if none was supplied). (the 15th char is '\0')

        if (fgets(dateStr, sizeof(dateStr), pipe) == NULL){ // capture the output of ParseDateV.exe
            printf("-- Error parsing date from last line of SnapshotList");
            return 4;
        }
        printf("-- Recieved target date: %s\n\n", dateStr);
        fflush(stdout);
        fclose(pipe); // We have our str, close the pipe


        
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


        // Create a string to store the dir structure. It will start as the timestamp dir
        char dirStructure[256] = {'\0'};
        memcpy(dirStructure, dateStr, sizeof(dateStr)); // Copy dateStr over to dirStructure since the dateStr will be the folder containing the stuff we requested
        char lsOutput[256] = {'\0'}; // Used inside the burrowing loop to extract the insides of directories

        char fileName[256] = {'\0'}; // Stores the name of the file. This will also be at the end of dirStructure after the loop, if everything goes correctly


        // Burrow into the retrieved dir structure to get the path to the file
        printf("\n\n\n\n-- Retrieving file...");
        while (dirExists(dirStructure)){

            // 3. Open a pipe and cd into that string UNLESS THE STRING IS "", in which case you can stay in the current dir
            char lsCommand[256] = "bash -c 'cd "; // This command will give me the next nested file/dir in the dir structure
            strcat(lsCommand, dirStructure); // bash -c 'cd DIRSTRUCTURE
            strcat(lsCommand, "; ls;'"); // bash -c 'cd DIRSTRUCTURE; ls;'

            // Collect the next file/dir using a pipe
            FILE* lsPipe = popen(lsCommand, "r");
            if (fgets(lsOutput, sizeof(lsOutput), lsPipe) == NULL){ // capture the output of lsCommand
                printf("\n-- Error getting the output of the command: %s. Continuing to a previous snapshot.\n\n\n\n", lsCommand);

                // If we can't get a file from this timestamp
                goto getTimestamp;
            }
            fclose(lsPipe); // close the pipe

            removeLastChar(lsOutput); // ls prints \n after every result, so we have to remove this \n
            strcpy(fileName, lsOutput); // When the loop breaks, this should be storing the name of the file

            //Add the new dir/file to dirStructure
            strcat(dirStructure, "/"); // CURRENT_STRUCTURE/
            strcat(dirStructure, lsOutput); // CURRENT_STRUCTURE/NEW_DIR_OR_FILE


            //FIXME: Is there a need for the following code?
            // Check if dirStructure is still the path to a dir
            if (dirExists(dirStructure)){
                //printf("\n-- Found dir: %s\n", dirStructure);
            }
        }

        // If it is now the path to a file, then we've found our file and can break the loop to extract it
        if (!fileExists(dirStructure)) {
            printf("\n\n-- Error getting path to retrieved file: %s", dirStructure);
            return 6;
        }

        // TODO: Eventually I'll probably want a way to check if we already have the file or not. If I decide on some kind of list of all files gathered, I should add the file name to it here

        printf("\n-- Found retrieved file: %s\n", dirStructure);
        printf("\n-- Filename: %s\n", fileName);

        // If the file is empty, try grabbing the next latest snapshot (by continuing the loop)
        if (isFileEmpty(dirStructure)){
            printf("\n-- File is empty. Attempting to fetch an earlier file.\n\n\n\n");
            continue;
        }


        //At this point, we know this is the file we want to keep.


        //Here, we get all of the href and src urls from the file
        char** urls = getLinks(dirStructure);
        freeUrlList(urls);


        // This contains the path to the file excluding the date folder (which contains everything else) and the file itself
        char dirStructureNoFile[256] = {'\0'};
        memcpy(dirStructureNoFile, dirStructure, strlen(dirStructure) - strlen(fileName)); // I could've just decreased the size in this call, I didn't need to call removeLastNChars...
        removeFirstNChars(dirStructureNoFile, strlen(dateStr) + 1); // Remove "DATESTR/" from the path
        

        // Create the dir structure to store the file in if it doesn't already exist (that's what the -p flag does in mkdir). Then, move the file to it
        char storeFileCommand[512] = "bash -c 'mkdir -p ";
        strcat(storeFileCommand, dirStructureNoFile); // "bash -c 'mkdir -p PATH/TO/MY/" <- doesn't have the actual file at the end, just the dir containing it
        strcat(storeFileCommand, "; mv "); // "bash -c 'mkdir -p PATH/TO/MY/; mv '"
        strcat(storeFileCommand, dirStructure); // "bash -c 'mkdir -p PATH/TO/MY/; mv DATE/PATH/TO/MY/FILE.EXTENSION"
        strcat(storeFileCommand, " "); // "bash -c 'mkdir -p PATH/TO/MY/; mv DATE/PATH/TO/MY/FILE.EXTENSION "
        strcat(storeFileCommand, dirStructureNoFile); // "bash -c 'mkdir -p PATH/TO/MY/; mv DATE/PATH/TO/MY/FILE.EXTENSION PATH/TO/MY/"
        strcat(storeFileCommand, ";'");
        system(storeFileCommand);



        getTimestamp: // This is a label. In some cases, we need to progress immediately to the next snapshot but can't do it using the continue keyword because we're inside a nested loop. To solve this, we can use the goto keyword to jump to this label.
    


        // Now we can move the timestamp dir to the trash folder because we've extracted the file from it
        command[0] = '\0'; // reset the command string so I can reuse it
        strcat(command, "bash -c 'mv -f ");
        strcat(command, dateStr); // bash -c 'mv DATESTR
        strcat(command, " trash;'"); // bash -c 'mv DATESTR trash;'
        system(command);

        //If we make it here, it means we haven't hit a 'continue' or 'goto getTimestamp', meaning our file exists and isn't empty.
        //This means we can safely break out of the loop.
        retrievedNonEmptyFile = 1;
        break;
    }

    // Here, either we have the file or we have run out of snapshots. We will know which is the case based on the value of retrievedNonEmptyFile
    if (retrievedNonEmptyFile){
        printf("\n-- Successfully retrieved file!\n");
    }
    else{
        printf("\n-- Couldn't find a non-empty snapshot of the file. Maybe try using a more recent date?\n");
    }

    //Now I parse out everything before the website
    


    return 0;
}