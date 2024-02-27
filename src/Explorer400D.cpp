#include "CameraManager.hpp"
#include "Console.hpp"
#include "ImgViewer.hpp"
#include "Includes.hpp"
#include "Moon.hpp"
#include "Screen.hpp"
#include "Settings.hpp"
#include "Weather.hpp"

using namespace Explorer400D;

std::shared_ptr<Console> console = std::make_shared<Console>();

void signalHandler(int signum)
{
    std::cout << std::endl;
    console->warn("Interrupt signal (" + std::to_string(signum) + ") received");
    console->stopSequence = true;
}

int main(void)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::shared_ptr<Settings> settings = std::make_shared<Settings>(console);
    std::shared_ptr<ImgViewer> imgViewer = std::make_shared<ImgViewer>(console, settings);
    std::shared_ptr<CameraManager> cameraManager = std::make_shared<CameraManager>(console, settings, imgViewer);
    std::shared_ptr<Weather> weather = std::make_shared<Weather>(console, settings);
    std::shared_ptr<Moon> moon = std::make_shared<Moon>(console, settings);
    Screen screen(console, settings, imgViewer, cameraManager, weather, moon);
    return 0;
}