#include "Console.hpp"

using namespace Explorer400D;

Console::Console()
{
    this->_isQuiet = true;
    this->_info = 0;
    this->_warn = 0;
    this->_error = 0;
}

Console::Console(bool isQuiet)
{
    this->_isQuiet = isQuiet;
    this->_info = 0;
    this->_warn = 0;
    this->_error = 0;
}

Console::~Console()
{
    if (!this->_isQuiet)
        std::cout << "Console: " << this->_info << " info, " << this->_warn << " warn, " << this->_error << " error" << std::endl;
}

std::string Console::getLocalTime()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    std::string hour = (ltm->tm_hour < 10) ? "0" + std::to_string(ltm->tm_hour) : std::to_string(ltm->tm_hour);
    std::string min = (ltm->tm_min < 10) ? "0" + std::to_string(ltm->tm_min) : std::to_string(ltm->tm_min);
    std::string sec = (ltm->tm_sec < 10) ? "0" + std::to_string(ltm->tm_sec) : std::to_string(ltm->tm_sec);
    return hour + ":" + min + ":" + sec;
}

void Console::info(std::string const &msg)
{
    this->_info++;
    std::string info = this->getLocalTime() + " E4-Info: " + msg;
    this->putHistory(info);

    if (!this->_isQuiet)
        this->print(ConsoleType::INFO, msg);
}

void Console::warn(std::string const &msg)
{
    this->_warn++;
    std::string warn = this->getLocalTime() + " E4-Warn: " + msg;
    this->putHistory(warn);

    if (!this->_isQuiet)
        this->print(ConsoleType::WARN, msg);
}

void Console::error(std::string const &msg)
{
    this->_error++;
    std::string error = this->getLocalTime() + " E4-Error: " + msg;
    this->putHistory(error);

    if (!this->_isQuiet)
        this->print(ConsoleType::ERROR, msg);
}

Exception Console::throwException(std::string const &msg)
{
    this->error(msg);
    this->_exception = std::make_unique<Exception>(msg);
    return *this->_exception;
}

void Console::print(ConsoleType type, std::string const &msg)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (type) {
    case ConsoleType::INFO:
        SetConsoleTextAttribute(hConsole, 10);
        std::cout << this->getLocalTime() + " E4-Info: " << msg << std::endl;
        break;
    case ConsoleType::WARN:
        SetConsoleTextAttribute(hConsole, 14);
        std::cout << this->getLocalTime() + " E4-Warn: " << msg << std::endl;
        break;
    case ConsoleType::ERROR:
        SetConsoleTextAttribute(hConsole, 12);
        std::cerr << this->getLocalTime() + " E4-Error: " << msg << std::endl;
        break;
    }
    SetConsoleTextAttribute(hConsole, 15);
#else
    switch (type) {
    case ConsoleType::INFO:
        std::cout << this->getLocalTime() + " \033[32mE4-Info:\033[0m " << msg << std::endl;
        break;
    case ConsoleType::WARN:
        std::cout << this->getLocalTime() + " \033[33mE4-Warn:\033[0m " << msg << std::endl;
        break;
    case ConsoleType::ERROR:
        std::cerr << this->getLocalTime() + " \033[31mE4-Error:\033[0m " << msg << std::endl;
        break;
    }
#endif
}

void Console::putHistory(std::string const &msg)
{
    if (this->_history.size() > HISTORY_SIZE) {
        this->_history.erase(this->_history.begin());
        this->_history.push_back(msg);
    } else {
        this->_history.push_back(msg);
    }
}

void Console::frameLoop()
{
    ImGui::Begin("Console", &this->state);
    for (auto &msg : this->_history) {
        // Separate Time and type and message
        std::string time = msg.substr(0, 8);
        std::string type = msg.substr(9, 8);
        std::string message = msg.substr(18);

        // Display time
        ImGui::Text("%s", time.c_str());

        // Display type
        ImGui::SameLine();
        if (type == "E4-Info:")
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", type.c_str());
        else if (type == "E4-Warn:")
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s", type.c_str());
        else if (type == "E4-Error:")
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", type.c_str());
        else
            ImGui::Text("%s", type.c_str());

        // Display message
        ImGui::SameLine();
        ImGui::Text("%s", message.c_str());
    }
    ImGui::End();
}