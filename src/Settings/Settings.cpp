#include "Settings.hpp"

using namespace Explorer400D;

Settings::Settings(std::shared_ptr<Console> console) : ChildConsole(console)
{
}

Settings::~Settings()
{
}

void Settings::frameLoop()
{
    ImGui::Begin("Settings", &this->state);
    ImGui::End();
}