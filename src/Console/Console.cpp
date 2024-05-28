#include "Console.hpp"

using namespace Explorer400D;

Console::Console(Settings &settings)
{
    this->_settings = &settings;
}

void Console::moduleInit()
{
}

void Console::moduleLoop()
{
    ImGui::Begin("Console");

    ImGui::End();
}

void Console::moduleLoadSettings()
{
    nlohmann::json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::Console::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
}

void Console::moduleSaveSettings()
{
    this->_settings->setSetting("Explorer400D::Console::State", this->state);
}