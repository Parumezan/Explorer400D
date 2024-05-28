#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "CameraManager/CameraManager.hpp"
#include "Console/Console.hpp"
#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"

namespace Explorer400D
{
    class Screen
    {
        private:
            GLFWwindow *_window;
            ApplicationState _state = ApplicationState::LOADING;
            std::vector<Module *> _modules;

            Settings _settings;
            Console _console;
            CameraManager _cameraManager;

            std::atomic<bool> _stopSignal = false;
            std::atomic<bool> _threadRunning = false;
            std::atomic<ThreadState> _threadState = ThreadState::INIT;
            std::thread _moduleThread;
            std::atomic<size_t> _moduleIndex = 0;

            void _screenRender();
            void _screenInit();
            void _screenLoop();
            void _screenClose();

        public:
            Screen();
            ~Screen();
    };
} // namespace Explorer400D

#endif /* !SCREEN_HPP_ */
