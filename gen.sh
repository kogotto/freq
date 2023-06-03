#!/bin/bash

mkdir -p build;
cmake -B build/ -S . -DCMAKE_BUILD_TYPE=Release;
# cmake -B build/ -S . -DCMAKE_BUILD_TYPE=Debug;
ln -fs build/compile_commands.json .
