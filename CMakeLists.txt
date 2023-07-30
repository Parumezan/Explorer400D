################# CONFIG ##################

#init Cmake
cmake_minimum_required(VERSION 3.25)
project(Explorer400D LANGUAGES CXX VERSION 0.1)

## Set project properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

############## DEPENDENCIES ###############

## Find dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_library(GPHOTO2_LIB gphoto2)

## Include ImGui "libray"
set(IMGUI_DIR lib/imgui)
include_directories(${IMGUI_DIR})
list(APPEND IMGUI
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
)

## Include ImGuiFileDialog "libray"
set(ImGuiFileDialog_DIR lib/ImGuiFileDialog)
include_directories(${ImGuiFileDialog_DIR})
list(APPEND IMGUI
    ${ImGuiFileDialog_DIR}/ImGuiFileDialog.cpp
)

## Include dirent "libray" if on Windows
if(WIN32)
    set(DIRENT_DIR lib/dirent)
    include_directories(${DIRENT_DIR})
endif()

## Include stb "libray" (used by ImGuiFileDialog and for Explorer400D)
set(STB_DIR lib/stb)
include_directories(${STB_DIR})

## Include libevent
list(APPEND EXTRA_LIBS
    -levent
)

################## BUILD ##################

## Set executable name
set(EXEC Explorer400D)

## Set root path
set(EXEC_ROOT src/)

## Include directories
include_directories(
    ${EXEC_ROOT}/Camera
    ${EXEC_ROOT}/CameraManager
    ${EXEC_ROOT}/ChildConsole
    ${EXEC_ROOT}/Console
    ${EXEC_ROOT}/Exception
    ${EXEC_ROOT}/Frame
    ${EXEC_ROOT}/Screen
    ${EXEC_ROOT}/
)

## List source files
set(EXEC_SRC
    ${EXEC_ROOT}/Camera/Camera.cpp
    ${EXEC_ROOT}/CameraManager/CameraManager.cpp
    ${EXEC_ROOT}/ChildConsole/ChildConsole.cpp
    ${EXEC_ROOT}/Console/Console.cpp
    ${EXEC_ROOT}/Exception/Exception.cpp
    ${EXEC_ROOT}/Frame/Frame.cpp
    ${EXEC_ROOT}/Screen/Screen.cpp
    ${EXEC_ROOT}/Explorer400D.cpp
)

## Create executable
add_executable(${EXEC} ${IMGUI} ${EXEC_SRC})

## Link libraries
target_link_libraries(${EXEC}
    OpenGL
    glfw
    ${EXTRA_LIBS}
    ${GPHOTO2_LIB}
)

## Set output directory
set_target_properties(${EXEC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set_target_properties(${EXEC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

## Install executable
install(TARGETS ${EXEC})