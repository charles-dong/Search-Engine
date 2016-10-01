/*

  FILE: crawler.h

  Description: header for entire crawler - has defines for max numbers, URL domain to stay within,
    and data structures used in the crawler

*/


#ifndef _CRAWLER_H_
#define _CRAWLER_H_


// DEFINES

#define MAX_LINE 1000 // Max length of each fgets line
#define MAX_URL_LENGTH 1000   // The max length of each URL path.
#define MAX_URL_PER_PAGE 10000
#define INTERVAL_PER_FETCH 1
#define MAX_URL_LENGTH 1000


// The URL we crawled should only start with this prefix. 
// You could remove this limtation but not before testing.
// The danger is a site may block you

#define URL_PREFIX "http://www.cs.dartmouth.edu"


  
#endif
