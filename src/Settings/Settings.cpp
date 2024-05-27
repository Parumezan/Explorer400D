#include "Settings.hpp"

using namespace Explorer400D;

void Settings::setSetting(const std::string &key, const nlohmann::json &value)
{
    this->_settings[key] = value;
}

nlohmann::json Settings::getSetting(const std::string &key)
{
    return this->_settings[key];
}

void Settings::deleteSetting(const std::string &key)
{
    this->_settings.erase(key);
}

void Settings::moduleInit()
{
    std::ifstream file("settings.json");

    if (file.is_open()) {
        file >> this->_settings;
        file.close();
    } else {
        this->_settings = nlohmann::json::object();
    }
}

void Settings::moduleLoop()
{
    ImGui::Begin("Settings", &this->state);

    ImGui::SeparatorText("Plot Time Display");
    ImGui::Checkbox("Local Time", &ImPlot::GetStyle().UseLocalTime);
    ImGui::SameLine();
    ImGui::Checkbox("ISO 8601", &ImPlot::GetStyle().UseISO8601);
    ImGui::SameLine();
    ImGui::Checkbox("24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);

    ImGui::End();
}

void Settings::moduleClose()
{
    std::ofstream file("settings.json");

    file << this->_settings.dump(4); // 4 spaces for indentation
    file.close();
}

void Settings::moduleLoadSettings()
{
    nlohmann::json obj = nullptr;

    (obj = this->getSetting("ImPlot::UseLocalTime")) != nullptr ? ImPlot::GetStyle().UseLocalTime = obj.get<bool>() : ImPlot::GetStyle().UseLocalTime = false;
    (obj = this->getSetting("ImPlot::UseISO8601")) != nullptr ? ImPlot::GetStyle().UseISO8601 = obj.get<bool>() : ImPlot::GetStyle().UseISO8601 = false;
    (obj = this->getSetting("ImPlot::Use24HourClock")) != nullptr ? ImPlot::GetStyle().Use24HourClock = obj.get<bool>() : ImPlot::GetStyle().Use24HourClock = false;
}

void Settings::moduleSaveSettings()
{
    this->setSetting("ImPlot::UseLocalTime", ImPlot::GetStyle().UseLocalTime);
    this->setSetting("ImPlot::UseISO8601", ImPlot::GetStyle().UseISO8601);
    this->setSetting("ImPlot::Use24HourClock", ImPlot::GetStyle().Use24HourClock);
}