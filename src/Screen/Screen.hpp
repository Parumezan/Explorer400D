#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "CameraManager/CameraManager.hpp"
#include "CameraOMatic/CameraOMatic.hpp"
#include "Console/Console.hpp"
#include "Includes.hpp"
#include "Map/Map.hpp"
#include "Module/Module.hpp"
#include "SelfGuiding/SelfGuiding.hpp"
#include "Settings/Settings.hpp"
#include "Weather/Weather.hpp"
#include "WebFetch/WebFetch.hpp"

namespace Explorer400D
{
    class Screen
    {
        private:
            GLFWwindow *_window;
            ApplicationState _state = ApplicationState::LOADING;
            std::vector<Module *> _modules;

            Settings _settings;
            WebFetch _webFetch;
            Console _console;
            CameraManager _cameraManager;
            CameraOMatic _cameraOMatic;
            Map _map;
            Weather _weather;
            SelfGuiding _selfGuiding;

            std::atomic<bool> _stopSignal = false;
            std::atomic<bool> _threadRunning = false;
            std::atomic<InitThreadState> _threadState = InitThreadState::INIT;
            std::thread _moduleThread;
            std::atomic<size_t> _moduleIndex = 0;

            // void _setWindowIcon();

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
