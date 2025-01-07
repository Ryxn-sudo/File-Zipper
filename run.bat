@echo off
REM Set DLL search path
SET PATH=C:\msys64\ucrt64\bin;%PATH%

REM Copy required DLLs if they don't exist
if not exist build\libglfw3.dll copy C:\msys64\ucrt64\bin\libglfw3.dll build\
if not exist build\libgcc_s_seh-1.dll copy C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll build\
if not exist build\libstdc++-6.dll copy C:\msys64\ucrt64\bin\libstdc++-6.dll build\
if not exist build\libwinpthread-1.dll copy C:\msys64\ucrt64\bin\libwinpthread-1.dll build\
if not exist build\libglew32.dll copy C:\msys64\ucrt64\bin\libglew32.dll build\

REM Run the application
cd build
start FileZipper.exe
