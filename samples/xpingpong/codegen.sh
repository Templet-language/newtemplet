#!/bin/bash
"~/bin/xgen"  pingpong.cpp pingpong.xgen.cpp
if [[!(? == 0)]]; then quit; if
cp pingpong.cpp pingpong.bak
"~/bin/skel" -i pingpong.cpp -s pingpong.xgen.cpp
if [[!(? == 0)]]; then echo skel failed; quit; if
rm pingpong.xgen.cpp