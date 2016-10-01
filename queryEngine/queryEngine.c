/*

  FILE: queryEngine.c

  Description: recreates an index from the index.dat file and returns ranked URLs to the user
  	in response to user-inputted keywords. Takes "AND" and "OR" commands, with "OR" taking
  	precedence

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "../util/dictionary.h"
#include "../util/file.h"
#include "../util/header.h"
#include "../util/hash.h"
#include "../util/html.h"
#include "../indexer/buildIndex.h"

#define MAX_NUMBER_OF_KEYWORDS 1000
//global variable for catching Ctrl + C
static int keepRunning = 1;

//function for catching Ctrl+C
void intHandler(int);

/*

MAIN QUERY ENGINE FUNCTION
------------

Description: 

Input: index.dat, target directory (./index.dat ../data/)

Return: ranked list of documents based on user's keyword searches

** Pseudo Code **

(1) argument checking
(2) initializing
(3) build index Dict from index.dat
(4) while (user has not hit Ctrl + C)
		get a query
		parse it into an array of pointers to DNODEs named parsedNodes
		parsedNodes[0] points to our ANDlist, and subsequent indices point to ORlists
		get the intersection of the ANDlist words and assign it to masterList
		for each ORlist
			get the union of the ORlist words
			get the intersection of masterList and the combined ORList
		sort the masterList by frequency
		print the masterList

*****

*/
int main(int argc, char *argv[]) {

	//arg checking													
	if (argc != 3) {
	  printf("Incorrect # of arguments. \nUsage: query [INDEX FILE NAME] [TARGET_DIRECTORY] \n");
	  exit(1);
	}
	/* check if readable file exists */
	if( access( argv[1], R_OK ) == -1 ) {
	  printf("%s is not a readable file.\n", argv[1]);
	  exit(1);
	}
	if (!isDirectory(argv[2])) {
	  printf("Please enter a valid directory path. Thanks!\n");
	  exit(1);
	}

	//initialization of stuff											
	char *indexFileName = argv[1];
	char *path = NormalizePath(argv[2]);
	int currentPosition = 0;
	DNODE **parsedNodes = (DNODE **)malloc(sizeof(DNODE*)*(MAX_NUMBER_OF_KEYWORDS));
	memset(parsedNodes, 0, sizeof(DNODE*)*(MAX_NUMBER_OF_KEYWORDS));

	//recreate Index from file
	DICTIONARY *index = recreateIndex(indexFileName);
	if (index == NULL){
		perror("Error recreating index from file.");
		exit(1);
	}
	else {
		#ifdef DEBUG
		printf("Successfully recreated (new)Index!\n");
		#endif
	}

	//for catching Ctrl + C
	signal(SIGINT, intHandler);

	//keep getting queries until user forcibly quits
	while( keepRunning ) {

		printf( "\nEnter query:");
		  
		//get query from user
		char * query = NULL;
		size_t len = 0;
		ssize_t read;
		char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
		MALLOC_CHECK(word);
		memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
		int parsedNodeCounter = 1; //parsedNode iterator

		read = getline(&query, &len, stdin);
		if (keepRunning){

			if (read == -1 || (strcmp(query, "\n") == 0)){
				printf( "\nYou did not enter a query.\n");
				free(word);
				free(query);
				continue;
			}
			else {
				printf("\nYou entered %s\n\n", query);
			}

	 		//get number of keywords in query
			int numberOfKeywords = 0;
			while ((currentPosition = getNextWord(query, word, currentPosition)) != 0) {
				numberOfKeywords++;
			}
			numberOfKeywords = numberOfKeywords + 2;
			free(word);

			//PARSE QUERY
				//returns an array of pointers to DNODEs.
				//the DNODE pointed to has the key AND or OR, depending on the list type and points to
				//a linked list of word DNODEs corresponding to the words in each list
			
			memset(parsedNodes, 0, sizeof(DNODE*)*(MAX_NUMBER_OF_KEYWORDS));
			int success = parseQuery(query, parsedNodes, index);										
			if (success == 0) {
				perror("Error parsing Query.\n");														
				exit(EXIT_FAILURE);
			}
			else if ( success == -1 ) {
				//there was an AND keyword that wasn't in our dict. thus, return nothing
				printf("Your keyword search didn't match any results.\n");
				freeParsedNodes(parsedNodes, numberOfKeywords);
				free(query);
				continue;
			}	
			else {							

				//get the AND list
				DocNode *firstDocInMasterList = getANDList(parsedNodes[0]);									

				//what if there's nothing in the ANDList?
				if (firstDocInMasterList == NULL) { //if no ANDs, make the initial list the first OR list
					firstDocInMasterList = getORList(parsedNodes[1]);
					parsedNodeCounter++;
				}

				//what if there's nothing in the ORList either?
				if (firstDocInMasterList == NULL) {
					printf("Your keyword search didn't match any results.\n");
					freeParsedNodes(parsedNodes, numberOfKeywords);
					free(query);
					continue;
				}

				//keep getting OR lists and match them with the master list
				while (parsedNodes[parsedNodeCounter] != NULL && parsedNodeCounter < numberOfKeywords) {

					DocNode *nextORlist = getORList(parsedNodes[parsedNodeCounter]);
					firstDocInMasterList = getIntersectionOfLists(firstDocInMasterList, nextORlist);	
					if (firstDocInMasterList == NULL){
						perror("Failed to add an OR List to the Master List.\n");
						exit(EXIT_FAILURE);
					}
					parsedNodeCounter++;
				}

				//sort the list of DocNodes by frequency
				firstDocInMasterList = sortDocNodesByFrequency(firstDocInMasterList);
				if (firstDocInMasterList == NULL){
					perror("Failed to sort Master List.\n");
					exit(EXIT_FAILURE);
				}

				//print each DocNode's URL and frequency for the user
				DocNode *currentDocNode = firstDocInMasterList;
				while (currentDocNode != NULL) {

					success = printDocNode(currentDocNode, path);
					if (success == 0){
						perror("Failed to print a doc node URL.\n");
					}
					currentDocNode = currentDocNode->next;
				}
				freeDocNodes(firstDocInMasterList);

			}
			//cleanup
			freeParsedNodes(parsedNodes, numberOfKeywords);
			free(query);

		}
		else {
			free(word);
			free(query);
		}
	}


	//cleanup
	free(parsedNodes);
	freeIndex(index);

}

//function for catching Ctrl+C
void intHandler(int sig) {
    keepRunning = 0;
}


