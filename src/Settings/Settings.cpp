#include "Settings.hpp"

using namespace Explorer400D;

void Settings::setupStyle()
{
    // Clean Dark/Red style by ImBritish from ImThemes
    ImGuiStyle &style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 1.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.729411780834198f, 0.7490196228027344f, 0.7372549176216125f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08627451211214066f, 0.08627451211214066f, 0.08627451211214066f, 0.9399999976158142f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5400000214576721f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 0.4000000059604645f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 0.6700000166893005f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 0.6523605585098267f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 0.6700000166893005f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.3803921639919281f, 0.3803921639919281f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.5411764979362488f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 0.4000000059604645f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 0.6705882549285889f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2705882489681244f, 0.2705882489681244f, 0.2705882489681244f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3529411852359772f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 0.3294117748737335f, 0.3294117748737335f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.4862745106220245f, 0.4862745106220245f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.4862745106220245f, 0.4862745106220245f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.2196078449487686f, 0.2196078449487686f, 0.2196078449487686f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2901960909366608f, 0.2901960909366608f, 0.2901960909366608f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1490196138620377f, 0.06666667014360428f, 0.06666667014360428f, 0.9700000286102295f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.4039215743541718f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.364705890417099f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2627451121807098f, 0.6352941393852234f, 0.8784313797950745f, 0.4377682209014893f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4666666686534882f, 0.1843137294054031f, 0.1843137294054031f, 0.9656652212142944f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

bool Settings::fileExists(std::string &path, std::string message)
{
    if (!std::filesystem::exists(path)) {
        spdlog::warn("{} '{}' doesn't exist", message, path);
        path = ".";
        return false;
    }
    return true;
}

void Settings::setSetting(const std::string &key, const json &value)
{
    this->_settings[key] = value;
}

json Settings::getSetting(const std::string &key)
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

    if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
        file >> this->_settings;
    else
        this->_settings = json::object();

    file.close();
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

void Settings::moduleSettingsLoad()
{
    json obj = nullptr;

    (obj = this->getSetting("Explorer400D::Settings::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;

    (obj = this->getSetting("ImPlot::UseLocalTime")) != nullptr ? ImPlot::GetStyle().UseLocalTime = obj.get<bool>() : ImPlot::GetStyle().UseLocalTime = false;
    (obj = this->getSetting("ImPlot::UseISO8601")) != nullptr ? ImPlot::GetStyle().UseISO8601 = obj.get<bool>() : ImPlot::GetStyle().UseISO8601 = false;
    (obj = this->getSetting("ImPlot::Use24HourClock")) != nullptr ? ImPlot::GetStyle().Use24HourClock = obj.get<bool>() : ImPlot::GetStyle().Use24HourClock = false;
}

void Settings::moduleSettingsSave()
{
    this->setSetting("Explorer400D::Settings::State", this->state);

    this->setSetting("ImPlot::UseLocalTime", ImPlot::GetStyle().UseLocalTime);
    this->setSetting("ImPlot::UseISO8601", ImPlot::GetStyle().UseISO8601);
    this->setSetting("ImPlot::Use24HourClock", ImPlot::GetStyle().Use24HourClock);
}