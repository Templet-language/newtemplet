"../../bin/xgen"  pingpong.cpp pingpong.xgen.cpp
if %ERRORLEVEL% NEQ 0 exit /B
copy pingpong.cpp pingpong.bak
"../../bin/skel" -i pingpong.cpp -s pingpong.xgen.cpp
if %ERRORLEVEL% NEQ 0 echo skel.exe failed
del pingpong.xgen.cpp