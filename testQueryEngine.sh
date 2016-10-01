#!/bin/bash
#
# File: testQueryEngine.sh
#
# Description: Tests and runs query engine with ./indexer/index.dat
#	Data is stored in ../data3/ (depth 3 run on http://www.cs.dartmouth.edu/~campbell)
#
# Input: n/a
#
# Output: prints log to querytestlog.date

date=`date`
filename="querytestlog.$date"
echo "Query Engine test results will be written to $filename"

#make index file
make index >> /dev/null
if [ $? -ne 0 ]; then
echo Make index failed
	exit 1
fi
# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi

#unit tests
make test >>/dev/null
a.out >> "$filename"

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi

#build query
make query >> /dev/null
echo "We have a clean query engine build" >> "$filename"
echo

#Test 1: input validation - # of arguments
echo "Input validation: invalid number of arguments"
echo >> "$filename"
echo "Input validation: invalid number of arguments " >> "$filename"
./query ./indexer/index.dat ./data3/ hi >> "$filename"
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
make query >> /dev/null

#Test 2: input validation - path
echo "Input validation: invalid path"
echo >> "$filename"
echo "Input validation: invalid path" >> "$filename"
./query ./indexer/index.dat ./d/ >> "$filename"
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
make query >> /dev/null

#Test 3: input validation - invalid .dat file
echo "Input validation: invalid .dat filename"
echo >> "$filename"
echo "Input validation: invalid .dat filename" >> "$filename"
./query ./indexer/wooooo.dat ./data3/ >> "$filename"
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
make query >> /dev/null

echo "All input validation tests successful!"
echo >> "$filename"
echo "All input validation tests successful!" >> "$filename"
echo

# Run query engine
echo "Running Query Engine"
echo >> "$filename"
echo "Running Query Engine" >> "$filename"
./query ./indexer/index.dat ./data3/