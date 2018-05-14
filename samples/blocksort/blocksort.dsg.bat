"../../bin/gen" -design blocksort.cpp blocksort.dsg.cpp
"../../bin/skel" -i blocksort.cpp -s blocksort.dsg.cpp
del blocksort.dsg.cpp
