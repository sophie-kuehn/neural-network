#!/bin/bash
SCRIPT_DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
BUILD_DIR=$SCRIPT_DIR/../build

if [ "$1" == "refresh" ]
then
    rm $BUILD_DIR/test.nn
fi

$BUILD_DIR/neural-network --file $BUILD_DIR/test.nn --network "3;10,Identity;1"

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

    $BUILD_DIR/neural-network --file $BUILD_DIR/test.nn --input "$input" --expected "$expected" -p $epsilon
done
