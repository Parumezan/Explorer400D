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
            bool _isCameraSelected;
            std::string _selectedCameraName;
            std::string _selectedCameraPort;
            std::string _cameraSummary;

            std::string _folderPath;
            bool _folderDialog;
            bool _initFolderPath;

            int _inputTime;
            int _inputShots;
            int _saveInputTime;
            std::time_t _lastCapture;

            void _listConnectedCameras();
            void _connectCamera();
            void _disconnectCamera();
            void _takePicture();

            void _setupCamera();
            void _folderPart();
            void _picturePart();

        public:
            CameraManager(Settings &settings);
            ~CameraManager() = default;

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
} // namespace Explorer400D

#endif /* !CAMERAMANAGER_HPP_ */
