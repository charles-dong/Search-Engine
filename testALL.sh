#!/bin/bash
#
# File: testQueryEngine.sh
#
# Description: Tests crawler, indexer, and runs query engine. and prints to 
#	crawl/indextestlog.date in lab6/crawler and lab6/indexer, respectively.
#	Data is stored in ../data3/ (depth 3 run on http://www.cs.dartmouth.edu/~campbell)
#
# Input: n/a
#
# Output: prints logs to crawl/indextestlog.date in lab6/crawler and lab6/indexer, respectively

#test crawler
cd crawler
test_and_print.sh http://www.cs.dartmouth.edu/~campbell
make leavedata
cd ..

#test indexer
cd indexer
test_indexer.sh
make leavedat
cd ..

#test and run query engine
testQueryEngine.sh
