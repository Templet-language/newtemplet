#!/bin/bash
~/bin/xgen pingpong.cpp pingpong.xgen.cpp
if [[ $? -ne 0 ]]; then exit; fi
cp pingpong.cpp pingpong.bak
~/bin/skel -i pingpong.cpp -s pingpong.xgen.cpp
if [[ $? -ne 0 ]]; then echo "skel failed"; fi
rm pingpong.xgen.cpp