#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../util/minunit.h"
#include "../util/header.h"
#include "../util/file.h"
#include "../util/hash.h"
#include "../util/dictionary.h"
#include "../indexer/buildIndex.h"

#define MAX_NUMBER_OF_KEYWORDS 1000

int tests_run = 0;
char * query;
DICTIONARY *myIndex;



//Testing getNextWord
static char *test_getNextWord(){
	
	printf("\n\nTesting getNextWord\n"); 

	//query
	query = "This and that and hi or bob       k yay cs50!\n";
	#ifdef DEBUG
		printf("Our query is: %s\n", query);
	#endif

	//setup
	int currentPosition = 0;
	char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
	MALLOC_CHECK(word);
	memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);

	while ( (currentPosition = getNextWord(query, word, currentPosition)) != 0) {
		printf("The next word is: %s\n", word);
	}
  
  	//cleanup
  	free(word);

	return 0;
}




//Testing getIntersectionOfLists 
static char *test_getIntersectionOfLists(){
	
	printf("\n\nTesting getIntersectionOfLists\n"); 

	//recreate Index from file
	char *indexFileName = "./indexer/index.dat";
	myIndex = recreateIndex(indexFileName);
	if (myIndex == NULL){
		perror("Error recreating index from file.");
		exit(1);
	}
	else {
		printf("Successfully recreated (new)Index!\n");
	}
	
	//setup
	DNODE *academia = getDNODEWithKey("academia", myIndex);
	DNODE *academic = getDNODEWithKey("academic", myIndex);

	//print individual
	DocNode *academiaData = academia->data;
	DocNode *academicData = academic->data;
	printf("List of academia docs:\n");
	while (academiaData != NULL) {
		printf("\tdocID: %d\tFreq: %d\n", academiaData->doc_id, academiaData->page_word_frequency); 
        academiaData = academiaData->next;
	}
	printf("List of academic docs:\n");
	while (academicData != NULL) {
		printf("\tdocID: %d\tFreq: %d\n", academicData->doc_id, academicData->page_word_frequency); 
        academicData = academicData->next;
	}

	//test
	DocNode *firstList = copyDocNodeList(academia->data);
	DocNode *secondList = copyDocNodeList(academic->data);
	DocNode *combined = getIntersectionOfLists(firstList, secondList);

	//print combined
	printf("List of intersecting docs:\n");
	while (combined != NULL) {
		printf("\tdocID: %d\tFreq: %d\n", combined->doc_id, combined->page_word_frequency); 
        combined = combined->next;
	}

	freeIndex(myIndex);
	return 0;
}

DocNode *test_sortDocNodesByFrequency(DocNode *firstDoc){

	printf("\n\nTesting sortDocNodesByFrequency\n");
	return sortDocNodesByFrequency(firstDoc);

}

static char *test_getANDList(){
	
	printf("\n\nTesting getANDList\n"); 


    //recreate Index from file
	char *indexFileName = "./indexer/index.dat";
	myIndex = recreateIndex(indexFileName);
	if (myIndex == NULL){
		perror("Error recreating index from file.");
		exit(1);
	}
	else {
		printf("Successfully recreated (new)Index!\n");
	}

	//setup
	printf("Query is %s\n", query);
	char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
	MALLOC_CHECK(word);
	memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
	int currentPosition = 0;
	//get number of keywords in query
	int numberOfKeywords = 0;
	while ((currentPosition = getNextWord(query, word, currentPosition)) != 0) {
		numberOfKeywords++;
	}
	printf("You entered %d keywords\n", numberOfKeywords);
	numberOfKeywords = numberOfKeywords + 2;
	free(word);

	//PARSE QUERY
		//returns an array of pointers to DNODEs.
		//the DNODE pointed to has the key AND or OR, depending on the list type and points to
		//a linked list of word DNODEs corresponding to the words in each list
	DNODE **parsedNodes = (DNODE **)malloc(sizeof(DNODE*)*(MAX_NUMBER_OF_KEYWORDS));
	memset(parsedNodes, 0, sizeof(DNODE*)*(MAX_NUMBER_OF_KEYWORDS));
	int success = parseQuery(query, parsedNodes, myIndex);		

	//print what we parsed
	if (success == 0) {
		printf("Error parsing query.\n");
	}
	else if ( success == -1 ) {
		//there was an AND keyword that wasn't in our dict. thus, return nothing
		printf("Your keyword search didn't match any results.\n");
	}	
	else {	
		//printing what we parsed

		printf("\nPrinting parsed query DNODE keys:\n");
		int counter = 0;
		while (parsedNodes[counter] != NULL){
			DNODE *n = parsedNodes[counter];
			printf("parsedNodes[%d]: \n", counter);
			while ( n != NULL) {
			    printf("Key: %s\n", n->key);
			    n = n->next;
			}

		    fputc('\n', stdout);
			counter++;
		}
	}

	//get ANDList
	DocNode *firstNodeOfANDList = getANDList(parsedNodes[0]);
	DocNode *m = firstNodeOfANDList;
	printf("Printing Doc_IDs from ANDList:\n");
	while ( m != NULL) {
	    printf("doc_id: %d\n", m->doc_id);
	    m = m->next;
	}


	//test sortDocNodesByFrequency
	firstNodeOfANDList = test_sortDocNodesByFrequency(firstNodeOfANDList);

	//print DocNodes, URLs, and frequencies
	m = firstNodeOfANDList;
	printf("Printing Doc_IDs, URLs, and frequencies from sorted ANDList:\n");
	char *path = "./data/";
	while ( m != NULL) {
	    printDocNode(m, path);
	    m = m->next;
	}

  	//cleanup
	freeDocNodes(firstNodeOfANDList);
  	freeParsedNodes(parsedNodes, numberOfKeywords);
  	free(parsedNodes);
  	freeIndex(myIndex);


	return 0;
}


static char *test_getORList(){
	
	printf("\n\nTesting getORList\n"); 


    //recreate Index from file
	char *indexFileName = "./indexer/index.dat";
	myIndex = recreateIndex(indexFileName);
	if (myIndex == NULL){
		perror("Error recreating index from file.");
		exit(1);
	}
	else {
		printf("Successfully recreated (new)Index!\n");
	}

	//setup
	printf("Query is %s\n", query);
	char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
	MALLOC_CHECK(word);
	memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
	int currentPosition = 0;
	//get number of keywords in query
	int numberOfKeywords = 0;
	while ((currentPosition = getNextWord(query, word, currentPosition)) != 0) {
		numberOfKeywords++;
	}
		printf("You entered %d keywords\n", numberOfKeywords);
	numberOfKeywords = numberOfKeywords + 2;
	free(word);

	//PARSE QUERY
		//returns an array of pointers to DNODEs.
		//the DNODE pointed to has the key AND or OR, depending on the list type and points to
		//a linked list of word DNODEs corresponding to the words in each list
	DNODE **parsedNodes = (DNODE **)malloc(sizeof(DNODE*)*(numberOfKeywords));
	memset(parsedNodes, 0, sizeof(DNODE*)*(numberOfKeywords));
	int success = parseQuery(query, parsedNodes, myIndex);		

	//print what we parsed
	if (success == 0) {
		perror("Error parsing Query.\n");

	}
	else if ( success == -1 ) {
		//there was an AND keyword that wasn't in our dict. thus, return nothing
		printf("Your keyword search didn't match any results.\n");
	}	
	else {	
		//printing what we parsed

		printf("\nPrinting parsed query DNODE keys:\n");
		int counter = 0;
		while (parsedNodes[counter] != NULL){
			DNODE *n = parsedNodes[counter];
			printf("parsedNodes[%d]: \n", counter);
			while ( n != NULL) {
			    printf("Key: %s\n", n->key);
			    n = n->next;
			}

		    fputc('\n', stdout);
			counter++;
		}
	}

	if (parsedNodes[1] != NULL) {
		DocNode *firstNodeOfORList = getORList(parsedNodes[1]);
		DocNode *m = firstNodeOfORList;
		printf("Printing Doc_IDs from ORList:\n");
		while ( m != NULL) {
		    printf("doc_id: %d\n", m->doc_id);
		    m = m->next;
		}
		freeDocNodes(firstNodeOfORList);
  	}

  	//cleanup
  	freeParsedNodes(parsedNodes, numberOfKeywords);
  	free(parsedNodes);
  	freeIndex(myIndex);


	return 0;
}


//Testing parseQuery
static char *test_parseQuery(){
	
	printf("\n\nTesting parseQuery\n"); 


    //recreate Index from file
	char *indexFileName = "./indexer/index.dat";
	myIndex = recreateIndex(indexFileName);
	if (myIndex == NULL){
		perror("Error recreating index from file.");
		exit(1);
	}
	else {
		printf("Successfully recreated (new)Index!\n");
	}

	//setup
	printf("Query is %s\n", query);
	char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
	MALLOC_CHECK(word);
	memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
	int currentPosition = 0;
	//get number of keywords in query
	int numberOfKeywords = 0;
	while ((currentPosition = getNextWord(query, word, currentPosition)) != 0) {
		numberOfKeywords++;
	}
	printf("You entered %d keywords\n", numberOfKeywords);
	free(word);

	//PARSE QUERY
		//returns an array of pointers to DNODEs.
		//the DNODE pointed to has the key AND or OR, depending on the list type and points to
		//a linked list of word DNODEs corresponding to the words in each list
	DNODE **parsedNodes = (DNODE **)malloc(sizeof(DNODE*)*(numberOfKeywords+2));
	memset(parsedNodes, 0, sizeof(DNODE*)*(numberOfKeywords+2));
	int success = parseQuery(query, parsedNodes, myIndex);					

	//print what we parsed
	if (success == 0) {
		perror("Error parsing Query.\n");

	}
	else if ( success == -1 ) {
		//there was an AND keyword that wasn't in our dict. thus, return nothing
		printf("Your keyword search didn't match any results.\n");
	}	
	else {	
		//printing what we parsed

		printf("\nPrinting parsed query DNODE keys:\n");
		int counter = 0;
		while (parsedNodes[counter] != NULL){
			DNODE *n = parsedNodes[counter];
			printf("parsedNodes[%d]: \n", counter);
			while ( n != NULL) {
			    printf("Key: %s\n", n->key);
			    n = n->next;
			}

		    fputc('\n', stdout);
			counter++;
		}


	}

  
  //cleanup
  freeParsedNodes(parsedNodes, numberOfKeywords);
  free(parsedNodes);
  freeIndex(myIndex);

  return 0;
}











static char *all_tests() {
	
    mu_run_test(test_getNextWord);

 

    query = "hi AND academy AND yes AND no";
    mu_run_test(test_parseQuery);
    query = "hi OR academy AND yes OR no";
    mu_run_test(test_parseQuery);
    query = " AND academy AND yes AND ";
    mu_run_test(test_parseQuery);
    query = "OR academy AND yes OR";
    mu_run_test(test_parseQuery);
    query = "word";
    mu_run_test(test_parseQuery);
    query = "and AND or";
    mu_run_test(test_parseQuery);
    query = "or";
    mu_run_test(test_parseQuery);
    query = "";
    mu_run_test(test_parseQuery);

    mu_run_test(test_getIntersectionOfLists);

    query = "academic AND academia academy OR yes series  ";
    mu_run_test(test_getANDList);
    query = "OR";
    mu_run_test(test_getANDList);
    query = "abstraction";
    mu_run_test(test_getANDList);

    query = "academic OR academia OR academy yes and series  ";
    mu_run_test(test_getORList);
    query = "or";
    mu_run_test(test_getORList);
    return 0;
}




int main() {
  char *result = all_tests();

  if (result != 0)
        printf("%s\n", result);
  else
      printf("\nALL UNIT TESTS PASSED\n");
  printf("Tests run: %d\n\n\n", tests_run);

  return result != 0;
}
