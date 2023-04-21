#!/bin/bash
SCRIPT_DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
BUILD_DIR=$SCRIPT_DIR/../build

HOST=localhost
PORT=8000

trap "kill 0" EXIT
$BUILD_DIR/neural-network -s $PORT &
sleep 1
echo text1 | netcat $HOST $PORT
sleep 1
echo text2 | netcat $HOST $PORT
sleep 1
echo text3 | netcat $HOST $PORT
