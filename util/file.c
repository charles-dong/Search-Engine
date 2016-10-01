
/*

  FILE: file.c

  Description: file manipulation functions

  Functions: getPage, NormalizePath, getNextWordFromHTMLDoc, readFileToBuffer, isDirectory, sortFile

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include "../crawler/crawler.h"
#include "dictionary.h"
#include "../crawler/list.h"
#include "header.h"
#include "file.h"
#include "hash.h"
#include "html.h"
#include "../indexer/buildIndex.h"

/*

*getPage*
------------

Description: Download a file from the internet using wget into temp. Read temp
  into a string that is returned by getPage. Store temp to a file named fileNumber (1... N)
  and save the URL and depth on the first and second lines
  Note: returns NULL if wget fails

Input: url, currentDepth, path to directory to store file in, file number

Return: pointer to char array holding HTML of page we wget'ed 

** Pseudo Code **

(1) wget the page into temp; if wget fails, retry 3 times and then return NULL
(2) check if temp exists and is readable
(3) get the number of characters in temp
(4) create a char array large enough to hold the HTML, and copy temp to it
(5) create a new file named fileNumber, and check if it exists
(6) open the new file and append url, depth, html
(7) clean up and return the char array with the html in it

*****

*/
char *getPage(char* url, int depth, char* path, int* fileNumber){

  /* wget the page */
  char command[MAX_URL_LENGTH];
  memset(command, 0, sizeof(command));
  char tempFile[20];
  strcpy(tempFile, path);
  strcat(tempFile, "temp.html");
  int len = snprintf(command, sizeof(command),"wget -q -O %s '%s'", tempFile, url);
  if ( len > sizeof(command) ) {
    fprintf(stderr, "Command buffer too short.\n");
    exit(EXIT_FAILURE);
  }
  int retry = 0;
  int wgetStatus;
  while ( retry < 3) {
    wgetStatus = system(command);
    if (wgetStatus == 0) {
      break;
    }
    else {
      sleep(INTERVAL_PER_FETCH);
      retry++;

      #ifdef DEBUG
      /*open new file with name logOutput and append*/
      char *f = "logOutput";
      FILE *newFile = fopen(f,"a");
      fprintf(newFile, "Try %d to wget %s failed.\n", retry, url);
      fclose(newFile);
      #endif

      if (retry >= 2 ){

        #ifdef DEBUG
        /*open new file with name logOutput and append*/
        char *f = "logOutput";
        FILE *newFile = fopen(f,"a");
        fprintf(newFile, "Failed to wget %s.\n", url);
        fclose(newFile);
        #endif
        unlink(tempFile);
        return NULL;
      }
    }
  }

  /* get temp.html into a buffer */
  char *pageBuffer = readFileToBuffer(tempFile);
  if (pageBuffer == NULL){
    unlink(tempFile);
    return NULL;
  }
  
  /*create new fileName*/
  char newFileName[100];
  strcpy(newFileName, path);
  char t[100];
  snprintf(t, sizeof(t), "%d", *fileNumber);
  strcat(newFileName, t);

  /* check if the file already exists */
  if( access( newFileName, R_OK ) != -1 ) {
     fprintf(stderr, "The file %s already seems to exist.\n", newFileName);
     unlink(tempFile);
     return NULL;
  }

  /*open new file with newFileName and append url, depth, html*/
  FILE * newFile = fopen(newFileName,"a");
  fputs(url, newFile);
  fputc('\n', newFile);
  fprintf(newFile, "%d\n", depth);
  int res = fputs(pageBuffer, newFile);
  if (res == EOF) {
      #ifdef DEBUG
      /*open new file with name logOutput and append*/
      char *f = "logOutput";
      FILE *newFile = fopen(f,"a");
      fprintf(newFile, "There was an error writing pageBuffer to %s.\n", newFileName);
      fclose(newFile);
      #endif
      unlink(tempFile);
     return NULL;
  }
  fputc(EOF, newFile);

  /*cleanup*/
  #ifdef DEBUG
  /*open new file with name logOutput and append*/
  char *f = "logOutput";
  FILE *newF = fopen(f,"a");
  fprintf(newF, "%s - crawled\n", url);
  printf("%s CRAWLED\n", url);
  fclose(newF);
  #endif
  *fileNumber = *fileNumber + 1;
  fclose(newFile);
  unlink(tempFile);
  return pageBuffer; 

}

/*

*NormalizePath*
--------------

Description: Makes sure path ends with a trailing slash '/' character
returns pointer to new char array

Input: path

Output: normalized path

*****

*/
char *NormalizePath(char *path){
  int len = strlen(path);

  //if there's a trailing backslash, just return the passed path
  if (path[len - 1] == '/') {
    return path;
  }
  else {
    //create a new char array to hold the path
    char * normalPath = malloc(sizeof(char)*(len + 2));
    memset(normalPath, 0, sizeof(char)*(len + 2));
    //copy the new URL into it
    strcpy(normalPath, path);

    //add the backslash
    normalPath[len] = '/';

    return normalPath;
  }
}

/*

*getFileLength*
--------------

Description: returns fileLength + 1 of passed file

Input: file

Output: number of characters

*****

*/

int getFileLength(FILE *file){
  if (file == NULL){
    return 0;
  }
  fseek(file, 0, SEEK_END);
  int num = ftell(file);
  rewind(file);
  return num;
}


/*

*isDirectory*
-------------

Description: See if the input path is a directory; returns 1 if it is and 0 otherwise.

Input: input_path

Return: int signal 1 - if the input path is a directory
                   0 - otherwise

*****

*/

int isDirectory(char* path) {
  DIR* directory = opendir(path);
  if (directory)
  {
      //valid directory
      closedir(directory);
      return 1;
  }
  else {
      //not a directory
      return 0;
  }
}

/*

*readFileToBuffer*
--------------

Description: reads fileName to buffer and returns the buffer

Input: fileName to read

Output: buffer of fileName's contents (or NULL for fail)

*****

*/
char *readFileToBuffer(char *fileName){

  /* check if readable file exists */
  if( access( fileName, R_OK ) == -1 ) {
      #ifdef DEBUG
      printf("%s is not a readable file.\n", fileName);
      #endif
      return NULL;
  }

  /* Get the number of characters in the file */
  FILE *temp = fopen(fileName, "r");
  int numChars = getFileLength(temp);
  if (numChars == 0) {
    fclose(temp);
    #ifdef DEBUG
    printf("File %s is empty.\n", fileName);
    #endif
    return NULL;
  }

  /* Allocate appropriate number of characters to buffer */
  char *buffer = malloc(sizeof(char)*numChars + 1);
  MALLOC_CHECK(buffer);
  memset(buffer, 0, sizeof(char)*numChars + 1);

  /*copy file to buffer*/
  long bufSize = fread(buffer, sizeof(char), numChars, temp);
  if (bufSize != numChars) {
    free(buffer);
    #ifdef DEBUG
    printf("Error copying %s to pageBuffer.\n", fileName);
    #endif
    return NULL;
  }

  fclose(temp);
  return buffer;
}


/*

*getNextWordFromHTMLDoc*
--------------

Description: returns the next word from the HTML doc passed as pageBuffer, skipping the first two lines
  and the HTML tags

Input: string to parse, empty string to write word to

Output: new currentPosition to resume at

*****
*/
int getNextWordFromHTMLDoc(char *pageBuffer, char *word, int currentPosition){

  char currentChar = pageBuffer[currentPosition];
  //if we haven't reached the end of the file..
  while ( currentChar != 0 ) {

    //if we're at the beginning of the file, skip the first two lines and find the first HTML tag
    if (currentPosition == 0) {
      int linesToSkip = 2;
      while (linesToSkip){
        currentChar = pageBuffer[++currentPosition];
        if (currentChar == '\n'){
          linesToSkip--;
        }
      }
    }

    //if we've reached an HTML tag, skip it
    if (currentChar == '<'){
      while ( currentChar != '>' && currentChar != 0) {
        currentChar = pageBuffer[++currentPosition];
      }

      //if we've reached the end of the file
      if (currentChar == 0)
        return 0;
      //we've reached the end of the HTML tag, get the next char
      else
        currentChar = pageBuffer[++currentPosition];
    }

    //if we've reached a new word.. yay
    if (isalpha(currentChar)){
      //setup
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      int index = 0;

      //copy the new word into 'word' and normalize
      while (isalpha(currentChar)){
        word[index] = currentChar;
        currentChar = pageBuffer[++currentPosition];
        index++;
      }
      NormalizeWord(word);

      //return our new position (not alphabetic)
      return currentPosition;
    }
    //we've reached another HTML tag or random non-alphabetic character..
    else {
      //if we've reached another HTML tag, continue
      if (currentChar == '<'){
        continue;
      }
      //if we've reached a non-alphabetic character, increment and continue
      else{
        currentChar = pageBuffer[++currentPosition];
        continue;
      }
    }
  }

  //we've reached the end of the file
    return 0;
}


/*

*sortFile*
--------------

Description: sorts contents of fileName alphabetically by the first letter of every line

Input: file to sort

Output: 1 for success, 0 for failure

*****
*/
int sortFile(char *fileName){
  char command[100];
  memset(command, 0, sizeof(command));
  int len = sprintf(command, "sort '%s' -o '%s'", fileName, fileName);
  if ( len > sizeof(command) ) {
    fprintf(stderr, "Command buffer too short.\n");
    exit(EXIT_FAILURE);
  }
  int sortStatus = system(command);
  //if successful
  if (sortStatus == 0)
    return 1;
  else
    return 0;
}











//puts next word in query into 'word'
//returns position to start looking for next word in query
int getNextWord(char *query, char *word, int currentPosition){

  char *and = "AND";
  char *or = "OR";
  char currentChar = query[currentPosition];
  //if we haven't reached the end of the query..
  while ( currentChar != 0 ) {

    //if we've reached the end of the file
    if (currentChar == 0)
      return 0;

    //if we've reached a word
    if (isalpha(currentChar)){
      //setup
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      int index = 0;

      //copy the new word into 'word' and normalize
      while (isalpha(currentChar)){
        word[index] = currentChar;
        currentChar = query[++currentPosition];
        index++;
      }
      //if it's an AND or OR, do not normalize
      if ((strcmp(word, and) == 0 && strlen(word) == 3) || (strcmp(word, or) == 0 && strlen(word) == 2)){
        //do nothing
      }
      else {
        NormalizeWord(word);
      }
      //return our new position (not alphabetic)
      return currentPosition;

    }
    else {
      //if we've reached a non-alphabetic character, increment and continue
        currentChar = query[++currentPosition];
        continue;
    }
    
  }

  //we've reached the end of the file
  return 0;
}





