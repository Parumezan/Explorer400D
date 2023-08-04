#ifndef CAMERAMANAGER_HPP_
#define CAMERAMANAGER_HPP_

#include "ChildConsole.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class CameraManager : public Frame, public ChildConsole
    {
        private:
            std::string _model;
            std::string _port;
            void autoDetectCameras();
            void connectCamera();

        public:
            CameraManager(std::shared_ptr<Console> console);
            ~CameraManager();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !CAMERAMANAGER_HPP_ */