#!/bin/bash

rm -rf CMakeFiles/
rm -rf CMakeCache.txt
rm -rf Release/
mkdir Release
cp ../ali_face_engine_sdk/Windows/x86/* Release

cmake -G "Visual Studio 14"
