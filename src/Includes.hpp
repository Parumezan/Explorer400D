#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#include <GLFW/glfw3.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <memory>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
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

// gphoto2pp
#include <gphoto2pp/camera_abilities_list_wrapper.hpp>
#include <gphoto2pp/camera_capture_type_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>
#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/camera_widget_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/window_widget.hpp>

// curlpp
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

// nlohmann/json
#include <nlohmann/json.hpp>

// libraw
#include <libraw/libraw.h>

// sqlite3
#include <sqlite3.h>

// Explorer400D
#define HISTORY_SIZE 2048

namespace Explorer400D
{
    enum ConsoleType {
        INFO,
        WARN,
        ERROR
    };

    static const std::vector<std::string> WidgetNames = {"Console", "Settings", "CameraManager", "ImgViewer", "Weather", "Moon"};

    static const std::vector<std::string> ImgTypeRaw = {"arw", "cr2", "cr3", "dng", "nef", "nrw", "orf", "pef", "raf", "rw2", "srw"};
    static const std::vector<std::string> ImgTypeStb = {"jpg", "jpeg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic"};

} // namespace Explorer400D

#endif /* !INCLUDES_HPP_ */