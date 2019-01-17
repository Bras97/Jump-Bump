#!/bin/bash
rm -r compiled
mkdir compiled
cmake src
make 
mv server compiled/server
rm Makefile
rm CMakeCache.txt
rm cmake_install.cmake
echo Compilation completed successfully
