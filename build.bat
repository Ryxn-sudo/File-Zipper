@echo off
REM Change to the correct directory first
cd /d "%~dp0"

REM Set MSYS2 environment
SET PATH=C:\msys64\ucrt64\bin;%PATH%

REM Set full paths for required executables
SET CMAKE_EXE=C:\msys64\ucrt64\bin\cmake.exe
SET MAKE_EXE=C:\msys64\ucrt64\bin\mingw32-make.exe

REM Create and enter build directory
if not exist build mkdir build
cd build

REM Clean previous build
if exist CMakeCache.txt del /f /q CMakeCache.txt
if exist CMakeFiles rmdir /s /q CMakeFiles

REM Run CMake and make with full paths
"%CMAKE_EXE%" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
"%MAKE_EXE%" VERBOSE=1

REM Show error if make fails
if errorlevel 1 (
    echo Build failed! Please check if MSYS2 is installed correctly.
    echo Expected path: C:\msys64\ucrt64\bin\mingw32-make.exe
)

pause
