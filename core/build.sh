#!/bin/sh
mkdir -p build && cd build
cmake .. --target core test
make -j4
cd -
./build/test
