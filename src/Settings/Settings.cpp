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

    ImGui::SeparatorText("Plot Time Display");
    ImGui::Checkbox("Local Time", &ImPlot::GetStyle().UseLocalTime);
    ImGui::SameLine();
    ImGui::Checkbox("ISO 8601", &ImPlot::GetStyle().UseISO8601);
    ImGui::SameLine();
    ImGui::Checkbox("24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);

    ImGui::End();
}