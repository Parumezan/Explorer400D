#ifndef CAMERAMANAGER_HPP_
#define CAMERAMANAGER_HPP_

#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class CameraManager : public Frame
    {
        public:
            CameraManager();
            ~CameraManager();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !CAMERAMANAGER_HPP_ */