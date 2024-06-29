#ifndef IMGVIEWER_HPP_
#define IMGVIEWER_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"

namespace Explorer400D
{
    class ImgViewer : public Module
    {
        private:
            std::string _imgPath;
            ImgType _imgType;

            int _width;
            int _height;
            int _channels;
            unsigned char *_stbData;
            LibRaw _rawProcessor;
            libraw_processed_image_t *_rawData;
            GLuint _glTexture;
            ImTextureID _imguiTexture;

            float _iso;
            float _shutter;
            float _aperture;
            float _focalLength;

            std::atomic<bool> _isLoaded;
            std::atomic<bool> _isFailed;

            void _freeImg();

            void _loadRaw();
            void _loadStb();

        public:
            ImgViewer(std::string imgPath);
            ~ImgViewer();

            void loadImg();

            void moduleLoop() override;
    };
}

#endif /* !IMGVIEWER_HPP_ */
