
/*

  FILE: list.c

  Description: creates and manipulates URL Nodes and Lists 

  Functions: makeURLNODE, makeURLlist, updateListURLsToBeVisited, validURL, extractURLs, setURLasVisited

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawler.h"
#include "../util/dictionary.h"
#include "list.h"
#include "../util/header.h"
#include "../util/file.h"
#include "../util/hash.h"
#include "../util/html.h"




/*

*makeURLNODE*
------------

Description: mallocs and memsets a new URLNODE - returns pointer to it

Input: n/a 

Return: pointer to new URLNODE

** Pseudo Code **

(1) malloc new URLNODE
(2) memset everything

*****

*/
URLNODE * makeURLNODE(char *url,int d){

  URLNODE *urlnode = malloc(sizeof(URLNODE));
  MALLOC_CHECK(urlnode);
  memset(urlnode, 0, sizeof(URLNODE));

  urlnode->depth = d;
  urlnode->visited = 0;
  strncpy(urlnode->url, url, MAX_URL_LENGTH);

  return urlnode;
}


/*

*makeURLlist*
------------

Description: mallocs and memsets a new URL List, which holds URLs extracted
  from current URL's getPage call - returns pointer to it

Input: n/a 

Return: pointer to new URL List

** Pseudo Code **

(1) malloc new URL List
(2) memset everything

*****

*/
//factory for URL List - holds URLs extracted from current URL's getPage call
char **makeURLlist(){
  
  char **url_list = malloc(sizeof(char *)*MAX_URL_PER_PAGE);
  MALLOC_CHECK(url_list);
  memset(url_list, 0, sizeof(url_list[0])*MAX_URL_PER_PAGE);

  return url_list;

}


/*

*updateListURLsToBeVisited*
------------

Description: for each URL in the given URL List, adds it to the dictionary
  if it's unique and <= maxDepth

Input: dictionary, URL List, currentDepth

Return: n/a

** Pseudo Code **

(1) iterates through each url in URL List, and adds to dictionary if unique
  Note: also frees the memory malloc'ed for each URL string
(2) resets URL List to 0s

*****

*/
void updateListURLsToBeVisited(DICTIONARY* dict, char **url_list, int depth){

  //while the url_list still points to a URL
    //add each url to the dictionary (auto checks for uniqueness)
  for (int i = 0; url_list[i] != NULL; i++ ) {
    URLNODE *urlnode = makeURLNODE(url_list[i], depth);
    DNODE *duplicate = addElementToDictionary(dict, url_list[i], urlnode);
    if (duplicate != NULL){
      free(urlnode);
    }
    free(url_list[i]);
  }

  //set url_list back to zeros
  memset(url_list, 0, sizeof(url_list[0])*MAX_URL_PER_PAGE);

}



/*

*validURL*
------------

Description: checks to see if the passed url is valid (returns 0 for valid)

Input: url

Return: 0 for valid

** Pseudo Code **

(1) return 0 if we can successfully access the url

*****

*/
int validURL(char * url){

  char cmd[MAX_URL_LENGTH];
  snprintf(cmd, sizeof(cmd), "curl --output /dev/null --head --silent --fail %s", url); 

  //system returns 0 if success
  return system(cmd);

}

/*

*extractURLs*
------------

Description: extract all URLs from a given HTML string into URL List (from urlToVisit)

Input: page, URL List, urlToVisit

Return: n/a

** Pseudo Code **

(1) memset URL List and initialize variables
(2) if getPage failed, return NULL 
(3) keep getting the next URL from campbell's parser until we reach the end of the string
(4) for each URL parsed, if it matches our prefix, normalize it, malloc a new char array 
  for it that's pointed to by URL List, and copy it into the newly malloc'ed char array

*****

*/
void extractURLs(char** url_list, char* page, char* urlToVisit) {
  
  // reset url_list to null pointers
  memset(url_list, 0, sizeof(url_list[0])*MAX_URL_PER_PAGE);
  
  //initialize variables
  int counter = 0; //how many URLs we've parsed
  int pos = 0; //start parse from this position in the HTML
  char result[MAX_URL_LENGTH]; //next parsed URL will be written into result
  memset(result, 0, sizeof(char)*MAX_URL_LENGTH);

  //if getPage failed, return
  if ( page == NULL ){

    #ifdef DEBUG
    printf("No URLs extracted from %s.\n", urlToVisit);
    #endif

    return;
  }
  else {
    //returns 1 + pos of the newfound URL in HTML, -1 if end of doc is reached
    //while there are still more URLs.. get the next one into result
    while ((pos = GetNextURL(page, urlToVisit, result, pos)) > 0) {

      //if new URL doesn't match our prefix, throw it away
      #ifdef PREFIX
      if ( strncmp(URL_PREFIX, result, (strlen(URL_PREFIX) - 1)) != 0 ){
        memset(result, 0, sizeof(char)*MAX_URL_LENGTH); //reset result, in which next parsed URL will be written
        continue;
      }
      #endif

      //normalize new URL
      if ( NormalizeURL(result) ){ //returns 1 if valid, 0 if not

      //create a new char array to hold the URL
      url_list[counter] = malloc(sizeof(char)*MAX_URL_LENGTH);
      memset(url_list[counter], 0, sizeof(char)*MAX_URL_LENGTH);
      //copy the new URL into it
      strcpy(url_list[counter], result); 

      //maintenance
      counter++;
      }

      memset(result, 0, sizeof(char)*MAX_URL_LENGTH); //reset result, in which next parsed URL will be written

    }
  }

  #ifdef DEBUG
  /*open new file with name logOutput and append*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\t%d URLs parsed from %s!\n\n", counter, urlToVisit);
  fclose(newFile);
  #endif

}


/*

*setURLasVisited*
------------

Description: sets specified URL as visited in its corresponding URLNode

Input: dictionary, url (to set as visited)

Return: n/a

** Pseudo Code **

(1) get hash value of url
(2) if there's nothing at the dict[hashValue], return
(3) iterate through each URL with the same hash value
  if we reach the end of the dictionary or cluster, return
  if we find the URL in question, set its URLNODE's 'visited' variable to 1

*****

*/
void setURLasVisited(DICTIONARY* dict, char* url){
  
  //get hash value
  unsigned long hashValue = hash1(url)%(unsigned long)MAX_HASH_SLOT;

  //check to make sure the URL is in our dictionary
  if ( dict->hash[hashValue] == NULL ) {
    #ifdef DEBUG
    /*open new file with name logOutput and append*/
    char *f = "logOutput";
    FILE *newFile = fopen(f,"a");
    fprintf(newFile, "Sorry, we couldn't find %s, which we were supposed to set as 'visited'\n", url);
    fclose(newFile);
    #endif
    return;
  }
  else {
    DNODE *nodeToCheck = dict->hash[hashValue];

    //For each DNODE with the same hashValue..
    while ( hashValue == (hash1(nodeToCheck->key)%MAX_HASH_SLOT)){

      //if we've found the DNODE we're looking for..
      if (strcmp(nodeToCheck->key, url) == 0){

        //mark it as VISITED
        ((URLNODE*)nodeToCheck->data)->visited = 1;

        return;
      }
      
      //if we've reached the end of the dictionary or cluster...
      if ( nodeToCheck == dict->end || hashValue != (hash1(nodeToCheck->next->key)%MAX_HASH_SLOT)){
        #ifdef DEBUG
        /*open new file with name logOutput and append*/
        char *f = "logOutput";
        FILE *newFile = fopen(f,"a");
        fprintf(newFile, "Sorry, we couldn't find %s, which we were supposed to set as 'visited'\n", url);
        fclose(newFile);
        #endif
        return;
      }

      //go to next DNODE
      nodeToCheck = nodeToCheck->next;
        
    }
    
    //just in case
    #ifdef DEBUG
    /*open new file with name logOutput and append*/
    char *f = "logOutput";
    FILE *newFile = fopen(f,"a");
    fprintf(newFile, "Sorry, we couldn't find %s, which we were supposed to set as 'visited'\n", url);
    fclose(newFile);
    #endif
    return;
  }
}

