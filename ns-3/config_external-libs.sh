#!/bin/bash
cp ./external-libs/AI-Toolbox/lib/libAIToolboxMDP.so /usr/lib # ubuntu 20.04 uses /usr/lib , fedora 33 /usr/lib64 , neither recognises /usr/local/lib* 

cp -r ./external-libs/AI-Toolbox/include /usr/local/

#Eigen
cp -r ./external-libs/Eigen/include /usr/local/
