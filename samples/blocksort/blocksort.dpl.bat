"../../bin/gen" -deploy blocksort.cpp blocksort.dpl.cpp
"../../bin/skel" -i blocksort.cpp -s blocksort.dpl.cpp
del blocksort.dpl.cpp
