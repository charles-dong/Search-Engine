#!/bin/bash
#
# File: test_and_print.sh
#
# Description: Tests crawler and prints to crawltestlog.date
#
# Input: URL to crawl
#
# Output: prints log to crawltestlog.date

#input validation
if [ $# != 1 ]
then
	echo "Please enter the URL you'd like to crawl. Thanks!"
	exit 1
fi

date=`date`
filename="crawltestlog.$date"
url="$1"
echo "URL is $url. Crawler test results will be written to $filename"
echo

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi


#Unit Testing
echo "Unit Testing of important functions"
echo "Unit Testing of important functions" >> "$filename"
make test >> /dev/null
a.out >> "$filename"
if [ $? -ne 0 ]; then
echo Unit testing failed
	exit 1
fi
echo "All tests passed."
echo "All tests passed." >> "$filename"

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi

#build crawler
make crawl >> /dev/null
echo "We have a clean crawler build" >> "$filename"
echo

echo "Input validation: invalid URL"
echo >> "$filename"
echo "Input validation: invalid URL" >> "$filename"
./crawl http://www.cs.drtmouth.edu ../data/ 1 >> "$filename"
if [ $? -eq 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawl >> /dev/null

echo "Input validation: invalid number of arguments"
echo >> "$filename"
echo "Input validation: invalid number of arguments " >> "$filename"
./crawl $url hi ../data/ 1 >> "$filename"
if [ $? -eq 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawl >> /dev/null

echo "Input validation: invalid path" 
echo >> "$filename"
echo "Input validation: invalid path" >> "$filename"
./crawl $url ../ta 0 >> "$filename"
if [ $? -eq 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawl >> /dev/null


echo "Input validation: negative depth"
echo >> "$filename"
echo "Input validation: negative depth" >> "$filename"
./crawl $url ../data/ -1 >> "$filename"
if [ $? -eq 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawl >> /dev/null

echo "Input validation: depth too large"
echo >> "$filename"
echo "Input validation: depth too large" >> "$filename"
./crawl $url ../data/ 5 >> "$filename"
if [ $? -eq 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawlprint >> /dev/null

echo "Test 1: depth 0"
echo >> "$filename"
echo "Test 1: depth 0" >> "$filename"
mkdir ../data0
echo "Test 1 data will be saved in ../data0"
echo "Test 1 data will be saved in ../data0" >> "$filename"
./crawl $url ../data0/ 0 >> "$filename"
if [ $? -ne 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make leavedata >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawlprint >> /dev/null

echo "Test 2: depth 1"
echo >> "$filename"
echo "Test 2: depth 1" >> "$filename"
mkdir ../data1
echo "Test 2 data will be saved in ../data1"
echo "Test 2 data will be saved in ../data1" >> "$filename"
./crawl $url ../data1/ 1 >> "$filename"
if [ $? -ne 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make leavedata >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawlprint >> /dev/null

echo "Test 3: depth 2"
echo >> "$filename"
echo "Test 3: depth 2" >> "$filename"
mkdir ../data2
echo "Test 3 data will be saved in ../data2"
echo "Test 3 data will be saved in ../data2" >> "$filename"
./crawl $url ../data2/ 2 >> "$filename"
if [ $? -ne 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make leavedata >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi
make crawlprint >> /dev/null

echo "Test 4: depth 3"
echo >> "$filename"
echo "Test 4: depth 3" >> "$filename"
mkdir ../data3
echo "Test 4 data will be saved in ../data3"
echo "Test 4 data will be saved in ../data3" >> "$filename"
./crawl $url ../data3/ 3 >> "$filename"
if [ $? -ne 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make leavedata >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi

echo "All tests successful!"
echo "Number of files downloaded:"
echo "Depth 0:"; ls ../data0 | wc -l
echo "Depth 1:"; ls ../data1 | wc -l
echo "Depth 2:"; ls ../data2 | wc -l
echo "Depth 3:"; ls ../data3 | wc -l



