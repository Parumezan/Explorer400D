#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stdio.h>

// ImGui & OpenGL & GLFW
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// ImGuiFileDialog
#include "ImGuiFileDialog.h"

// Dirent
#ifdef _WIN32
    #include "dirent.h"
#endif

// gphoto2
#include <gphoto2/gphoto2.h>

#endif /* !INCLUDES_HPP_ */