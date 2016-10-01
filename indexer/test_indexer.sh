#!/bin/bash
#
# File: test_indexer.sh
#
# Description: Tests indexer and prints to indextestlog.date. Uses data stored
#	in ../data/
#
# Input: n/a
#
# Output: prints log to indextestlog.date

date=`date`
filename="indextestlog.$date"
echo "Indexer test results will be written to $filename"

# Clean up
make clean >> /dev/null
if [ $? -ne 0 ]; then
echo Clean failed
	exit 1
fi

#build indexer
make index >> /dev/null
echo "We have a clean indexer build" >> "$filename"
echo

#Test 1: input validation - # of arguments
echo "Input validation: invalid number of arguments"
echo >> "$filename"
echo "Input validation: invalid number of arguments " >> "$filename"
./index ../data/ index.dat hi >> "$filename"
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
make index >> /dev/null

#Test 2: input validation - # of arguments
echo "Input validation: invalid number of arguments"
echo >> "$filename"
echo "Input validation: invalid number of arguments " >> "$filename"
./index ../data/ >> "$filename"
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
make index >> /dev/null

#Test 3: input validation - invalid path
echo "Input validation: invalid path" 
echo >> "$filename"
echo "Input validation: invalid path" >> "$filename"
./index ./yay index.dat >> "$filename"
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
make index >> /dev/null

#Test 4: input-validation - path without backslash
echo "Input validation: path without backslash"
echo >> "$filename"
echo "Input validation: path without backslash" >> "$filename"
./index ../data index.dat >> "$filename"
if [ $? -ne 0 ]; then
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
make index >> /dev/null

#Test 5: input-validation - different [RESULTS FILENAME]s
echo "Input validation: different [RESULTS FILENAME]s"
echo >> "$filename"
echo "Input validation: different [RESULTS FILENAME]s" >> "$filename"
./index ../data/ index.dat worddddd.dat newIndex.dat >> "$filename"
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
make index >> /dev/null

#Test 6: input-validation - invalid file name 
echo "Input validation: invalid file name"
echo >> "$filename"
echo "Input validation: invalid file name" >> "$filename"
./index ../data/ indexes indexes newIndex.dat >> "$filename"
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
make index >> /dev/null

#Test 7: input-validation - invalid file name 
echo "Input validation: invalid newFile name"
echo >> "$filename"
echo "Input validation: invalid newFile name" >> "$filename"
./index ../data/ index.dat index.dat newIndex >> "$filename"
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
make index >> /dev/null

echo "All input validation tests successful!"
echo >> "$filename"
echo "All input validation tests successful!" >> "$filename"
echo




#Test 8: just make index.dat 
echo "Test 1: Creating justindex.dat from ../data/"
echo >> "$filename"
echo "Test 1: Creating justindex.dat from ../data/" >> "$filename"
./index ../data3/ justindex.dat >> "$filename"
if [ $? -ne 0 ]; then
echo Test failed
	exit 1
fi
echo

# Clean up
make leavedat >> /dev/null
if [ $? -ne 0 ]; then
echo Clean except data failed
	exit 1
fi
make index >> /dev/null


#Test 9: make index.dat and newIndex.dat, then diff
echo "Test 2: Creating index.dat from ../data3/ and newIndex.dat from index.dat"
echo >> "$filename"
echo "Test 2: Creating index.dat from ../data3/ and newIndex.dat from index.dat" >> "$filename"
./index ../data3/ index.dat index.dat newIndex.dat >> "$filename"
if [ $? -ne 0 ]; then
echo Test failed
	exit 1
fi
echo

#Test 10: diff
echo "Test 2 continued: diff index.dat newIndex.dat"
echo >> "$filename"
echo "Test 2 continued: diff index.dat newIndex.dat" >> "$filename"
diff index.dat newIndex.dat >> "$filename"

# Clean up
make leavedat >> /dev/null
if [ $? -ne 0 ]; then
echo Clean except data failed
	exit 1
fi

