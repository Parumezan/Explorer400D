#ifndef IMGLOADER_HPP_
#define IMGLOADER_HPP_

#include "Console.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    enum ImgType {
        RAW,
        STB
    };

    typedef struct ImgSpec_s {
            int iso;
            int shutter;
            int aperture;
            int focal;
    } ImgSpec_t;

    class ImgLoader
    {
        private:
            std::shared_ptr<Console> _console;
            std::string _path;
            int _width;
            int _height;
            int _channels;
            unsigned char *_stbData;
            LibRaw _rawProcessor;
            libraw_processed_image_t *_rawData;
            GLuint _glTexture;
            ImTextureID _imguiTexture;
            ImgType _type;
            ImgSpec_t _spec;

            void loadRaw();
            void loadStb();

        public:
            ImgLoader(std::shared_ptr<Console> console, std::string path);
            ~ImgLoader();
            ImTextureID getImguiTexture();
            int getImgWidth();
            int getImgHeight();
            ImgSpec_t getImgSpec();
            ImgType getImgType();
    };
} // namespace Explorer400D

#endif /* !IMGLOADER_HPP_ */