# File Zipper

A file compression tool using Huffman coding with GUI interface.

## Prerequisites

1. MSYS2 (for MinGW and required packages)
2. C++17 compatible compiler
3. CMake

## Installation Steps

### 1. Install MSYS2
1. Download MSYS2 from https://www.msys2.org/
2. Run the installer
3. Open MSYS2 terminal and update:
```bash
pacman -Syu
```

### 2. Install Required Packages
Run these commands in MSYS2 terminal:
```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-make
pacman -S mingw-w64-ucrt-x86_64-glfw
pacman -S mingw-w64-ucrt-x86_64-glew
```

### 3. Set Up Environment Variables
1. Open Windows System Properties
2. Click "Environment Variables"
3. Add to System PATH:
   - C:\msys64\mingw64\bin
   - C:\msys64\usr\bin
   - C:\msys64\ucrt64\bin


### 4. Download Dear ImGui
1. Clone or download ImGui from: https://github.com/ocornut/imgui
2. Extract to project directory in 'imgui' folder
3. Ensure these files exist:
   - imgui/*.cpp
   - imgui/*.h
   - imgui/backends/imgui_impl_glfw.cpp
   - imgui/backends/imgui_impl_opengl3.cpp

## Building the Project

### Method 1: Using build script
1. Double-click `build.bat`
2. Check build/Release for the executable

### Method 2: Manual build
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

## Usage

1. Run FileZipper.exe
2. Select input file using "Browse Input"
3. Choose output location using "Browse Output"
4. Click "Compress" to compress or "Decompress" to decompress

## Troubleshooting

### Common Issues:
1. CMake not found:
   - Verify MSYS2 installation
   - Check PATH environment variables

2. Build fails:
   - Ensure all required packages are installed
   - Check ImGui files are in correct location
   - Verify compiler compatibility

3. GUI doesn't start:
   - Check OpenGL dependencies
   - Update graphics drivers
   - Verify GLFW and GLEW installation
