#!/bin/bash

num_lines=0
num_files=0
FILES=code/*

for f in $FILES; do
    num_files=$(($num_files + 1))      
    num_lines=$(($num_lines + $(cat $f | wc -l)))
done

echo "Number of files: $num_files"
echo "Total lines of code: $num_lines"
python -c "print('Average per file: ' + str($num_lines*1.0/$num_files))"
