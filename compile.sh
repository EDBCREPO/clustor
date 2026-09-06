#!/bin/bash

mkdir -p ./build

if [ ! -d "./build/CMakeFiles" ] ; then
   ( cd ./build; cmake cmake .. )
fi

( cd ./build; make ); 

if [ ! $? -eq 0 ]; then
    echo "exit error"; exit;
fi

./build/main