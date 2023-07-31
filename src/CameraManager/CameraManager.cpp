#include "CameraManager.hpp"

using namespace Explorer400D;

// GP_OK (error to check)

CameraManager::CameraManager(std::shared_ptr<Console> console) : ChildConsole(console)
{
    this->_console->info("Initializing Camera Manager...");
    // this->_context = gp_context_new();
    this->_console->info("Camera Manager initialized");
    int value = this->searchCameras();
    this->_console->info("Searched for cameras (" + std::to_string(value) + ")");
}

CameraManager::~CameraManager()
{
}

void CameraManager::frameLoop()
{
    ImGui::Begin("Camera Manager", &this->state);
    ImGui::End();
}

int CameraManager::searchCameras()
{
    // this->_console->info("Searching for cameras...");
    // int value = gp_list_reset(this->_cameraList);
    // this->_console->info("Reset camera list (" + std::to_string(value) + ")");
    // return gp_camera_autodetect(this->_cameraList, this->_context);
    return 0;
}