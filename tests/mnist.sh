#!/bin/bash
SCRIPT_DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
BUILD_DIR=$SCRIPT_DIR/../build

if [ "$1" == "refresh" ]
then
    rm $BUILD_DIR/mnist.nn
fi

$BUILD_DIR/neural-network --file $BUILD_DIR/mnist.nn -m $SCRIPT_DIR/../mnist-data
