#include "Console.hpp"

using namespace Explorer400D;

Console::Console(Settings &settings)
{
    this->_settings = &settings;

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("explogs.txt", true);
    fileSink->set_level(spdlog::level::trace);

    auto streamSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(this->_consoleLogStream);
    streamSink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks = {consoleSink, fileSink, streamSink};
    auto combinedLogger = std::make_shared<spdlog::logger>("console", begin(sinks), end(sinks));
    spdlog::register_logger(combinedLogger);
    spdlog::set_default_logger(combinedLogger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::trace);
}

void Console::moduleInit()
{
}

void Console::moduleLoop()
{
    ImGui::Begin("Console", &this->state);

    ImGui::Text(this->_consoleLogStream.str().c_str());

    ImGui::End();
}

void Console::moduleClose()
{
}

void Console::moduleSettingsLoad()
{
    json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::Console::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
}

void Console::moduleSettingsSave()
{
    this->_settings->setSetting("Explorer400D::Console::State", this->state);
}