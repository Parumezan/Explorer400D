#include "ImgLoader.hpp"

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Explorer400D;

ImgLoader::ImgLoader(std::shared_ptr<Console> console, std::string path) : _console(console), _path(path)
{
    std::string extension = path.substr(path.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    this->_rawData = NULL;
    this->_stbData = NULL;
    this->_glTexture = 0;

    try {
        for (std::string rawExtension : ImgTypeRaw) {
            if (extension == rawExtension) {
                this->_console->info("Loading RAW image...");
                this->_type = RAW;
                this->loadRaw();
                return;
            }
        }
        for (std::string stbExtension : ImgTypeStb) {
            if (extension == stbExtension) {
                this->_console->info("Loading STB image...");
                this->_type = STB;
                this->loadStb();
                return;
            }
        }
        this->_console->warn("Failed to load image: " + this->_path);
    } catch (std::exception &error) {
        this->_console->error(error.what());
    }
}

ImgLoader::~ImgLoader()
{
    if (this->_rawData != NULL)
        this->_rawProcessor.dcraw_clear_mem(this->_rawData);
    if (this->_stbData != NULL)
        stbi_image_free(this->_stbData);
    if (this->_glTexture != 0)
        glDeleteTextures(1, &this->_glTexture);
}

void ImgLoader::loadStb()
{
    this->_stbData = stbi_load(this->_path.c_str(), &this->_width, &this->_height, &this->_channels, STBI_rgb_alpha);

    if (this->_stbData == NULL) {
        this->_console->throwException("Failed to load image: " + this->_path);
        return;
    }
    glGenTextures(1, &this->_glTexture);
    glBindTexture(GL_TEXTURE_2D, this->_glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_width, this->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->_stbData);
    this->_imguiTexture = (void *)(intptr_t)this->_glTexture;
    this->_console->info("Loaded STB image: " + this->_path);
}

void ImgLoader::loadRaw()
{
    int ret = 0;

    if ((ret = this->_rawProcessor.open_file(this->_path.c_str())) != LIBRAW_SUCCESS) {
        this->_console->throwException("Failed to open image: " + this->_path);
        return;
    }

    if ((ret = this->_rawProcessor.unpack()) != LIBRAW_SUCCESS) {
        this->_console->throwException("Failed to unpack image: " + this->_path);
        return;
    }

    if ((ret = this->_rawProcessor.dcraw_process()) != LIBRAW_SUCCESS) {
        this->_console->throwException("Failed to process image: " + this->_path);
        return;
    }

    this->_rawData = this->_rawProcessor.dcraw_make_mem_image(&ret);

    if (ret != LIBRAW_SUCCESS) {
        this->_console->throwException("Failed to make image: " + this->_path);
        return;
    }

    this->_width = this->_rawData->width;
    this->_height = this->_rawData->height;
    this->_channels = this->_rawData->colors;

    glGenTextures(1, &this->_glTexture);
    glBindTexture(GL_TEXTURE_2D, this->_glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->_width, this->_height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->_rawData->data);
    this->_imguiTexture = (void *)(intptr_t)this->_glTexture;
    this->_console->info("Loaded RAW image: " + this->_path);
}

ImTextureID ImgLoader::getImguiTexture()
{
    return this->_imguiTexture;
}

int ImgLoader::getImgWidth()
{
    return this->_width;
}

int ImgLoader::getImgHeight()
{
    return this->_height;
}

ImgSpec_t ImgLoader::getImgSpec()
{
    return this->_spec;
}

ImgType ImgLoader::getImgType()
{
    return this->_type;
}