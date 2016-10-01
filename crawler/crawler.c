/*

  FILE: crawler.c

  Description: for a given seed url, directory to put data in, and max crawling depth, downloads and
    parses the seed webpage, extracts any embedded URLs that are tagged, retrieves those pages, and so on.
    Once the crawler has completed at least one complete crawling cycle (i.e., it has visited a target number 
    of Web pages which is defined by a depth parameter on the crawler command line), then the crawler process
    will complete its operation.

  Inputs: ./crawler [SEED URL] [TARGET DIRECTORY WHERE TO PUT THE DATA] [MAX CRAWLING DEPTH]

  Outputs: For each webpage crawled the crawler program will create a file in the 
  [TARGET DIRECTORY]. The name of the file will start a 1 for the  [SEED URL] 
  and be incremented for each subsequent HTML webpage crawled. 

  Each file (e.g., 10) will include the URL associated with the saved webpage and the
  depth of search in the file. The URL will be on the first line of the file 
  and the depth on the second line. The HTML will for the webpage 
  will start on the third line.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "crawler.h"
#include "../util/dictionary.h"
#include "list.h"
#include "../util/header.h"
#include "../util/file.h"
#include "../util/hash.h"
#include "../util/html.h"

/*
*Crawler*
-------------

// Input command processing logic

(1) Command line processing on arguments
    Inform the user if arguments are not present
    IF target_directory does not exist OR depth exceeds max_depth THEN
       Inform user of usage and exit failed

// Initialization of any data structures

(2) *initLists* Initialize any data structure and variables

// Bootstrap part of Crawler for first time through with SEED_URL

(3) page = *getPage(seedURL, current_depth, target_directory)* Get HTML into a string and return as page, 
            also save a file (1..N) with correct format (URL, depth, HTML) 
    IF page == NULL THEN
       *log(PANIC: Cannot crawl SEED_URL)* Inform user
       exit failed
(4) URLsLists = *extractURLs(page, SEED_URL)* Extract all URLs from SEED_URL page.
  
(5) *free(page)* Done with the page so release it

(6) *updateListLinkToBeVisited(URLsLists, current_depth + 1)*  For all the URL 
    in the URLsList that do not exist already in the dictionary then add a DNODE/URLNODE 
    pair to the DNODE list. 
(7) *setURLasVisited(SEED_URL)* Mark the current URL visited in the URLNODE.

// Main processing loop of crawler. While there are URL to visit and the depth is not 
// exceeded keep processing the URLs.
(8) WHILE ( URLToBeVisited = *getAddressFromTheLinksToBeVisited(current_depth)* ) DO
        // Get the next URL to be visited from the DNODE list (first one not visited from start)
 
      IF current_depth > max_depth THEN
    
          // For URLs that are over max_depth, we just set them to visited
          // and continue on
    
          setURLasVisited(URLToBeVisited) Mark the current URL visited in the URLNODE.
          continue;

    page = *getPage(URLToBeVisited, current_depth, target_directory)* Get HTML into a 
            string and return as page, also save a file (1..N) with correct format (URL, depth, HTML) 

    IF page == NULL THEN
       *log(PANIC: Cannot crawl URLToBeVisited)* Inform user
       setURLasVisited(URLToBeVisited) Mark the bad URL as visited in the URLNODE.
       Continue; // We don't want the bad URL to stop us processing the remaining URLs.
   
    URLsLists = *extractURLs(page, URLToBeVisited)* Extract all URLs from current page.
  
    *free(page)* Done with the page so release it

    *updateListLinkToBeVisited(URLsLists, current_depth + 1)* For all the URL 
    in the URLsList that do not exist already in the dictionary then add a DNODE/URLNODE 
    pair to the DNODE list. 

    *setURLasVisited(URLToBeVisited)* Mark the current URL visited in the URLNODE.

    // You must include a sleep delay before crawling the next page 
    // See note below for reason.

    *sleep(INTERVAL_PER_FETCH)* Sneak by the server by sleeping. Use the 
     standard Linux system call

(9)  *log(Nothing more to crawl)

(10) *cleanup* Clean up data structures and make sure all files are closed,
      resources deallocated.

*/

int MAX_DEPTH = 4; //max depth of 4 - user inputs above this are not accepted
int maxDepth, currentDepth;

int main(int argc, char *argv[]) {
  
  // Command line processing of arguments
  printf("You entered: \n[SEED_URL] %s\n[TARGET_DIRECTORY] %s\n[CRAWLING_DEPTH] %s\n\n", argv[1], argv[2], argv[3]);
  if (argc != 4) {
    printf("Incorrect # of arguments. Usage: crawler [SEED URL] [DIRECTORY TO PUT DATA IN] [MAX CRAWLING DEPTH]\n");
    exit(EXIT_FAILURE);
  }
  int valid = validURL(argv[1]);
  if (valid) {
    printf("Please enter a valid URL. Thanks!\n");
    exit(EXIT_FAILURE);
  }
  if(isDirectory(argv[2]) != 1) {
    printf ("Please enter a valid directory path. Thanks!\n");
    exit(EXIT_FAILURE);
  }
  maxDepth = atoi(argv[3]);
  if ( (maxDepth > MAX_DEPTH) || (maxDepth < 0) ) {
    printf("Incorrect depth. Please enter a number between 0 and %d. Thanks!\n", MAX_DEPTH);
    exit(EXIT_FAILURE);
  }

  //initialize data structures and variables
  int fileNumber = 1;
  DICTIONARY *dict = makeDictionary();
  char **url_list = makeURLlist();
  currentDepth = 0;
  char *path = NormalizePath(argv[2]);
  char *URLToBeVisited;

  //bootstrap for first time with Seed URL
    //normalize new URL
    NormalizeWord(argv[1]);
    if ( NormalizeURL(argv[1]) == 0 ){ //returns 1 if valid, 0 if not
      printf("Please enter a valid URL. Thanks!\n");
      exit(EXIT_FAILURE);
    }
  URLNODE *urlnode = makeURLNODE(argv[1], currentDepth);
  addElementToDictionary(dict, argv[1], urlnode); 
  char *page = getPage(argv[1], currentDepth, path, &fileNumber);
  if ( page == NULL ) {
    printf("Sorry, cannot crawl %s.\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  //extract URLs from Seed URL webpage
  extractURLs( url_list, page, argv[1]);
  free(page);

  //add extracted URLs to dictionary
  updateListURLsToBeVisited(dict, url_list, (currentDepth+1));

  //set Seed URL as visited
  setURLasVisited(dict, argv[1]);

  //main processing loop
  //evaluates left operand, which will be NULL when there are no more URLs
  //while there are still URLs to visit and we have NOT reached max depth, keep processing URLs
  while ( (URLToBeVisited = getAddressFromLinksToBeVisited(dict, &currentDepth)) != NULL ){
        
    //if we're over max_depth, just set them to visited and continue
    if (currentDepth > maxDepth) {

      #ifdef DEBUG
      /*open new file with name logOutput and append*/
      char *f = "logOutput";
      FILE *newFile = fopen(f,"a");
      fprintf(newFile, "%s - not crawling - over maxDepth %d (%d)\n", URLToBeVisited, maxDepth, currentDepth);
      fclose(newFile);
      #endif

      setURLasVisited(dict, URLToBeVisited);
      continue;
    }

    //get URLToBeVisited into a string and return it as page (also saves a file 1...N with URL, depth, HTML)
    page = getPage(URLToBeVisited, currentDepth, path, &fileNumber);

    //if there's a problem getting the page..
    if (page == NULL) {

      #ifdef DEBUG
      /*open new file with name logOutput and append*/
      char *f = "logOutput";
      FILE *newFile = fopen(f,"a");
      fprintf(newFile, "%s - failed to crawl\n\n", URLToBeVisited);
      fclose(newFile);
      #endif

      setURLasVisited(dict, URLToBeVisited);
      continue;
    }

    // if we've reached maxDepth, don't add any new URLs to the dictionary
    if (currentDepth == maxDepth) {
      free(page);
      setURLasVisited(dict, URLToBeVisited);
      sleep(INTERVAL_PER_FETCH);
      continue;
    }
    else {
      //puts all URLs into the memory that url_list points to
      extractURLs(url_list, page, URLToBeVisited);
      free(page);

      //For all URLs in url_list, add to dictionary if unique
      updateListURLsToBeVisited(dict, url_list, (currentDepth+1));

      //set URLToBeVisited as visited
      setURLasVisited(dict, URLToBeVisited);
      
      //sleep delay before crawling next page
      sleep(INTERVAL_PER_FETCH);
    }
  }

  printf("Done crawling!!\n");
  printf("%d files downloaded.\n", (fileNumber - 1));

  #ifdef DEBUG
  printDictionary(dict);
  #endif

  /* clean up */
  freeDictionary(dict);
  free(url_list);

  return 0;
}




