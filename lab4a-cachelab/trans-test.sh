#!/bin/bash
make clean > /dev/null
make > /dev/null
# echo 32x32
# ./test-trans -M 32 -N 32
echo 64x64
./test-trans -M 64 -N 64
# echo 67x61
# ./test-trans -M 61 -N 67
