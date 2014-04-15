#!/bin/bash

if [ ! -d "Debug" ]; then
echo "mkdir Debug folder..."
mkdir Debug
fi

if [ ! -d "Release" ]; then
echo "mkdir Release folder..."
mkdir Release
fi

SRC=`find ./Softing -name "*.dll"`
cp ${SRC} Debug
cp ${SRC} Release
echo "copy Softing DLL to Debug & Release..."

