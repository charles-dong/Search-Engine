/*

  FILE: buildIndex.c

  Description: creates an index from the files found in the directory passed

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "../util/dictionary.h"
#include "../util/file.h"
#include "../util/header.h"
#include "../util/hash.h"
#include "../util/html.h"
#include "./buildIndex.h"

//filter for scandir - support function
int numFilter(const struct dirent *entry);

/*

MAIN INDEXER FUNCTION
------------

Description: creates an index from the files found in the directory passed and saves
	it as a fileName.dat that's passed. If it's passed five arguments, rebuilds the 
	Index Dictionary and saves it as new_fileName.dat 

Input: directory of files to index, fileName.dat to save as, and potentially the 
	new fileName to save the recreated index dictionary to

Return: n/a

** Pseudo Code **

(1) argument checking
(2) initializing
(3) build index from directory passed
(4) save index to file in appropriate format (word #docs DocID1 DocID1# DocID2 DocID2# etc.)
(5) clean up

if (argc == 5)

(6) read in .dat file
(7) recreate Index Dictionary
(8) save to new .dat file
(9) clean up

*****

*/
int main(int argc, char *argv[]) {
	if (argc == 3){
		printf("You entered: \n[TARGET_DIRECTORY] %s\n[RESULTS FILE NAME] %s\n\n", argv[1], argv[2]);
	}
	if (argc == 5){
		printf("You entered: \n[TARGET_DIRECTORY] %s\n[RESULTS FILE NAME] %s\n[RESULTS FILE NAME] %s\n[REWRITTEN FILE NAME] %s\n\n", argv[1], argv[2], argv[3], argv[4]);
	}
	if (argc != 3 && argc != 5) {
	  printf("Incorrect # of arguments. \nUsage: index [TARGET_DIRECTORY] [RESULTS FILE NAME]\n"\
	  	"Or: index [TARGET_DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]\n");
	  exit(1);
	}
	if (!isDirectory(argv[1])) {
	  printf("Please enter a valid directory path. Thanks!\n");
	  exit(1);
	}
	int len = strlen(argv[2]);
	if (len < 4 || argv[2][len-1] != 't' || argv[2][len-2] != 'a' || argv[2][len-3] != 'd' || argv[2][len-4] != '.'){
		printf("Please enter a file name of the format [fileName.dat]. Thanks!\n");
	 	exit(1);
	}
	if (argc == 5){
		//make sure results file names are the same
		if (strcmp(argv[2], argv[3]) != 0 ){
		printf("Please enter the same [RESULTS FILE NAME] in both instances.\n");
		exit(1);
		}

		//make sure newfilename ends in .dat
		len = strlen(argv[4]);
		if (len < 4 || argv[4][len-1] != 't' || argv[4][len-2] != 'a' || argv[4][len-3] != 'd' || argv[4][len-4] != '.'){
		printf("For the rewritten file name, please enter a file name of the format [fileName.dat]. Thanks!\n");
	 	exit(1);
		}

		//make sure results file name and the new file name are different
		if (argv[4] == argv[3]){
		printf("Please enter different file names for [RESULTS FILE NAME] and [REWRITTEN FILE NAME].\n");
		exit(1);
		}
	}

	//initialize stuff
	char *path = NormalizePath(argv[1]);
	char *saveFileName = argv[2];

	//build index from directory passed
	DICTIONARY *index = createIndex(path);
	if (index == NULL){
		perror("Error saving creating Index from Target Directory.\n");
		exit(1);
	}
	else{
		#ifdef DEBUG
		printf("Successfully created Index!\n");
		#endif
	}

	//save index to file
	int i = saveIndexToFile(index, saveFileName);
	if (i == 0){
		perror("Error saving Index to File.\n");
		exit(1);
	}
	else{
		#ifdef DEBUG
		printf("Successfully saved file to %s!\n", saveFileName);
		#endif
		freeIndex(index);
	}

	//sort index.dat file
	i = sortFile(saveFileName);
	if (i == 0){
		perror("Error sorting index file.");
		exit(1);
	}
	else{
		#ifdef DEBUG
		printf("Successfully sorted %s!\n", saveFileName);
		#endif
	}

	//if we're being asked to recreate the index and save it as a new file name..
	if (argc == 5){

		//recreate Index from file
		DICTIONARY *newIndex = recreateIndex(argv[3]);
		if (newIndex == NULL){
			perror("Error recreating index from file.");
			exit(1);
		}
		else {
			#ifdef DEBUG
			printf("Successfully recreated (new)Index!\n");
			#endif
		}

		//save new recreated index to file
		i = saveIndexToFile(newIndex, argv[4]);
		if (i == 0){
			perror("Error saving Index to File.\n");
			exit(1);
		}
		else{
			#ifdef DEBUG
			printf("Successfully saved recreated index to %s!\n", argv[4]);
			#endif
			freeIndex(newIndex);;
		}

		//sort index.dat file
		i = sortFile(argv[4]);
		if (i == 0){
			perror("Error sorting index file.");
			exit(1);
		}
		else{
			#ifdef DEBUG
			printf("Successfully sorted %s!\n", argv[4]);
			#endif
		}
	}

	return 0;
}




//filter for scandir (checks if 1st digit is a number)
//returns 1 for yes, 0 for non-decimal
int numFilter(const struct dirent *entry)
{
   return isdigit(entry->d_name[0]);
}

/*

createIndex
------------

Description: creates an index from the files found in the directory passed

Input: directory of files to index

Return: index dictionary

** Pseudo Code **

for each document in ../data/
(1) get the document's text (pageBuffer) and ID (docID)
(2) currentPosition = 0
(3) while (currentPosition = getNextWordFromHTMLDoc(pageBuffer, word, position))
		updateIndex(indexDict, word, docID)
(4) return indexDict


*****

*/
DICTIONARY *createIndex(char *path){

	/* get number of files and fileNames from directory */
	DICTIONARY *indexDict = makeDictionary();
	struct dirent **nameList;
	int n = scandir(path, &nameList, numFilter, alphasort);
    if (n < 0) {
        perror("Error scanning directory\n");
        return NULL;
    }
	
	/* For each file found in the directory.. */
	while (n--) {

		/*check for ., .., and temp.html
		if (strcmp(nameList[n]->d_name, ".") == 0 || strcmp(nameList[n]->d_name, "..") == 0 || strcmp(nameList[n]->d_name, "temp.html") == 0){
				#ifdef DEBUG
				printf("Throwing away docID: %s\n", nameList[n]->d_name);
				#endif
			free(nameList[n]);
			continue;
		}
		*/
		

		//get full name of file including path
		char tempFile[25];
    	memset(tempFile, 0, sizeof(tempFile));
		strcpy(tempFile, path);
    	strcat(tempFile, nameList[n]->d_name);
    	
    	if (isDirectory(tempFile)){
				#ifdef DEBUG
			    printf("%s is a directory. Throwing it away.\n", tempFile);
			    #endif
		    free(nameList[n]);
			continue;
		}
	  		#ifdef DEBUG
		    printf("Accessing new file: %s\n", tempFile);
		    #endif

		//read file into pageBuffer
		char *pageBuffer = readFileToBuffer(tempFile);
		if (pageBuffer == NULL) {
			free(nameList[n]);
			continue;
		}

		//set up for getNextWordFromHTMLDoc
		int currentPosition = 0;
		char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
		MALLOC_CHECK(word);
  		memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);

		//process pageBuffer, adding appropriate DNODEs and DocNodes into our index DICTIONARY
    	while ((currentPosition = getNextWordFromHTMLDoc(pageBuffer, word, currentPosition)) != 0){
        	int j = updateIndex(indexDict, word, atoi(nameList[n]->d_name));
        	if (j == 0){
        		perror("Error updating index.\n");
        	}
		}

		//cleanup
		free(pageBuffer);
		free(word);
        free(nameList[n]);
	}
	        
	//cleanup
	free(nameList);
		#ifdef DEBUG
		printDictionary3(indexDict);
		#endif
	return indexDict;
}

