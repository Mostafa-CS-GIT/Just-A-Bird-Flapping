@echo off
echo Building Flappy Bird...

set MSYS2=D:\APPS\msys64\ucrt64
set GPP=%MSYS2%\bin\g++.exe

"%GPP%" -g "%~dp0FlappyBird.cpp" ^
    -o "%~dp0FlappyBird.exe" ^
    -I"%MSYS2%\include" ^
    -L"%MSYS2%\lib" ^
    -lglfw3 -lglew32 -lopengl32 -lgdi32 -luser32

if %ERRORLEVEL% == 0 (
    echo.
    echo Build successful!  Running FlappyBird.exe ...
    echo.
    "%~dp0FlappyBird.exe"
) else (
    echo.
    echo Build FAILED. Check the errors above.
    pause
)