#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

// Include C++ standard libraries
#include <memory>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <vector>

// Include GLAD, GLFW
#include <glad/glad.h>
// Space between includes is intentional
#include <GLFW/glfw3.h>

// Include ImGui, ImPlot, ImGui File Dialog
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

// Include spdlog
#include "spdlog/spdlog.h"
// Space between includes is intentional
#include "spdlog/sinks/stdout_color_sinks.h"

// Include nlohmann/json
#include "nlohmann/json.hpp"

// Include curl
#include <curl/curl.h>

// Include gphoto2
#ifdef interface
    #undef interface
#endif
#include <gphoto2/gphoto2-camera.h>

namespace Explorer400D
{
    enum class ApplicationState {
        LOADING,
        RUNNING,
        STOPPING,
        STOPPED
    };

    enum class InitThreadState {
        INIT,
        SAVE
    };

    static const std::vector<std::string> ImgTypeRaw = {"arw", "cr2", "cr3", "dng", "nef", "nrw", "orf", "pef", "raf", "rw2", "srw"};
    static const std::vector<std::string> ImgTypeStb = {"jpg", "jpeg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic"};
} // namespace Explorer400D

#endif /* !INCLUDES_HPP_ */