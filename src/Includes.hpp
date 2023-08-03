#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <memory>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <vector>

// ImGui & OpenGL & GLFW
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// ImPlot
#include "implot.h"

// ImGuiFileDialog
#include "ImGuiFileDialog.h"

// Dirent
#ifdef _WIN32
    #include "dirent.h"
#endif

// gphoto2
#include <gphoto2/gphoto2.h>

// curl
#include <curl/curl.h>

// nlohmann/json
#include <nlohmann/json.hpp>

// Explorer400D
#define HISTORY_SIZE 2048

namespace Explorer400D
{
    enum ConsoleType {
        INFO,
        WARN,
        ERROR
    };
} // namespace Explorer400D

#endif /* !INCLUDES_HPP_ */