#!/bin/bash
SCRIPT_DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
BUILD_DIR=$SCRIPT_DIR/../build

FILE=$BUILD_DIR/mnist.nn

if [ "$1" == "refresh" ]
then
    rm $FILE
fi

$BUILD_DIR/mnist-test --file $FILE -m $SCRIPT_DIR/../mnist-data
