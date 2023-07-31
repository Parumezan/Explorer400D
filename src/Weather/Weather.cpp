#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(std::shared_ptr<Console> console) : ChildConsole(console)
{
}

Weather::~Weather()
{
}

void Weather::frameLoop()
{
    ImGui::Begin("Weather");
    ImGui::End();
}