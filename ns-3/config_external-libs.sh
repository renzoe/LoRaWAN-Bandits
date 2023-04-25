#!/bin/bash
set -x #echo on
ARCH=$(dpkg --print-architecture)
LIB=""
echo Architecture: $ARCH

if [ "$ARCH" == "arm64" ]; then
    echo "Using the arm64 compiled AI-Toolbox libraries"
    LIB="_arm64" 
else
    echo "Using the x86_64 compiled AI-Toolbox libraries (If this is not your architecture you will have problems)"
fi

cp ./external-libs/AI-Toolbox/lib$LIB/libAIToolboxMDP.so  /usr/lib  # ubuntu 20.04 uses /usr/lib , fedora 33 /usr/lib64 , neither recognises /usr/local/lib* 

cp -r ./external-libs/AI-Toolbox/include /usr/local/

#Eigen
cp -r ./external-libs/Eigen/include /usr/local/
