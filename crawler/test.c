#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../util/minunit.h"
#include "./crawler.h"
#include "../util/header.h"
#include "../util/file.h"
#include "../util/hash.h"
#include "../util/dictionary.h"
#include "./list.h"

int tests_run = 0;
char *page;
char *path;
char *globalURL;
DICTIONARY *dict;

//Testing getPage
static char *test_getPage() {
  printf("\n\nTesting getPage\n"); 

  /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting getPage\n"); 
  fclose(newFile);

  int fileNumber = 4;
  printf("fileNumber is now %d\n", fileNumber);
  int depth = 2;
  path = "../data/";
  globalURL = "http://www.cs.dartmouth.edu/~campbell/cs50";
  page = getPage(globalURL, depth, path, &fileNumber);
  printf("fileNumber is now %d\n", fileNumber);

  return 0;
}

//Testing getAddressFromLinksToBeVisited
static char *test_getAddress() {
  printf("\n\nTesting getAddressFromLinksToBeVisited\n");
    /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting getAddressFromLinksToBeVisited\n"); 
  fclose(newFile);
  char *nextURL;
  int currentDepth = 0;
  printDictionary(dict);

  printf("\nRunning getAddressFromLinksToBeVisited\n");
  while ( (nextURL = getAddressFromLinksToBeVisited( dict, &currentDepth)) ) {
    printf("Setting nextURL %s to visited.\n", nextURL);
    setURLasVisited(dict, nextURL);
    printDictionary(dict);
  }

  printDictionary(dict);

  return 0;
}



//Testing extractURLs
static char *test_extractURLs() {
  printf("\n\nTesting extractURLs\n"); 
  /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting extractURLs\n"); 
  fclose(newFile);

  char **url_list = makeURLlist();
  extractURLs(url_list, page, globalURL); 
  int j = 0;
  while (url_list[j] != NULL){
    j++;
  }
  if (j == 0) {
    return 0;
  }
  else {
    printf("url_list[%d] = %s\n", (j-1), url_list[j-1]);
    printf("url_list[%d] = %s\n", (j), url_list[j]);
  }
  return 0;
}

//Testing setURLasVisited
static char *test_setURLasVisited() {
  printf("\n\nTesting setURLasVisited\n"); 
  /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting setURLasVisited\n"); 
  fclose(newFile);

  printDictionary(dict);

  printf("Setting %s to visited.\n", globalURL);
  setURLasVisited(dict, globalURL);
  
  printDictionary(dict);
  return 0;
}



//Testing dictionary
static char *test_dictionary() {

  printf("\n\nTesting dictionary\n"); 
  /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting dictionary\n"); 
  fclose(newFile);

  //setup
  dict = (DICTIONARY *)makeDictionary();
  char *url = "www.dartmouth.edu";
  int d = 1;
  printDictionary(dict);

  //add initial element
  URLNODE * urlnode = makeURLNODE(url, d);
  addElementToDictionary(dict, url, urlnode);
  printf("Adding %s to dictionary\n", url);
  mu_assert("Error - adding initial element to dictionary failed\n", dict->start != NULL && dict->start == dict->end);
  printDictionary(dict);

  //add another element
  url = "www.google.com";
  printf("Adding %s to dictionary\n", url);
  urlnode = makeURLNODE(url, d);
  addElementToDictionary(dict, url, urlnode);
  mu_assert("Error - adding 2nd element to dictionary failed\n", dict->start != dict->end);
  printDictionary(dict);

  //add another element
  d = 3;
  url = "www.yahoo.com";
  urlnode = makeURLNODE(url, d);
  printf("Adding %s to dictionary\n", url);
  addElementToDictionary(dict, url, urlnode);
  mu_assert("Error - adding 3rd element to dictionary failed\n",  dict->hash[hash1("www.google.com")%(unsigned long)MAX_HASH_SLOT]->key != dict->end->key);
  printDictionary(dict);

  //add element 2 again
  url = "www.google.com";
  urlnode = makeURLNODE(url, d);
  printf("Adding %s to dictionary\n", url);
  addElementToDictionary(dict, url, urlnode);
  printDictionary(dict);

  //add element 2 again
  url = "www.yahoo.com";
  printf("Adding %s to dictionary\n", url);
  urlnode = makeURLNODE(url, d);
  addElementToDictionary(dict, url, urlnode);
  printDictionary(dict);

  return 0;
}



static char *all_tests() {
    //Crawler Unit Tests
    mu_run_test(test_dictionary);
    mu_run_test(test_getPage);
    mu_run_test(test_extractURLs);
    mu_run_test(test_setURLasVisited);
    mu_run_test(test_getAddress);


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

