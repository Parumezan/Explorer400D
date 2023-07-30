#ifndef CAMERAMANAGER_HPP_
#define CAMERAMANAGER_HPP_

#include "ChildConsole.hpp"
#include "Console.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class CameraManager : public Frame, public ChildConsole
    {
        private:
            GPContext *_context;
            CameraList *_cameraList;

        public:
            CameraManager(std::shared_ptr<Console> console);
            ~CameraManager();
            void frameLoop() override;
            int searchCameras();
    };
} // namespace Explorer400D

#endif /* !CAMERAMANAGER_HPP_ */