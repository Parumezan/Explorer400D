#include "ImgViewer.hpp"

using namespace Explorer400D;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImgViewer::ImgViewer(std::string imgPath) : _imgPath(imgPath)
{
    this->_imgType = ImgType::UNKNOWN;
    this->_isLoaded = false;
    this->_isFailed = true;

    this->_rawData = NULL;
    this->_stbData = NULL;
    this->_glTexture = 0;

    std::string extension = this->_imgPath.substr(this->_imgPath.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    for (std::string rawExtension : ImgTypeRaw)
        if (extension == rawExtension)
            this->_imgType = ImgType::RAW;

    for (std::string stbExtension : ImgTypeStb)
        if (extension == stbExtension)
            this->_imgType = ImgType::STB;

    if (this->_imgType == ImgType::UNKNOWN)
        spdlog::warn("Unknown image {} type", this->_imgPath);
}

ImgViewer::~ImgViewer()
{
    this->_freeImg();
}

void ImgViewer::_freeImg()
{
    if (this->_rawData != NULL)
        this->_rawProcessor.dcraw_clear_mem(this->_rawData);
    if (this->_stbData != NULL)
        stbi_image_free(this->_stbData);
    if (this->_glTexture != 0)
        glDeleteTextures(1, &this->_glTexture);
}

void ImgViewer::_loadRaw()
{
    int ret = 0;

    if ((ret = this->_rawProcessor.open_file(this->_imgPath.c_str())) != LIBRAW_SUCCESS) {
        spdlog::error("Failed to open RAW image {}", this->_imgPath);
        this->_isLoaded.store(true);
        return;
    }

    if ((ret = this->_rawProcessor.unpack()) != LIBRAW_SUCCESS) {
        spdlog::error("Failed to unpack RAW image {}", this->_imgPath);
        this->_isLoaded.store(true);
        return;
    }

    if ((ret = this->_rawProcessor.dcraw_process()) != LIBRAW_SUCCESS) {
        spdlog::error("Failed to process RAW image {}", this->_imgPath);
        this->_isLoaded.store(true);
        return;
    }

    this->_rawData = this->_rawProcessor.dcraw_make_mem_image(&ret);

    if (ret != LIBRAW_SUCCESS) {
        spdlog::error("Failed to make memory image for RAW image {}", this->_imgPath);
        this->_isLoaded.store(true);
        return;
    }

    this->_width = this->_rawData->width;
    this->_height = this->_rawData->height;
    this->_channels = this->_rawData->colors;

    // this->_iso = this->_rawProcessor.imgdata.other.iso_speed;
    // this->_shutter = this->_rawProcessor.imgdata.other.shutter;
    // this->_aperture = this->_rawProcessor.imgdata.other.aperture;
    // this->_focalLength = this->_rawProcessor.imgdata.other.focal_len;

    glGenTextures(1, &this->_glTexture);
    glBindTexture(GL_TEXTURE_2D, this->_glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->_width, this->_height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->_rawData->data);
    this->_imguiTexture = (ImTextureID)(intptr_t)this->_glTexture;

    this->_isFailed.store(false);
    this->_isLoaded.store(true);
}

void ImgViewer::_loadStb()
{
    this->_stbData = stbi_load(this->_imgPath.c_str(), &this->_width, &this->_height, &this->_channels, STBI_rgb_alpha);

    if (this->_stbData == NULL) {
        spdlog::error("Failed to load STB image {}", this->_imgPath);
        this->_isLoaded.store(true);
        return;
    }

    glGenTextures(1, &this->_glTexture);
    glBindTexture(GL_TEXTURE_2D, this->_glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_width, this->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->_stbData);
    this->_imguiTexture = (ImTextureID)(intptr_t)this->_glTexture;

    this->_isFailed.store(false);
    this->_isLoaded.store(true);
}

void ImgViewer::loadImg()
{
    if (this->_isLoaded) {
        this->_freeImg();
        this->_isLoaded.store(false);
        this->_isFailed.store(true);
    }

    switch (this->_imgType) {
    case ImgType::RAW:
        this->moduleThreadInit(std::bind(&ImgViewer::_loadRaw, this));
        break;
    case ImgType::STB:
        this->moduleThreadInit(std::bind(&ImgViewer::_loadStb, this));
        break;
    default:
        break;
    }
}

void ImgViewer::moduleLoop()
{
    if (!this->_isLoaded)
        return;

    if (this->_isFailed) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load image %s", this->_imgPath.c_str());
        return;
    }

    ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoGridLines;
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot(this->_imgPath.c_str(), plotSize, ImPlotFlags_CanvasOnly | ImPlotFlags_Equal)) {
        ImPlot::SetupAxes(0, 0, flags, flags);

        ImVec2 canvasSize = ImPlot::GetPlotPos();
        ImVec2 canvasMax = ImVec2(canvasSize.x + plotSize.x, canvasSize.y + plotSize.y);

        ImPlot::PushPlotClipRect();
        ImPlot::GetPlotDrawList()->AddRectFilled(canvasSize, canvasMax, IM_COL32_BLACK);
        ImPlot::PopPlotClipRect();

        ImPlot::PlotImage(this->_imgPath.c_str(), this->_imguiTexture, ImVec2(0, 0), ImVec2(this->_width, this->_height));

        ImPlot::EndPlot();
    }
}