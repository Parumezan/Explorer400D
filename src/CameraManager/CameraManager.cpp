#include "CameraManager.hpp"

using namespace Explorer400D;

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::frameLoop()
{
    ImGui::Begin("Camera Manager", &this->state);
    ImGui::End();
}