#!/bin/bash
rm ./build/test.nn
./build/neural-network --file build/test.nn --network "3;10,Identity;1"

epsilon=0.0001

for i in {1..1000}
do
    input="1,1,1"
    expected="3"

    for i in {1..10}
    do
        a=$(( $RANDOM % 10 + 1 ))
        b=$(( $RANDOM % 10 + 1 ))
        c=$(( $RANDOM % 10 + 1 ))
        d=$(( $a+$b+$c ))
        input="$input;$a,$b,$c"
        expected="$expected;$d"
    done

    ./build/neural-network --file build/test.nn --input "$input" --expected "$expected" -p $epsilon
done
