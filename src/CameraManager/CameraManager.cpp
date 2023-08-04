#include "CameraManager.hpp"

using namespace Explorer400D;

CameraManager::CameraManager(std::shared_ptr<Console> console) : ChildConsole(console)
{

    this->_console->info("CameraManager module started");
}

CameraManager::~CameraManager()
{
}

void CameraManager::autoDetectCameras()
{
    auto cameraPair = gphoto2pp::autoDetect();

    this->_model = cameraPair.first;
    this->_port = cameraPair.second;
    this->_console->info("Camera detected: " + this->_model + " on port " + this->_port);
}

void CameraManager::connectCamera()
{
    auto cameraWrapper = gphoto2pp::CameraWrapper(this->_model, this->_port);

    this->_console->info("Camera connected: " + this->_model + " on port " + this->_port);
    this->_console->info("Camera summary: " + cameraWrapper.getSummary());
}

void CameraManager::frameLoop()
{
    ImGui::Begin("Camera Manager", &this->state);

    if (ImGui::Button("Auto Detect & Connect")) {
        try {
            this->autoDetectCameras();
            this->connectCamera();
        } catch (const std::exception &error) {
            this->_console->error(error.what());
        }
    }

    ImGui::End();
}