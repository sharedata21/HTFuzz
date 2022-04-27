#!/bin/bash
MODE=$1

if [[ $MODE == "angora" ]]; then
    export CC=/angora/bin/angora-clang
    export CXX=/angora/bin/angora-clang++
    export ANGORA_TAINT_RULE_LIST=$PWD/zlib_abilist.txt
elif [[ $MODE == "tofuzz" ]]; then
    export CC=$HOME/yyp/xxFuzz/afl-clang-fast
    export CXX=$HOME/yyp/xxFuzz/afl-clang-fast++
elif [[ $MODE == "mopt" ]]; then
    export CC=$HOME/yyp/test/MOpt/afl-clang-fast
    export CXX=$HOME/yyp/test/MOpt/afl-clang-fast++
elif [[ $MODE == "mopt-seq" ]]; then
    export CC=$HOME/yyp/test/MOpt-seq/afl-clang-fast
    export CXX=$HOME/yyp/test/MOpt-seq/afl-clang-fast++
elif [[ $MODE == "afl-p" ]]; then
    export CC=$HOME/yyp/test/afl-p/afl-clang-fast
    export CXX=$HOME/yyp/test/afl-p/afl-clang-fast++
elif [[ $MODE == "normal" ]]; then
    export CC=gcc
    export CXX=g++
elif [[ $MODE == "asan" ]]; then
    export CC="gcc -fsanitize=address"
    export CXX="g++ -fsanitize=address"
fi

echo "start compiling $MODE ..."

chmod a+x code/configure
chmod a+x code/mkinstalldirs

rm -rf build_$MODE bin_$MODE
mkdir build_$MODE
cd build_$MODE

../code/configure --prefix=$PWD/../bin_$MODE --disable-shared

if [[ $MODE == "angora" ]]; then
    USE_TRACK=1 make -j8
    make install
    mv ../bin_$MODE/bin/magick ../bin_$MODE/bin/magick.taint
    make clean
    USE_FAST=1 make -j8
    make install
    mv ../bin_$MODE/bin/magick ../bin_$MODE/bin/magick.fast
else
    make
    make install
fi