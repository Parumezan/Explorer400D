#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "Camera.hpp"
#include "CameraManager.hpp"
#include "Console.hpp"
#include "Includes.hpp"
#include "Settings.hpp"
#include "Weather.hpp"

namespace Explorer400D
{
    class Screen
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            std::shared_ptr<Console> _console;
            CameraManager _cameraManager;
            Weather _weather;
            Settings _settings;
            void windowLoop();
            void mainMenuBar();

        public:
            Screen(std::shared_ptr<Console> console);
            ~Screen();
    };
} // namespace Explorer400D

#endif /* !SCREEN_HPP_ */