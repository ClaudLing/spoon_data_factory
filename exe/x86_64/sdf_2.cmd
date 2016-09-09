@echo off
set /p userid=Enter userid, please.
set /p spoonid=Enter spoonid, please.
sdf.exe -a114.215.121.190 -p9203 -s1024 -i1000 -d200 -u%userid% -c%spoonid% -n10 -m0 -l1