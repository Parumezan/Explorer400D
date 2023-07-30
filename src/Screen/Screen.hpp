#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "Camera.hpp"
#include "CameraManager.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class Screen
    {
        private:
            CameraManager _cameraManager;
            std::shared_ptr<GLFWwindow> _window;
            void windowLoop();
            void mainMenuBar();

        public:
            Screen();
            ~Screen();
    };
} // namespace Explorer400D

#endif /* !SCREEN_HPP_ */