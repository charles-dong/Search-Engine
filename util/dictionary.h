/*

  FILE: dictionary.h

  Description: header for dictionary data structure

*/


#ifndef _DICTIONARY__H_
#define _DICTIONARY__H_

#define KEY_LENGTH 1000
#define MAX_HASH_SLOT 10000 //size of hash table

// Dictionary Node. This is a general double link list structure that
// holds the key (URL - we explained into today's lecture why this is there)
// and a pointer to void that points to a URLNODE in practice. 
// key is the same as URL recall.

typedef struct DNODE {
  struct DNODE  *next;
  struct DNODE  *prev;
  void    *data;        //  actual data points to URLNODE
  char key[KEY_LENGTH]; //  actual (URL) key 
  int docCount;
} DNODE;


// The DICTIONARY holds the hash table and the start and end pointers into a double 
// link list. This is a unordered list with the exception that DNODES with the same key (URL) 
// are clusters along the list. So you hash into the list. Check for uniqueness of the URL.
// If not found add to the end of the cluster associated wit the same URL. You will have
// to write an addElement function.

typedef struct {
  DNODE *hash[MAX_HASH_SLOT]; // the hash table of slots, each slot points to a DNODE
  DNODE *start;               // start of double link list of DNODES terminated by NULL pointer
  DNODE *end;                 // points to the last DNODE on this list
} DICTIONARY;

//data structure holding each document's ID and frequency of word in question
typedef struct _DocNode {
  struct _DocNode *next;        // pointer to the next member of the list.
  int doc_id;                   // document identifier
  int page_word_frequency;           // number of occurrences of the word
} __DocNode;
typedef struct _DocNode DocNode;


//make null-intiialized dictionary and return pointer to it
DICTIONARY *makeDictionary();

//add an element to the appropriate place in the dictionary
DNODE *addElementToDictionary(DICTIONARY* dict, char* key, void* data);

//scan dictionary for next unvisited URL and return pointer to it
//updates current_depth with depth of returned URL's depth
char *getAddressFromLinksToBeVisited(DICTIONARY* dict, int *currentDepth);

//print all urls in dictionary
void printDictionary(DICTIONARY* dict);

//print all data - lots of data
void printDictionary2(DICTIONARY* dict);

//frees all URLNODES and DNODES and dictionary
void freeDictionary(DICTIONARY* dict);

//free DNODEs in list - takes first DNODE in list
void freeDNODEs(DNODE* nodeToCheck);

//free DocNodes in list - takes first DocNode in list
void freeDocNodes(DocNode* nodeToCheck);


/* index-specific functions */

//adds the passed word to the index dictionary in appropriate manner
int updateIndex(DICTIONARY *indexDict, char *word, int docID);

//factory method for new DocNode
DocNode *makeDocNode(int docID);

//if word is duplicate, updateIndex calls this
//Adds DocNode if doc_id not already present - otherwise, increments frequency
int addDuplicateWord(DNODE *wordNode, DocNode *docNode);

//saves the index dictionary structure as a .dat file in the current directory
int saveIndexToFile(DICTIONARY *index, char *saveFileName);

//print index Dictionary 
void printDictionary3(DICTIONARY* dict);

//frees all DocNodes and DNODES and index
void freeIndex(DICTIONARY* dict);

//recreates an index from the file passed (probably index.dat)
DICTIONARY *recreateIndex(char *fileName);




/* query engine specific functions */

//return array of pointers to DNODEs
//the DNODE pointed to has the key AND or OR, depending on the list type and points to
  //a linked list of word DNODEs corresponding to the words in each list
int parseQuery(char *query, DNODE** parsedNodes, DICTIONARY *index);

//frees each DNODE in parsedNodes
void freeParsedNodes(DNODE** parsedNodes, int numberOfKeywords);

//looks for the intersection of two lists of DNODEs, adding frequencies of matches
  //returns a pointer to the first match in the list
DocNode *getIntersectionOfLists(DocNode *list1, DocNode *list2);

//gets the intersection of all words in the AND list into a master linked list of DNODEs
  //adds frequencies of matches
  //returns pointer to first DNODE
DocNode *getANDList(DNODE *ANDsentinel);

//gets the union of all words in the OR list into a master linked list of DNODEs
  //takes max frequency of duplicates
  //returns pointer to first DNODE
DocNode *getORList(DNODE *ORsentinel);

//returns pointer to DNODE (Word Node) with the key passed, or NULL if not found
DNODE *getDNODEWithKey(char *key, DICTIONARY *dict);

//sorts DocNodes by frequency in descending order
DocNode *sortDocNodesByFrequency(DocNode *firstDocInMasterList);

//prints the passed Docnode's frequency and URL
int printDocNode(DocNode * toPrint, char *path); 

//copies DocNode List passed and returns pointer to first element
DocNode *copyDocNodeList(DocNode *);


#endif
