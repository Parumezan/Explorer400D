#ifndef CAMERAMANAGER_HPP_
#define CAMERAMANAGER_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"

namespace Explorer400D
{
    class CameraManager : public Module
    {
        private:
            Settings *_settings;

            GPContext *_context;
            CameraList *_cameraList;
            Camera *_camera;
            GPPortInfoList *_portInfoList;

            size_t _numCameras;
            std::string _selectedCameraName;
            std::string _selectedCameraPort;
            std::string _cameraSummary;

            std::string _folderPath;

            int _inputTime;
            int _inputShots;
            int _saveInputTime;
            std::time_t _lastCapture;

        public:
            CameraManager(Settings &settings);
            ~CameraManager() = default;

            void listConnectedCameras();
            void connectCamera();
            void takePicture();

            void setupCamera();
            void folderPart();
            void picturePart();

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleLoadSettings() override;
            void moduleSaveSettings() override;
    };
} // namespace Explorer400D

#endif /* !CAMERAMANAGER_HPP_ */
