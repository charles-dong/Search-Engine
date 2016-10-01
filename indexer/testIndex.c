#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../util/minunit.h"
#include "../util/header.h"
#include "../util/file.h"
#include "../util/hash.h"
#include "../util/dictionary.h"
#include "./buildIndex.h"

int tests_run = 0;
char *path;

//Testing getNextWordFromHTMLDoc
static char *test_getNextWordFromHTMLDoc(){
  printf("\n\nTesting getNextWordFromHTMLDoc\n"); 
  /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting getNextWordFromHTMLDoc\n");

  char *path = "../data/4";
  fprintf(newFile, "\nHere are the words from: %s\n", path); 

  //read file into pageBuffer
  char *pageBuffer = readFileToBuffer(path);

  //set up for getNextWordFromHTMLDoc
  int currentPosition = 0;
  char * word = malloc(sizeof(char)*MAX_WORD_LENGTH);
  MALLOC_CHECK(word);
  memset(word, 0, sizeof(char)*MAX_WORD_LENGTH);

  //process pageBuffer, adding appropriate DNODEs and DocNodes into our index DICTIONARY
  while ((currentPosition = getNextWordFromHTMLDoc(pageBuffer, word, currentPosition)) != 0){
    fprintf(newFile, "%s\n", word);
  }

  //cleanup
  free(pageBuffer);
  free(word);
  fclose(newFile);

  return 0;
}


//Testing createIndex
static char *test_createIndex(){
  printf("\n\nTesting createIndex\n"); 
  /*append to logOutput*/
  char *f = "logOutput";
  FILE *newFile = fopen(f,"a");
  fprintf(newFile, "\n\nTesting createIndex\n");

  char *path = "../data/";
  createIndex(path);
  
  //cleanup
  fclose(newFile);

  return 0;
}






static char *all_tests() {

    mu_run_test(test_getNextWordFromHTMLDoc);
    mu_run_test(test_createIndex);

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

