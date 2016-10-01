/*

  FILE: file.h

  Description: header for file manipulation functions 

*/

#ifndef _FILE__H_
#define _FILE__H_

/*Description: Download a file from the internet using wget into temp. Read temp
  into a string that is returned by getPage. Store temp to a file named fileNumber (1... N)
  and save the URL and depth on the first and second lines
  Note: returns NULL if wget fails */
char *getPage(char* url, int depth,  char* path, int* fileNumber);

// Adds backslash to directory path if needed
char *NormalizePath(char *path);

//reads fileName to buffer and returns the buffer
char *readFileToBuffer(char *fileName);

//See if the input path is a directory; returns 1 if it is and 0 otherwise.
int isDirectory(char* path);



/* indexer */

//gets the next word from the HTML doc passed as pageBuffer into 'word', skipping the first two lines
  //and the HTML tags. Returns the new currentPosition
int getNextWordFromHTMLDoc(char *pageBuffer, char *word, int currentPosition);

//sorts contents of fileName alphabetically by the first letter of every line
int sortFile(char *fileName);


/* query */

//returns next word in query
int getNextWord(char *query, char *word, int currentPosition);

#endif
