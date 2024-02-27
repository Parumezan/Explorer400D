#ifndef IMGVIEWER_HPP_
#define IMGVIEWER_HPP_

#include "Console.hpp"
#include "Frame.hpp"
#include "ImgLoader.hpp"
#include "Includes.hpp"
#include "Settings.hpp"

namespace Explorer400D
{
    static const std::vector<std::string> ImgViewerModes = {"Folder", "Image"};

    typedef struct ImgViewerFile_s {
            std::string name;
            std::string path;
            std::string extension;
            std::shared_ptr<ImgLoader> img;
    } ImgViewerFile_t;

    class ImgViewer : public Frame
    {
        private:
            std::shared_ptr<Console> _console;
            std::shared_ptr<Settings> _settings;
            std::shared_ptr<ImgLoader> _image;
            std::string _mode;
            std::string _folderPath;
            std::string _imgPath;
            std::string _lastMode;
            bool _folderDialog;
            bool _initFolderPath;
            bool _loadImageFiles;
            std::vector<ImgViewerFile_t> _imageFiles;

            void switchMode(std::string mode);
            void initFolder();
            void folderMode();
            void imageMode();
            // void loadImageByPath();

        public:
            ImgViewer(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings);
            ~ImgViewer();
            void setImage(std::string imgPath);
            void reloadImages();
            std::string getFolderPath();
            void frameLoop() override;
            void initFrame() override;
            void saveSettings() override;
    };
} // namespace Explorer400D

#endif /* !IMGVIEWER_HPP_ */
