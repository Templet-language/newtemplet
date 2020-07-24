"../../bin/xgen"  diamond.hpp diamond.xgen.hpp
if %ERRORLEVEL% NEQ 0 exit /B
copy diamond.hpp diamond.bak
"../../bin/skel" -i diamond.hpp -s diamond.xgen.hpp
if %ERRORLEVEL% NEQ 0 echo skel.exe failed
del diamond.xgen.hpp