#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "CameraManager.hpp"
#include "Console.hpp"
#include "ImgViewer.hpp"
#include "Includes.hpp"
#include "Moon.hpp"
#include "Settings.hpp"
#include "Weather.hpp"

namespace Explorer400D
{
    class Screen
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            std::shared_ptr<Console> _console;
            std::shared_ptr<Settings> _settings;
            std::shared_ptr<ImgViewer> _imgViewer;
            std::shared_ptr<CameraManager> _cameraManager;
            std::shared_ptr<Weather> _weather;
            std::shared_ptr<Moon> _moon;
            std::string _imguiIniPath;
            bool _resetWidgets;

            std::map<std::string, bool> _widgetStatus;
            void loadConfig();
            void windowLoop();
            void saveWidgetsStatus();
            void resetWidgetsStatus();
            void mainMenuBar();

        public:
            Screen(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings, std::shared_ptr<ImgViewer> imgViewer, std::shared_ptr<CameraManager> cameraManager, std::shared_ptr<Weather> weather, std::shared_ptr<Moon> moon);
            ~Screen();
    };
} // namespace Explorer400D

#endif /* !SCREEN_HPP_ */