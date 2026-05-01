@echo off
"D:\APPS\msys64\ucrt64\bin\g++.exe" -g "%~dp0MyFirstOpenGLProg.CPP" -o "%~dp0MyFirstOpenGLProg.exe" -I"D:\APPS\msys64\ucrt64\include" -L"D:\APPS\msys64\ucrt64\lib" -lglfw3 -lglew32 -lopengl32
pause