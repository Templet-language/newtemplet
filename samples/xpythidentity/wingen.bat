"../../bin/xgen"  pythidentity.cpp pythidentity.xgen.cpp
if %ERRORLEVEL% NEQ 0 exit /B
copy pythidentity.cpp pythidentity.bak
"../../bin/skel" -i pythidentity.cpp -s pythidentity.xgen.cpp
if %ERRORLEVEL% NEQ 0 echo skel.exe failed
del pythidentity.xgen.cpp