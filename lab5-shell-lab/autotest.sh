#!/bin/bash

failed=0
mkdir test_data
dir=./test_data

for i in $(seq 1 9)
do
    make test0$i > $dir/test0$i.res.txt
    make rtest0$i > $dir/rtest0$i.res.txt
done

for i in $(seq 0 6)
do
    make test1$i > $dir/test1$i.res.txt
    make rtest1$i > $dir/rtest1$i.res.txt
done
