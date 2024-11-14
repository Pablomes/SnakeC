@echo off

set opts=-FC -GR- -EHa- -W4 -nologo -Zi -D_CRT_SECURE_NO_WARNINGS
::cl %opts% main.c -FeCSnake.exe /link -incremental:no
gcc main.c -Wall -Wextra -o CSnake.exe