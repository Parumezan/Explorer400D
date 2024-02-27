#ifndef CAMERAMANAGER_HPP_
#define CAMERAMANAGER_HPP_

#include "Console.hpp"
#include "Frame.hpp"
#include "ImgLoader.hpp"
#include "ImgViewer.hpp"
#include "Includes.hpp"
#include "Settings.hpp"

namespace Explorer400D
{
    class CameraManager : public Frame
    {
        private:
            std::shared_ptr<Console> _console;
            std::shared_ptr<Settings> _settings;
            std::shared_ptr<ImgViewer> _imgViewer;
            std::string _model;
            std::string _port;
            std::shared_ptr<gphoto2pp::CameraWrapper> _camera;
            std::shared_ptr<gphoto2pp::CameraListWrapper> _cameraList;
            std::shared_ptr<gphoto2pp::CameraFileWrapper> _cameraFileWrapper;
            std::shared_ptr<gphoto2pp::CameraFilePathWrapper> _cameraFilePathWrapper;
            std::string _cameraSummary;
            bool _showCameraInfo;

            std::shared_ptr<ImgLoader> _imgPreview;

            void autoDetectCameras();
            void connectCamera();
            void takePicture();

            std::string _selectedCameraName;
            void setCamera();
            void folderPart();
            void picturePart();

            void initFolder();

            std::string _folderPath;
            bool _initFolderPath;
            bool _folderDialog;

            int _inputTime;
            int _inputShots;
            int _saveInputTime;
            std::time_t _lastCapture;

        public:
            CameraManager(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings, std::shared_ptr<ImgViewer> imgViewer);
            ~CameraManager();
            void frameLoop() override;
            void saveSettings() override;
    };
} // namespace Explorer400D

#endif /* !CAMERAMANAGER_HPP_ */