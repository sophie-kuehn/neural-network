#!/bin/bash
SCRIPT_DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
BUILD_DIR=$SCRIPT_DIR/../build

FILE=$BUILD_DIR/test.nn
EPSILON=0.0001
NETWORK="3;10,Identity;1"
BATCHES=1000
CHECKS_PER_BATCH=10

if [ "$1" == "refresh" ]
then
    rm $FILE
fi

$BUILD_DIR/neural-network --file $FILE --network "$NETWORK"

for i in $( seq 1 $BATCHES )
do
    CHECKS="1,1,1;3;$EPSILON"

    for i in $( seq 1 $CHECKS_PER_BATCH )
    do
        a=$(( $RANDOM % 10 + 1 ))
        b=$(( $RANDOM % 10 + 1 ))
        c=$(( $RANDOM % 10 + 1 ))
        d=$(( $a+$b+$c ))
        CHECKS="${CHECKS}_$a,$b,$c;$d;$EPSILON"
    done

    $BUILD_DIR/neural-network --file $FILE --checks "$CHECKS"
done

$BUILD_DIR/neural-network --file $FILE --checks "1,1,1"
