#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(Settings &settings, WebFetch &webFetch) : _settings(&settings), _webFetch(&webFetch)
{
}

void Weather::_setLocation()
{
    static char latitude[32] = "", longitude[32] = "", inputLocation[64] = "";

    ImGui::InputText("Search a location", inputLocation, sizeof(inputLocation));

    if (ImGui::Button("Search")) {
        // Fetch the location
    }
}

void Weather::moduleInit()
{
}

void Weather::moduleLoop()
{
    ImGui::Begin("Weather", &this->state);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Set location", nodeFlags)) {
        ImGui::TreePop();
    }

    ImGui::SeparatorText("Weather Data");

    if (ImGui::TreeNodeEx("Temperature", nodeFlags)) {
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Cloud Cover", nodeFlags)) {
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Precipitation", nodeFlags)) {
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Wind", nodeFlags)) {
        ImGui::TreePop();
    }

    ImGui::End();
}

void Weather::moduleClose()
{
}

void Weather::moduleSettingsLoad()
{
    nlohmann::json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::Weather::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
}

void Weather::moduleSettingsSave()
{
    this->_settings->setSetting("Explorer400D::Weather::State", this->state);
}