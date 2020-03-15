#!/bin/bash

### This script is used to run clang-format on the source files.

# Directory containing this bash script.
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

# Directories
readonly SRC_DIR=$DIR/src
readonly INCLUDE_DIR=$DIR/include
readonly TEST_DIR=$DIR/tests

# files
readonly DOT_CLANG_FORMAT=$DIR/.clang-format

delete_file_if_exists () {
    if [ -e $1 ]; then
        rm -f $1
    fi
}

format () {
    cd $1
    delete_file_if_exists ./.clang-format
    cp $DOT_CLANG_FORMAT ./.clang-format
    find -name '*.cpp' -o -name '*.hh' -o -name '*.hpp' -o -name '*.h' | xargs clang-format -i
    rm -f ./.clang-format
}

delete_file_if_exists $DOT_CLANG_FORMAT

wget --quiet https://raw.githubusercontent.com/actor-framework/actor-framework/master/.clang-format

# Format the source files.
format $SRC_DIR
format $INCLUDE_DIR
format $TEST_DIR

exit 0

