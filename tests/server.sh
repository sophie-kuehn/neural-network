#!/bin/bash
SCRIPT_DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
BUILD_DIR=$SCRIPT_DIR/../build

HOST=localhost
PORT=$1
FILE=$BUILD_DIR/server.nn
NETWORK="3;10,Identity;1"
EPSILON=0.0001
BATCHES=1000
CHECKS_PER_BATCH=10

if [ "$1" == "refresh" ]
then
    rm $FILE
fi

trap "kill 0" EXIT
$BUILD_DIR/neural-network -f $FILE -n "$NETWORK" -s $PORT &
sleep 1

for i in $( seq 1 $BATCHES )
do
    INPUT="1,1,1"
    EXPECTED="3"

    for i in $( seq 1 $CHECKS_PER_BATCH )
    do
        a=$(( $RANDOM % 10 + 1 ))
        b=$(( $RANDOM % 10 + 1 ))
        c=$(( $RANDOM % 10 + 1 ))
        d=$(( $a+$b+$c ))
        INPUT="$INPUT;$a,$b,$c"
        EXPECTED="$EXPECTED;$d"
    done

    echo "$INPUT\_$EXPECTED\_$EPSILON" | nc $HOST $PORT
done

echo "1,1,1" | nc $HOST $PORT
