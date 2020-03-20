#!/bin/bash

# Directory containing this bash script
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

readonly PREVIOUS_DIRECTORY=$(pwd)

readonly ROOT_DIR=$DIR/..

cd $ROOT_DIR/external

cd ./jaeger-client-cpp

if [ ! -d ./build ] || [ "$1" == "rebuild" ]; then
    rm -rf ./build
    
    mkdir ./build

    cd ./build

    cmake -DBUILD_TESTING=OFF ..

    make -j$(nproc)

    cd ..
fi

cd $PREVIOUS_DIRECTORY

exit 0

