#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    // Space between includes is intentional
    #include <windows.h>
#endif

// Include C++ standard libraries
#include <memory>
#include <signal.h>
#include <sstream>
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
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Include nlohmann/json
#include "nlohmann/json.hpp"
using json = nlohmann::json;

// Include curl
#include <curl/curl.h>

// Include gphoto2
#ifdef interface
    #undef interface
#endif
#include <gphoto2/gphoto2-camera.h>

// libraw
#include <libraw/libraw.h>

// Setup separator for file paths
#ifdef _WIN32
    #define _PLAT_SEPARATOR "\\"
#else
    #define _PLAT_SEPARATOR "/"
#endif

// Setup different sizes for buffers
// Input Text Buffer Size
#define _INPUT_TEXT_SIZE 256

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

    enum class ImgType {
        UNKNOWN,
        RAW,
        STB
    };

    static const std::vector<std::string> ImgTypeRaw = {"arw", "cr2", "cr3", "dng", "nef", "nrw", "orf", "pef", "raf", "rw2", "srw"};
    static const std::vector<std::string> ImgTypeStb = {"jpg", "jpeg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic"};
} // namespace Explorer400D

#endif /* !INCLUDES_HPP_ */