#include "Map.hpp"

using namespace Explorer400D;

Map::Map(Settings &settings, WebFetch &webFetch) : _settings(&settings), _webFetch(&webFetch)
{
}

void Map::moduleInit()
{
}

void Map::moduleLoop()
{
    ImGui::Begin("Map", &this->state);

    ImGui::BeginDisabled(this->_uiDisable);

    ImGui::EndDisabled();

    ImGui::End();
}

void Map::moduleClose()
{
}

void Map::moduleSettingsLoad()
{
    nlohmann::json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::Map::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
}

void Map::moduleSettingsSave()
{
    this->_settings->setSetting("Explorer400D::Map::State", this->state);
}