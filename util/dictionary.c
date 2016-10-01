
/*

  FILE: dictionary.c

  Description: dictionary data structure

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../crawler/crawler.h"
#include "dictionary.h"
#include "../crawler/list.h"
#include "header.h"
#include "file.h"
#include "hash.h"
#include "../indexer/buildIndex.h"


/*

*makeDictionary*
------------

Description: mallocs and memsets a new Dictionary - returns pointer to it

Input: n/a 

Return: pointer to new DICTIONARY

** Pseudo Code **

(1) malloc new DICTIONARY
(2) memset everything

*****

*/
DICTIONARY *makeDictionary(){

  //make dictionary
  DICTIONARY *dict = malloc(sizeof(DICTIONARY));
  MALLOC_CHECK(dict);
  memset(dict, 0, sizeof(DICTIONARY));

  //set everything equal to null
  memset(dict->hash, 0, (sizeof(DNODE*)*MAX_HASH_SLOT));
  dict->start = NULL;
  dict->end = NULL;             

  return dict;
}

/*

*getAddressFromLinksToBeVisited*
------------

Description: scan dictionary for next unvisited URL and return pointer to it;
  updates currentDepth with depth of returned URL's depth

Input: dictionary, pointer to currentDepth

Return: pointer to next unvisited URL in dictionary

** Pseudo Code **

(1) if there's nothingin the dictionary, return NULL
(2) iterate through DNODEs, starting from dict->start, until reach unvisited URL
  return a pointer to the unvisited URL
(3) if reach the end of the dictionary, return NULL

*****

*/
char *getAddressFromLinksToBeVisited(DICTIONARY* dict,int *currentDepth) {

  //if there is nothing in the dictionary..
  if(dict->start == NULL) {
    #ifdef DEBUG
    printf("There are no more links to be visited.\n");
    #endif
    return NULL;
  }
  else {
  
    DNODE *nodeToCheck = dict->start;
    while ( nodeToCheck != NULL ) {
      //check if nodeToCheck's url has been visited. If not, update depth and return url
      if ( ((URLNODE*)nodeToCheck->data)->visited == 0 ){
        *currentDepth = ((URLNODE*)nodeToCheck->data)->depth;
        return ((URLNODE*)nodeToCheck->data)->url;
      }

      //if we've reached the end of the dictionary..
      if( nodeToCheck == dict->end ) {
        #ifdef DEBUG
        printf("There are no more links to be visited.\n");
        #endif
        return NULL;
      }
      
      //go to next DNODE
      nodeToCheck = nodeToCheck->next;
    }
  }
  return NULL;
}

/*

*addElementToDictionary*
------------

Description: adds data to dictionary if it's unique (creates new DNODE
  with appropriate data) at appropriate cluster

Input: dictionary, url to add, depth of url to add

Return: n/a

** Pseudo Code **

(1) get hash value
(2) if there's nothing in the dictionary, add first url as first element
(3) if something exists in the dictionary but not at the hash value,
  add the url to the end of the list and point dict[hashValue] at it
(4) if something already exists at the hash value index, go through each 
  existing element with the same hash value until:
    (a) we find a duplicate - do nothing
    (b) we reach the end of the dictionary - it's unique - add to end of list
    (c) we reach the end of the cluster of that hash value - add to the end of the cluster

*****

*/
DNODE *addElementToDictionary(DICTIONARY* dict, char* key, void* data){

  //get hash value
  unsigned long hashValue = hash1(key)%(unsigned long)MAX_HASH_SLOT;

  //two cases. Either there's something in the dictionary already, or not

  //if there's nothing in the dictionary..
  if (dict->start == NULL) {

    //add a new DNODE
    dict->hash[hashValue] = malloc(sizeof(DNODE));
    MALLOC_CHECK(dict->hash[hashValue]);
    memset(dict->hash[hashValue], 0, sizeof(DNODE));

    //update start and end pointers
    dict->start = dict->hash[hashValue];
    dict->end = dict->hash[hashValue];

    //set the new DNODE's key 
    strncpy(dict->hash[hashValue]->key, key, KEY_LENGTH);

    //set DNODE's docCount
    dict->hash[hashValue]->docCount = 1;

    //set the new DNODE's data
    dict->hash[hashValue]->data = data; 
  }
  //if there's stuff in the dictionary already..
  else {   

    //if there's nothing at the hash index..
    if ( dict->hash[hashValue] == NULL ) {

      //add a new DNODE..
      dict->hash[hashValue] = malloc(sizeof(DNODE));
      MALLOC_CHECK(dict->hash[hashValue]);
      memset(dict->hash[hashValue], 0, sizeof(DNODE));

      //..to the end
      dict->hash[hashValue]->prev = dict->end;
      dict->end->next = dict->hash[hashValue];
      dict->end = dict->hash[hashValue];
      
      //set the new DNODE's key 
      strncpy(dict->hash[hashValue]->key, key, KEY_LENGTH);

      //set DNODE's docCount
      dict->hash[hashValue]->docCount = 1;

      //set the new DNODE's data
      dict->hash[hashValue]->data = data;
    }

    // there's at least one DNODE with the same hash value...
    else {
      DNODE *nodeToCheck = dict->hash[hashValue];
      
      //For each DNODE with the same hashValue..
      while ( hashValue == (hash1(nodeToCheck->key)%MAX_HASH_SLOT)){

        //if this is a duplicate..
        if (strcmp(nodeToCheck->key, key) == 0){
          return nodeToCheck;
        }

        //if we've reached the end of the dictionary..
        if ( nodeToCheck == dict->end ) {
          //This key is unique. Add it to the end
          dict->end->next = malloc(sizeof(DNODE));
          MALLOC_CHECK(dict->end->next);
          memset(dict->end->next, 0, sizeof(DNODE));

          //set pointers
          dict->end->next->prev = dict->end;
          dict->end = dict->end->next;
          
          //set the new DNODE's key 
          strncpy(dict->end->key, key, KEY_LENGTH);

          //set DNODE's docCount
          dict->end->docCount = 1;
      
          //set the new DNODE's data
          dict->end->data = data;

          //we're done
          break;
        }
        
        //if we've reached the end of the cluster..
        if (hashValue != (hash1(nodeToCheck->next->key)%MAX_HASH_SLOT)){
          //This key is unique. Insert it at the end of the cluster
          DNODE *temp = nodeToCheck->next;
          nodeToCheck->next = malloc(sizeof(DNODE));
          MALLOC_CHECK(nodeToCheck->next);
          memset(nodeToCheck->next, 0, sizeof(DNODE));

          //set pointer
          nodeToCheck->next->prev = nodeToCheck;
          nodeToCheck->next->next = temp;
          temp->prev = nodeToCheck->next;

          //set the new DNODE's key 
          strncpy(nodeToCheck->next->key, key, KEY_LENGTH);

          //set DNODE's docCount
          nodeToCheck->next->docCount = 1;

          //set the new DNODE's data
          nodeToCheck->next->data = data;

          //we're done
          break;
        }

        //go to next DNODE
        nodeToCheck = nodeToCheck->next;
        
      }
    }
  }
  return NULL;
}


/*

*printDictionary*
------------

Description: prints each element in the dictionary (url, depth, visited status) to 
  the file logOutput

Input: dictionary

Return: n/a

** Pseudo Code **

(1) iterate through dictionary and print each element

*****

*/
void printDictionary(DICTIONARY* dict){
  printf("Printing Dictionary To File\n");

  /*open new file with name logOutput and append printDictionary*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "Printing Dictionary.\n");

  DNODE *n = dict->start;
  while ( n != NULL) {
    fprintf(newFile, "URL: %s\tDepth: %d\tVisited: %d\n", ((URLNODE*)n->data)->url,
      ((URLNODE*)n->data)->depth, ((URLNODE*)n->data)->visited );
    fputc('\n', newFile);
    n = n->next;
  }
  fclose(newFile);
}


/*

*printDictionary2*
------------

Description: prints each element in the dictionary (url, depth, visited status, prev, next) as
  well as the dictionary's pointers to start, end, and each hash index to the file logOutput

Input: dictionary

Return: n/a

** Pseudo Code **

(1) iterate through dictionary and print each element

*****

*/
void printDictionary2(DICTIONARY* dict){
  
  /*open new file with name logOutput and append printDictionary*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  
  fprintf(newFile, "\n\nPrinting Dictionary\n");
  if( dict->start != NULL ) 
    fprintf(newFile, "Start: %s\n", ((URLNODE*)dict->start->data)->url);
  else
    fprintf(newFile, "\tStart: NULL\n");
  if( dict->end != NULL ) 
    fprintf(newFile, "End: %s\n", ((URLNODE*)dict->end->data)->url);
  else
    fprintf(newFile, "\tEnd: NULL\n");
  unsigned int i;
  for (i= 0; i < MAX_HASH_SLOT; i++) {
    if (dict->hash[i] != NULL) {
      fprintf(newFile, "Hash[%d]: %s\n", i, ((URLNODE*)dict->hash[i]->data)->url);
    }
    else
      fprintf(newFile, "Hash[%d]: NULL\n", i);
  }
  DNODE *n = dict->start;
  while ( n != NULL) {
      fprintf(newFile, "\nURL: %s\tHash: %lu\tDepth: %d\tVisited: %d\n", ((URLNODE*)n->data)->url,
      (hash1(n->key)%MAX_HASH_SLOT), ((URLNODE*)n->data)->depth,
      ((URLNODE*)n->data)->visited ); 
    if( n->prev != NULL ) 
      fprintf(newFile, "\tPrev: %s\n", ((URLNODE*)n->prev->data)->url);
    else
      fprintf(newFile, "\tPrev: NULL\n");
    if( n->next != NULL ) 
      fprintf(newFile, "\tNext: %s\n", ((URLNODE*)n->next->data)->url);
    else
      fprintf(newFile, "\tNext: NULL\n");
    n = n->next;
  }
  fclose(newFile);
}

/*

*printDictionary3*
------------

Description: prints index dictionary

Input: dictionary

Return: n/a

** Pseudo Code **

(1) iterate through dictionary and print each element

*****

*/
void printDictionary3(DICTIONARY* dict){
  
  /*open new file with name logOutput and append printDictionary*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  
  fprintf(newFile, "\n\nPrinting Dictionary\n");
  if( dict->start != NULL ) 
    fprintf(newFile, "Start: %s\n", dict->start->key);
  else
    fprintf(newFile, "\tStart: NULL\n");
  if( dict->end != NULL ) 
    fprintf(newFile, "End: %s\n", dict->end->key);
  else
    fprintf(newFile, "\tEnd: NULL\n");
  unsigned int i;
  for (i= 0; i < MAX_HASH_SLOT; i++) {
    if (dict->hash[i] != NULL) {
      fprintf(newFile, "Hash[%d]: %s\n", i, dict->hash[i]->key);
    }
  }

  DNODE *n = dict->start;
  while ( n != NULL) {
      fprintf(newFile, "%s\tHash: %lu\n", n->key, (hash1(n->key)%MAX_HASH_SLOT));
      DocNode *doc = (DocNode *)n->data;
      while (doc != NULL){
        fprintf(newFile, "\tdocID: %d\tFreq: %d\n", doc->doc_id, doc->page_word_frequency); 
        doc = doc->next;
      }
      n = n->next;
  }
  fclose(newFile);
}



/*

*freeDictionary*
------------

Description: frees each element of passed dictionary

Input: dictionary

Return: n/a

** Pseudo Code **

(1) iterate through dictionary and free each DNODE and each DNODE's data

*****

*/
void freeDictionary(DICTIONARY* dict){

  DNODE *n = dict->start;
  DNODE *m;
  while ( n != NULL) {
    free(n->data); //free URL Node
    m = n;
    n = n->next; //get next node
    free(m); //free current node
  }
  free(dict);
}


/*

*freeDocNodes*
------------

Description: frees each DocNode in linked list

Input: pointer to first DocNode

Return: 1 for success

** Pseudo Code **

(1) iterate through DocNodes, freeing each one

*****

*/
void freeDocNodes(DocNode* nodeToCheck){

  DocNode *temp;
  while ( nodeToCheck != NULL) {
    temp = nodeToCheck;
    nodeToCheck = nodeToCheck->next; //get next node
    free(temp); //free current node
  }
}


/*

*freeDNODEs*
------------

Description: frees each DNODE in linked list

Input: pointer to first DNODE

** Pseudo Code **

(1) iterate through DNODEs, freeing each one

*****

*/
void freeDNODEs(DNODE* nodeToCheck){

  DNODE *temp;
  while ( nodeToCheck != NULL) {
    temp = nodeToCheck;
    nodeToCheck = nodeToCheck->next; //get next node
    free(temp); //free current node
  }
}


/*

*freeIndex*
------------

Description: frees each element of passed index

Input: dictionary

Return: n/a

** Pseudo Code **

(1) iterate through dictionary and free each DNODE and each DNODE's data

*****

*/
void freeIndex(DICTIONARY* dict){

  DNODE *n = dict->start;
  DNODE *m;
  while ( n != NULL) {
    freeDocNodes((DocNode *)n->data); //free Doc Nodes
    m = n;
    n = n->next; //get next DNODE
    free(m); //free current DNODE
  }
  free(dict);
}

/*

updateIndex
------------

Description: adds the passed word to the index in appropriate manner

Input: word to add, DocID in which word was found, dictionary to add to

Return: 1 if successful, 0 for failure

** Pseudo Code **

(1) create DocNode with passed docID and word frequency 0
(2) add word as DNODE to DICTIONARY *indexDict; data is new DocNode
  if the word is not already in indexDict, adds it with 'word' as key
  if the word is already in indexDict (it's a duplicate)
    if the DocNode already exists
      increment its frequency
    if the DocNode doesn't exist yet, add it to the end of the list


*****

*/
int updateIndex(DICTIONARY *indexDict, char *word, int docID){

  DocNode *newDocNode = makeDocNode(docID);
  DNODE *duplicate = addElementToDictionary(indexDict, word, newDocNode);
  //if this is a duplicate Word DNODE..
  if (duplicate != NULL) {
    int i = addDuplicateWord(duplicate, newDocNode);
    if (i == 0){
      perror("Error adding duplicate.\n");
      return 0;
    }
  }
  return 1;
}

/*

*makeDocNode*
------------

Description: mallocs and memsets a new DocNode - returns pointer to it

Input: word, docID

Return: pointer to new DocNode

** Pseudo Code **

(1) malloc new DocNode
(2) memset everything

*****

*/
DocNode *makeDocNode(int docID){

  DocNode *docnode = malloc(sizeof(DocNode));
  MALLOC_CHECK(docnode);
  memset(docnode, 0, sizeof(DocNode));

  docnode->doc_id = docID;
  docnode->page_word_frequency = 1;
  docnode->next = NULL;

  return docnode;
}

/*

*addDuplicateWord*
------------

Description: if the word we're adding is a duplicate, updateIndex calls this function. 
  Adds DocNode if doc_id not already present - otherwise, increments frequency

Input: DNODE and DocNode in question

Return: 1 if successful, 0 for failure

** Pseudo Code **

    runs through the list of DocNodes the DNODE points to
      if we find a doc_id matching the DocNode's
        increment frequency of word for that DocNode
        return
    //the docnode (and docID) in question wasn't found in the list.
    runs through the list of DocNodes the DNODE points to
      if we reach a docID larger than docnode's
        add docnode just before DocNode
      if we reach the end of the list.. 
        add to end of list

*****

*/
int addDuplicateWord(DNODE *wordNode, DocNode *docNode){
  DocNode *nodeToCheck = (DocNode *)wordNode->data;

  //runs through DocNodes to see if the DocNode is already there
    while ( nodeToCheck != NULL ){

      //DocNode found. increment freq
        if (nodeToCheck->doc_id == docNode->doc_id){
          nodeToCheck->page_word_frequency++;
          free(docNode);
          return 1;
        }

        //go to next DocNode
        nodeToCheck = nodeToCheck->next;
  }

  //the docnode (and docID) in question is unique. Add it to appropriate position
    nodeToCheck = (DocNode *)wordNode->data; //go back to beginning of list
    DocNode *previous = (DocNode *)wordNode->data;
    while ( nodeToCheck != NULL ){

        //if we've reached a docID greater than ours
        if (nodeToCheck->doc_id > docNode->doc_id){

          //we're at the first element...
          if (nodeToCheck == previous){
            docNode->next = nodeToCheck;
            wordNode->data = docNode;
            wordNode->docCount++;
            return 1;
          }
          //if we're further down the list..
          else {
            docNode->next = nodeToCheck;
            previous->next = docNode;
            wordNode->docCount++;
            return 1;
          }
        }

        //if we've reached the end of the list, add docnode to the end
        if(nodeToCheck->next == NULL) {
          nodeToCheck->next = docNode;
          wordNode->docCount++;
          return 1;
        }

        //go to next DocNode
        if (nodeToCheck == previous){ //if we're still at the beginning of the list
          nodeToCheck = nodeToCheck->next;
        }
        else {
          nodeToCheck = nodeToCheck->next;
          previous = previous->next;
        }
    }

  return 0;
}




/*

*saveIndexToFile*
--------------

Description: saves the index dictionary structure as a .dat file in the current directory

Input: index to save in 'saveFileName'

Output: 1 for success, 0 otherwise

*****
*/
int saveIndexToFile(DICTIONARY *index, char *saveFileName){

  FILE *newF = fopen(saveFileName, "w");
  if (newF == NULL){
    return 0;
  }

  DNODE *n = index->start;
  while ( n != NULL) {

      //print word and # of docs
      fprintf(newF, "%s %d ", n->key, n->docCount);
      DocNode *doc = (DocNode *)n->data;

      //for each doc, print docID and wordFrequency
      while (doc != NULL){
        fprintf(newF, "%d %d ", doc->doc_id, doc->page_word_frequency); 
        doc = doc->next;
      }
      n = n->next;
      fprintf(newF, "\n");
  }

  fclose(newF);
  return 1;
}

/*

*getLastDNODEInList*
--------------

Description: given a DNODE, finds the last DNODE in the list and returns it

Input: start of DNODE list

Output: pointer to last DNODE in list

*****
*/
DNODE *getLastDNODEInList(DNODE *start){
  while (start->next != NULL)
    start = start->next;

return start;
}


/*

*getDNODEWithKey*
--------------

Description: given a key, return pointer to DNODE(Word Node) with the key passed
  or NULL if not found

Input: key to find and dictionary to find it in

Output: pointer to DNODE with key passed, or NULL if not found

*****
*/
//returns pointer to DNODE (Word Node) with the key passed, or NULL if not found
DNODE *getDNODEWithKey(char *key, DICTIONARY *dict){

  //get hash value
  unsigned long hashValue = hash1(key)%(unsigned long)MAX_HASH_SLOT;

  //if there's nothing in the dictionary..
  if (dict->start == NULL) {
    return NULL;
  }
  //if there's stuff in the dictionary already..
  else {   

    //if there's nothing at the hash index..
    if ( dict->hash[hashValue] == NULL ) {
      return NULL;
    }

    // there's at least one DNODE with the same hash value...
    else {
      DNODE *nodeToCheck = dict->hash[hashValue];
      
      //For each DNODE with the same hashValue..
      while ( hashValue == (hash1(nodeToCheck->key)%MAX_HASH_SLOT)){

        //if we've found it..
        if (strcmp(nodeToCheck->key, key) == 0){
          return nodeToCheck;
        }

        //if we've reached the end of the dictionary..
        if ( nodeToCheck == dict->end ) {
          return NULL;
        }
        
        //if we've reached the end of the cluster..
        if (hashValue != (hash1(nodeToCheck->next->key)%MAX_HASH_SLOT)){
          return NULL;
        }

        //go to next DNODE
        nodeToCheck = nodeToCheck->next;
        
      }
    }
  }
  return NULL;
}





//copies the passed DNODE (and its pointer to DocNode list) and returns a pointer to the new DNODE
DNODE *copyDNODE(DNODE* toCopy){


  DNODE *newDNODE = malloc(sizeof(DNODE));
  MALLOC_CHECK(newDNODE);
  memset(newDNODE, 0, sizeof(DNODE));

  if (toCopy == NULL) {
    return NULL;
  }
  else {
    //copy everything
    newDNODE->prev = NULL;
    newDNODE->next = NULL;
    newDNODE->data = toCopy->data;
    strncpy(newDNODE->key, toCopy->key, KEY_LENGTH);
    newDNODE->docCount = toCopy->docCount;
  }
  return newDNODE;

}



//copies the passed DocNode (and its doc_id, frequency, and next pointer) and returns a pointer to the new DocNode
DocNode *copyDocNode(DocNode* toCopy){


  DocNode *newDocNode = malloc(sizeof(DocNode));
  MALLOC_CHECK(newDocNode);
  memset(newDocNode, 0, sizeof(DocNode));

  if (toCopy == NULL) {
    return NULL;
  }
  else {
    //copy everything
    newDocNode->next = toCopy->next;
    newDocNode->doc_id = toCopy->doc_id;
    newDocNode->page_word_frequency = toCopy->page_word_frequency;
  }
  return newDocNode;

}





/*

recreateIndex
------------

Description: recreates an index from the file passed (probably index.dat)

Input: name of file to recreate index from

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
DICTIONARY *recreateIndex(char *fileName){

  /* open index.dat to read, initialize new index */
    FILE *oldFile = fopen(fileName,"r");
    if (oldFile == NULL){
      perror("Failed to open file in order to recreate index.\n");
      return NULL;
    }
  DICTIONARY *newIndex = makeDictionary();

  //read in lines continuously until EOF
  char * currentLine = NULL;
  size_t len = 0;
  ssize_t read;
  char *word;
  while ((read = getline(&currentLine, &len, oldFile)) != -1) {

    //get the keyword
    word = strtok(currentLine, " ");
    if (word == NULL){
      continue;
    }

    //throw away docCount and get initial docID and freq
    char *docID, *freq;
    docID = strtok(NULL, " ");
    docID = strtok(NULL, " ");
    freq = strtok(NULL, " ");

    //add each DocNode 
    while (docID != NULL && freq != NULL){

      //make new DocNode and add to index
      DocNode *newDocNode = makeDocNode(atoi(docID));
      newDocNode->page_word_frequency = atoi(freq);
      DNODE *duplicate = addElementToDictionary(newIndex, word, newDocNode);
      if (duplicate != NULL) {
        int i = addDuplicateWord(duplicate, newDocNode);
        if (i == 0) {
          perror("Error adding DocNode during recreateIndex.\n");
          if(currentLine)
            free(currentLine);
          return NULL;
        }
      }

      //get the next docID and freq
      docID = strtok(NULL, " ");
      freq = strtok(NULL, " ");
    }

  }
  if (currentLine)
    free(currentLine);
  fclose(oldFile);
  return newIndex;
}








/*

*parseQuery*
--------------

Description: parses user-inputted query and returns an array of poiners to DNODEs. Each
  DNODE pointed to has the key AND or OR, depending on the list type, and points to
  a linked list of word DNODEs corresponding to the words in each list

Input: query to parse, parsedNodes array to store pointers in, index dictionary

Output: 1 for success, -1 for finding a word in AND list not in index, 0 for failure

*****
*/
int parseQuery(char *query, DNODE** parsedNodes, DICTIONARY *index){

  if (query[0] == 0){
    //no keywords
    return -1;
  }

  //setup
  int currentPosition = 0;
  char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
  MALLOC_CHECK(word);
  memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
  char * prevWord = malloc(sizeof(char)*MAX_WORD_LENGTH);
  MALLOC_CHECK(prevWord);
  memset(prevWord, 0, sizeof(char)*MAX_WORD_LENGTH);

  int operator = 0; //0 for and, 1 for OR
  int ORcounter = 1;
  int lastWordWasOR = 0; //0 for no, 1 for yes
  char *and = "AND";
  char *or = "OR";
  int oneMoreLoop = 1;
  int lastKnownPosition;


  //create beginning of AND list
  parsedNodes[0] = malloc(sizeof(DNODE));
  MALLOC_CHECK(parsedNodes[0]);
  memset(parsedNodes[0], 0, sizeof(DNODE));
  strcpy(parsedNodes[0]->key, and);


  while ((currentPosition = getNextWord(query, word, currentPosition)) != 0 || oneMoreLoop != 0){

    //we need the loop to run through one more time after returning a currentPosition of 0
    if (currentPosition != 0)
      lastKnownPosition = currentPosition;
    else {
      //for the last loop
      oneMoreLoop = 0;
      currentPosition = lastKnownPosition;
    }

    //if it's an AND, throw it away
    if (strcmp(word, and) == 0 && strlen(word) == 3){
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      continue;
    }

    //if it's an OR, set operator to OR and lastWordWasOR to yes, and throw away
    if (strcmp(word, or) == 0 && strlen(word) == 2){
      operator = 1;
      lastWordWasOR = 1;
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      continue;
    }

    //if this is the first word..
    if (prevWord[0] == '\0' && operator == 0) {
      strncpy(prevWord, word, MAX_WORD_LENGTH);
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      continue;
    }

    //if it's another word and operator == and, add prevWord to AND list
    if (operator == 0){
      //get the corresponding DNODE from our index Dict
      DNODE *currentDNODE = getDNODEWithKey(prevWord, index);

      //if we can't find the word in our index..
      if (currentDNODE == NULL) {
        //no results will match.
        free(word);
        free(prevWord);
        return -1;
      }
      //otherwise, we found the corresponding DNODE
      else {
        //add a copy of it to the end of the ANDlist
        DNODE *lastDNODEinANDList = getLastDNODEInList(parsedNodes[0]);
        lastDNODEinANDList->next = copyDNODE(currentDNODE);
      }

      //put current word in prevWord, and clear current word for getNextWord
      memset(prevWord, 0, sizeof(char)*MAX_WORD_LENGTH);
      strncpy(prevWord, word, MAX_WORD_LENGTH);
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      continue;
    }

    //if it's another word and operator == OR, add prevWord to current OR list
    //if the last word was OR, then operator still == OR
    //if the last word was not OR, then we're at the end of a sequence of ORs. change operator to == AND
    //and increment ORcounter
    if (operator == 1) {

      //get the corresponding DNODE from our index Dict and add to parsedNodes
      DNODE *currentDNODE = getDNODEWithKey(prevWord, index);
      //if we can't find the word in our index, it doesn't matter.
      if (currentDNODE == NULL) {
        //throw it away
      }
      else {
        //add it to the end of the current OR list
        if (parsedNodes[ORcounter] == NULL) {  //if this is the first element in the list..

          //create beginning of OR list
          parsedNodes[ORcounter] = malloc(sizeof(DNODE));
          MALLOC_CHECK(parsedNodes[ORcounter]);
          memset(parsedNodes[ORcounter], 0, sizeof(DNODE));
          strcpy(parsedNodes[ORcounter]->key, or);

          //add prevWord to end of list
          DNODE *lastDNODEinORList = getLastDNODEInList(parsedNodes[ORcounter]);
          lastDNODEinORList->next = copyDNODE(currentDNODE);

        }
        else {
          //add prevWord to end of list
          DNODE *lastDNODEinORList = getLastDNODEInList(parsedNodes[ORcounter]);
          lastDNODEinORList->next = copyDNODE(currentDNODE);
        }
      }

      //if the last word was OR, then operator still == OR
      if (lastWordWasOR == 1) {
        operator = 1;
        lastWordWasOR = 0;
      }
      //else we're at the end of a sequence of ORs. change operator to == AND, and increment ORcounter
      else {
        operator = 0;
        lastWordWasOR = 0;
        ORcounter++;
      }

      //put current word in prevWord, and clear current word for getNextWord
      memset(prevWord, 0, sizeof(char)*MAX_WORD_LENGTH);
      strncpy(prevWord, word, MAX_WORD_LENGTH);
      memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
      continue;
    }


    printf("Should not reach here. Current word is %s.\n", word);
    return 0;
    memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);
  }//end of while loop





  free(word);
  free(prevWord);
  return 1;
}



/*

*freeParsedNodes*
--------------

Description: frees each DNODE in parsedNodes

Input: pointer to array of pointers to DNODEs

Output: n/a

*****
*/
void freeParsedNodes(DNODE** parsedNodes, int numberOfKeywords){

  int counter;
  for (counter = 0; counter < numberOfKeywords; counter++){
    DNODE *n = parsedNodes[counter];
    DNODE *m;
    while ( n != NULL) {
      m = n;
      n = n->next; //get next DNODE
      free(m); //free current DNODE
    }
  }

}


//copy list of DocNodes and return pointer to first DocNode
DocNode *copyDocNodeList(DocNode *toCopy){
  //create the first copied DocNode
  DocNode *firstNode = malloc(sizeof(DocNode));
  MALLOC_CHECK(firstNode);
  memset(firstNode , 0, sizeof(DocNode));

  //copy doc_id and  frequency
  firstNode->next = NULL;
  firstNode->doc_id = toCopy->doc_id;
  firstNode->page_word_frequency = toCopy->page_word_frequency;

  //iterate through rest of DocNodes
  DocNode *currentNode = firstNode;
  while (toCopy->next != NULL){
    currentNode->next= malloc(sizeof(DocNode));
    MALLOC_CHECK(currentNode->next);
    memset(currentNode->next , 0, sizeof(DocNode));

    //copy
    currentNode->next->next = NULL;
    currentNode->next->doc_id = toCopy->next->doc_id;
    currentNode->next->page_word_frequency = toCopy->next->page_word_frequency;

    //iterate
    currentNode = currentNode->next;
    toCopy = toCopy->next;
  }

  return firstNode;
}



/*

*getIntersectionOfLists*
--------------

Description: looks for the intersection of two lists of DocNodes, adding frequencies of matches
  returns a pointer to the first match in the list (list returned is copied - not original from indexDict)

Input: pointers to the first DocNodes of the two lists to match

Output: pointer to first match in combined list, or NULL for failure

*****
*/
DocNode *getIntersectionOfLists(DocNode *list1, DocNode *list2){

  if (list1 == NULL || list2 == NULL) {
    if (list1 != NULL) {
      freeDocNodes(list1);
    }
    if (list2 != NULL) {
      freeDocNodes(list2);
    }

    return NULL;
  }

  DocNode *combinedList = NULL;
  DocNode *lastNodeInCombinedList;
  DocNode *currentDoc = list1;
  DocNode *docToCheck;

  //iterate through each DocNode in list1
  while (currentDoc != NULL){


    //for each DocNode in list1, compare with each doc in list2
    docToCheck = list2;
    while (docToCheck != NULL) {

      //if we've found an intersection
      if (currentDoc->doc_id == docToCheck->doc_id) {

        //if this is the first intersection we've found..
        if (combinedList == NULL){

          //create the first copied DocNode
          combinedList = malloc(sizeof(DocNode));
          MALLOC_CHECK(combinedList);
          memset(combinedList , 0, sizeof(DocNode));

          //copy doc_id and combine frequencies
          combinedList->next = NULL;
          combinedList->doc_id = currentDoc->doc_id;
          combinedList->page_word_frequency = (currentDoc->page_word_frequency + docToCheck->page_word_frequency);

          lastNodeInCombinedList = combinedList;
          break;
        }
        else {
          //create new copied DocNode
          lastNodeInCombinedList->next = malloc(sizeof(DocNode));
          MALLOC_CHECK(lastNodeInCombinedList->next);
          memset(lastNodeInCombinedList->next, 0, sizeof(DocNode));

          //copy doc_id and combine frequencies
          lastNodeInCombinedList->next->next = NULL;
          lastNodeInCombinedList->next->doc_id = currentDoc->doc_id;
          lastNodeInCombinedList->next->page_word_frequency = (currentDoc->page_word_frequency + docToCheck->page_word_frequency);

          lastNodeInCombinedList = lastNodeInCombinedList->next;
          break;
        }


      }


      docToCheck = docToCheck->next;
    }


    currentDoc = currentDoc->next;
  }
  freeDocNodes(list1);
  freeDocNodes(list2);

  return combinedList;

}


/*

*getUnionOfLists*
--------------

Description: looks for the Union of two lists of DocNodes, taking the max of frequencies between matches
  returns a pointer to the first element of the combined list (list returned is the original masterList plus
  copies of unique members of list2)

Input: pointers to the first DocNodes of the two lists to match

Output: pointer to first match in combined list, or NULL for failure

*****
*/
DocNode *getUnionOfLists(DocNode *masterList, DocNode *list2){

  if (masterList== NULL && list2 == NULL) {
    return NULL;
  }
  else if (masterList== NULL) {
    return list2;
  }
  else if (list2 == NULL) {
    return masterList;
  }

  //setup
  DocNode *currentDoc = list2;
  DocNode *docToCheck;
  int foundIntersection = 0;

  //get last node in masterList
  docToCheck = masterList;
  while (docToCheck->next != NULL){
    docToCheck = docToCheck->next;
  }
  DocNode *lastNodeInMasterList = docToCheck;

  //iterate through each DocNode in list2, and add to masterList if it's not a duplicate
  while (currentDoc != NULL){


    //compare with every DocNode in masterList
    docToCheck = masterList;
    while (docToCheck != NULL) {

      //if we've found an intersection
      if (currentDoc->doc_id == docToCheck->doc_id) {

        //make masterList's frequency the max of the two
        if (docToCheck->page_word_frequency >= currentDoc->page_word_frequency){
          //do nothing
        }
        else {
          docToCheck->page_word_frequency = currentDoc->page_word_frequency;
        }
        
        foundIntersection = 1;
        break;
      }

      docToCheck = docToCheck->next;
    }

    if (foundIntersection == 0) {
      //we did not find an intersection. Add copy of currentDoc to the end of masterList
      lastNodeInMasterList->next = malloc(sizeof(DocNode));
      MALLOC_CHECK(lastNodeInMasterList->next);
      memset(lastNodeInMasterList->next, 0, sizeof(DocNode));
      lastNodeInMasterList->next->next = NULL;
      lastNodeInMasterList->next->doc_id = currentDoc->doc_id;
      lastNodeInMasterList->next->page_word_frequency = currentDoc->page_word_frequency;
      lastNodeInMasterList = lastNodeInMasterList->next;
    }

    foundIntersection = 0;
    currentDoc = currentDoc->next;
  }

  freeDocNodes(list2);
  return masterList;

}


//gets the intersection of all words in the AND list into a master linked list of DNODEs
//adds frequencies of matches
//returns pointer to first DNODE or NULL if no words
DocNode *getANDList(DNODE *ANDsentinel){

  //if no words, return null
  if (ANDsentinel->next == NULL) {
    return NULL;
  }

  //put first word's docs in masterList
  DNODE *currentDNODE = ANDsentinel->next;
  DocNode *masterList = copyDocNodeList(currentDNODE->data);

  //iterate through the rest of the words
  while (currentDNODE->next != NULL){

    //get next word
    currentDNODE = currentDNODE->next;
    DocNode *secondList = copyDocNodeList(currentDNODE->data);

    //get intersection into masterList
    masterList = getIntersectionOfLists(masterList, secondList);

  }

  return masterList;

}

//gets the union of all words in the OR list into a master linked list of DNODEs
  //takes max frequency of duplicates
  //returns pointer to first DNODE
DocNode *getORList(DNODE *ORsentinel){

  //if no words, return null
  if (ORsentinel == NULL || ORsentinel->next == NULL) {
    return NULL;
  }

  //put first word's docs in masterList
  DNODE *currentDNODE = ORsentinel->next;
  DocNode *masterList = copyDocNodeList(currentDNODE->data);

  //iterate through the rest of the words
  while (currentDNODE->next != NULL){

    //get next word
    currentDNODE = currentDNODE->next;
    DocNode *secondList = copyDocNodeList(currentDNODE->data);

    //get union into masterList
    masterList = getUnionOfLists(masterList, secondList);
  }

  return masterList;
}




//sorts DocNodes by frequency in descending order
DocNode *sortDocNodesByFrequency(DocNode *firstDocInMasterList){

  //iterate through each DocNode in the list to sort
  DocNode *currentDoc = firstDocInMasterList;
  DocNode *firstDocInNewList = NULL;
  while (currentDoc != NULL) {

    DocNode *docToCheck = firstDocInNewList;
    DocNode *prev;
    //if there is nothing in our new list
    if (docToCheck == NULL) {
      docToCheck = copyDocNode(currentDoc);
      docToCheck->next = NULL;
      prev = docToCheck;
      firstDocInNewList = docToCheck;
    }
    else {
      prev = docToCheck;
      //compare currentDoc with each element of our new list until its freq is larger
      while (docToCheck != NULL && currentDoc->page_word_frequency <= docToCheck->page_word_frequency){
        if (prev == docToCheck){
          docToCheck = docToCheck->next;
        }
        else {
          prev = prev->next;
          docToCheck = docToCheck->next;
        }
      }

      //currentDoc's freq > docToCheck
      if (prev == docToCheck) {
        //add at begininning of list
        DocNode *newBeginning = copyDocNode(currentDoc);
        newBeginning->next = docToCheck;
        firstDocInNewList = newBeginning;
      }
      else {
        prev->next = copyDocNode(currentDoc);
        prev->next->next = docToCheck;
      }

    }
    currentDoc = currentDoc->next;
  }

  freeDocNodes(firstDocInMasterList);
  return firstDocInNewList;
}




//prints the passed Docnode's frequency and URL
  //returns 1 for success, 0 for failure
int printDocNode(DocNode * toPrint, char *path){
  int freq = toPrint->page_word_frequency;
  char fileName[50];
  strcpy(fileName, path);
  char docFileName[20];
  int len = snprintf(docFileName, 20, "%d", toPrint->doc_id);
  strcat(fileName, docFileName);
  if (len <= 0) {
    return 0; //for failure
  }

  /* check if readable file exists */
  if( access( fileName, R_OK ) == -1 ) {
      #ifdef DEBUG
      printf("%s is not a readable file.\n", fileName);
      #endif
      return 0;
  }

  //open the file
  FILE *temp = fopen(fileName, "r");
  if (temp == NULL){
    return 0;
  }

  //fgets first line
  char url[1000];
  if (fgets(url, 1000, temp) != NULL){
    printf("Doc: %d  \t Frequency: %d\t URL: %s", toPrint->doc_id, freq, url);
    fclose(temp);
    return 1;
  }

  fclose(temp);
  return 0;
}



