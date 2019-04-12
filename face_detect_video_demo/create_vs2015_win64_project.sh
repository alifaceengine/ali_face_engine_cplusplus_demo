#!/bin/bash

rm -rf CMakeFiles/
rm -rf CMakeCache.txt
rm -rf Release/
mkdir Release
cp ../../ali_face_engine_sdk/c++/Windows/x64/* Release

cmake -G "Visual Studio 14 Win64"
