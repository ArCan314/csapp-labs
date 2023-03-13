#! /usr/bin/bash

make clean
make csim
for((i=1;i<=20;i++));
do
    for((j=1;j<=20;j++));
    do
        # for((k=1;k<=$(expr 62 - $i); k++));
        for((k=1;k<=$(expr 1); k++));
        do
        echo "($i,$j,$k)"
        echo trans
        ./csim -s $i -E $j -b $k -t ./traces/trans.trace > csim.out.txt
        # echo csim-ref
        ./csim-ref -s $i -E $j -b $k -t ./traces/trans.trace > csim-ref.out.txt
        # echo diff
        diff csim.out.txt csim-ref.out.txt
        if (($? != 0)); then
            echo "break at ($i,$j,$k)"
            break 3
        fi

        echo dave
        ./csim -s $i -E $j -b $k -t ./traces/dave.trace > csim.out.txt
        # echo csim-ref
        ./csim-ref -s $i -E $j -b $k -t ./traces/dave.trace > csim-ref.out.txt
        # echo diff
        diff csim.out.txt csim-ref.out.txt
        if (($? != 0)); then
            echo "break at ($i,$j,$k)"
            break 3
        fi

        echo long
        time ./csim -s $i -E $j -b $k -t ./traces/long.trace > csim.out.txt
        # echo csim-ref
        time ./csim-ref -s $i -E $j -b $k -t ./traces/long.trace > csim-ref.out.txt
        # echo diff
        diff csim.out.txt csim-ref.out.txt
        if (($? != 0)); then
            echo "break at ($i,$j,$k)"
            break 3
        fi

        echo yi
        ./csim -s $i -E $j -b $k -t ./traces/yi.trace > csim.out.txt
        # echo csim-ref
        ./csim-ref -s $i -E $j -b $k -t ./traces/yi.trace > csim-ref.out.txt
        # echo diff
        diff csim.out.txt csim-ref.out.txt
        if (($? != 0)); then
            echo "break at ($i,$j,$k)"
            break 3
        fi

        echo yi2
        ./csim -s $i -E $j -b $k -t ./traces/yi2.trace > csim.out.txt
        # echo csim-ref
        ./csim-ref -s $i -E $j -b $k -t ./traces/yi2.trace > csim-ref.out.txt
        # echo diff
        diff csim.out.txt csim-ref.out.txt
        if (($? != 0)); then
            echo "break at ($i,$j,$k)"
            break 3
        fi
        done
    done
done

rm csim.out.txt csim-ref.out.txt .csim_results