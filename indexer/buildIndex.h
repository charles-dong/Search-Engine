/*

  FILE: buildIndex.h

  Description: header file for creating an index

*/

#ifndef _BUILDINDEX__H_
#define _BUILDINDEX__H_

#define MAX_WORD_LENGTH 300


//creates an index from the files found in the directory passed
DICTIONARY *createIndex(char *path);


#endif

