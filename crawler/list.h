/*

  FILE: list.h

  Description: header for URL lists, URLNODEs and related functions

*/

#ifndef _LIST__H_
#define _LIST__H_

// This is the key data structure that holds the information of each URL.
typedef struct {
  char url[MAX_URL_LENGTH];      // e.g., www.cs.dartmouth.edu
  int depth;                     //  depth associated with this URL.
  int visited;                   //  crawled or not, marked true(1), otherwise false(0)
} URLNODE;

//factory for URLNODE with passed URL and depth
URLNODE * makeURLNODE(char *url, int d);

//factory for URL List - holds URLs extracted from current URL's getPage call
char **makeURLlist();

// Takes the url_list* and for each URL in the list, adds it to the dictionary
  //Note: only adds if unique
void updateListURLsToBeVisited(DICTIONARY *dict, char **url_list, int depth);

//check if URL is valid
int validURL(char * url);

// Mark URL as visited in URLNODE structure
void setURLasVisited(DICTIONARY* dict, char* url);

//extract all URLs from a page (passed as char *pageBuffer) into char **url_list
void extractURLs(char** url_list, char* page, char* urlToVisit); 


#endif

