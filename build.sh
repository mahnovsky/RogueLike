#!/bin/bash

#SOURCES="$(find $PWD/code/src -name '*.cpp' -type f)"
SOURCES="$PWD/code/linux_startup.cpp"

INCLUDES="-I $PWD/code/include -I $PWD/code/src"
LIBS="-L/usr/lib/x86_64-linux-gnu -lX11 -lSDL2 -lpthread -lstdc++ -lGL -ldl -lm"
CLANG_COMPILLER="clang"
echo "$INCLUDES"
echo "$SOURCES"
echo "---------- start build ------------"

if [ ! -d "./obj" ]; then
    mkdir obj
fi

GLEW_SRC="$PWD/code/src/glew.c"

pushd obj
    $CLANG_COMPILLER -c $GLEW_SRC $INCLUDES -g
    $CLANG_COMPILLER -c $SOURCES $INCLUDES -std=c++11 -g -D_DEBUG -DGLM_ENABLE_EXPERIMENTAL
popd

OBJECTS="$(ls -d $PWD/obj/*.o)"

pushd bin
    $CLANG_COMPILLER -o game $OBJECTS $LIBS
popd
