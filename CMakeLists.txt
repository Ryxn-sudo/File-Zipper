cmake_minimum_required(VERSION 3.15)
project(FileZipper)

# Add compiler definitions to fix standard library issues
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D__USE_MINGW_ANSI_STDIO)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")

# Remove vcpkg reference and update package finding
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Add ImGui source files directly
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(FileZipper 
    FileZipper.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(FileZipper PRIVATE
    imgui
    imgui/backends
)

target_link_libraries(FileZipper PRIVATE
    opengl32
    glfw
    GLEW::GLEW
)

# Add Windows-specific libraries
if(WIN32)
    target_link_libraries(FileZipper PRIVATE
        ole32
        uuid
        gdi32
        dwmapi
    )
endif()
