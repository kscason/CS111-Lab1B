#!/bin/bash

touch a
touch b
touch c

./simpsh --rdonly a --rdonly b --rdonly c

if [ $? -ne 0 ]; then
  echo "Error opening files for read only"
  exit 1
fi

./simpsh --wronly a --wronly b --wronly c

if [ $? -ne 0 ]; then
  echo "Error opening files for write only"
  exit 1
fi

./simpsh --rdonly NOTAREALFILE 2> /dev/null

if [ $? -eq 0 ]; then
  echo "Should exit with nonzero status for failure to open read-only file"
  exit 1
fi

./simpsh --wronly NOTAREALFILE 2> /dev/null

if [ $? -eq 0 ]; then
  echo "Should exit with nonzero status for failure to open write-only file"
  exit 1
fi

./simpsh --rdonly a --verbose --rdonly b | grep -e "\-\-rdonly b" > /dev/null

if [ $? -ne 0 ]; then
  echo "Should print out commands after the verbose and not before"
  exit 1
fi

rm a
rm b
rm c

echo "Successfully passed all tests!"
exit 0