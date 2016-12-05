#!/bin/bash
TEST_NUM=$1
if [[ -z "$TEST_NUM" ]]; then
	FILENAME=_test.txt
else
	FILENAME=_test$TEST_NUM.txt
fi
g++ -g -Wall -std=c++11 code/*.cpp -o _lab_code
./_lab_code tests/$FILENAME
