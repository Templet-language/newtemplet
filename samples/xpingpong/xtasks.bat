"../../bin/xgen"  xtasks.cpp xtasks.xgen.cpp
copy xtasks.cpp xtasks.bak
"../../bin/skel" -i xtasks.cpp -s xtasks.xgen.cpp
