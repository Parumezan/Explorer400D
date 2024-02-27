#include "Console.hpp"

using namespace Explorer400D;

Console::Console()
{
    this->_info = 0;
    this->_warn = 0;
    this->_error = 0;
    this->stopSequence = false;
}

Console::~Console()
{
    this->_history.push_back(this->getHistoryCount());
    std::cout << this->getHistoryCount() << std::endl;
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
    std::pair<std::string, std::string> info = {"E4-Info: ", msg};
    this->putHistory(info);
}

void Console::warn(std::string const &msg)
{
    this->_warn++;
    std::pair<std::string, std::string> warn = {"E4-Warn: ", msg};
    this->putHistory(warn);
}

void Console::error(std::string const &msg)
{
    this->_error++;
    std::pair<std::string, std::string> error = {"E4-Error: ", msg};
    this->putHistory(error);
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

void Console::putHistory(std::pair<std::string, std::string> const &msg)
{
    // TODO: Remake this for iso 8601
    this->_history.push_back(this->getLocalTime() + " " + msg.first + msg.second);

    // TODO: Remove this and remake print function
    switch (msg.first[3]) {
    case 'I':
        this->print(ConsoleType::INFO, msg.second);
        break;
    case 'W':
        this->print(ConsoleType::WARN, msg.second);
        break;
    case 'E':
        this->print(ConsoleType::ERROR, msg.second);
        break;
    }
}

void Console::saveHistory(std::string settingsPath)
{
    std::string historyPath = settingsPath + "/lastlog.txt";
    std::ofstream historyFile(historyPath);

    try {
        if (historyFile.is_open()) {
            for (auto &msg : this->_history)
                historyFile << msg << std::endl;
            historyFile << this->getHistoryCount() << std::endl;
            historyFile.close();
        } else
            throw std::runtime_error("Unable to open history file");
    } catch (std::exception &error) {
        this->error(error.what());
    }
    this->info("History saved in " + historyPath);
}

std::string Console::getHistoryCount()
{
    return "Console: " + std::to_string(this->_info) + " info, " + std::to_string(this->_warn) + " warn, " + std::to_string(this->_error) + " error";
}

std::string Console::getIsoTime()
{
    // Get current time
    auto now = std::chrono::system_clock::now();

    // Convert to time_t
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    // Convert to tm struct
    std::tm tm_time = *std::localtime(&time);

    // Convert to ISO 8601
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%dT%H:%M:%S");
    std::string iso8601_time = oss.str();

    return iso8601_time;
}

void Console::frameLoop()
{
    ImGui::Begin("Console", &this->state);
    for (auto &msg : this->_history) {
        // separate time, type and message, they are separated by a space
        std::string time = msg.substr(0, msg.find(' '));
        std::string type = msg.substr(msg.find(' ') + 1, msg.find(' ', msg.find(' ') + 1) - msg.find(' ') - 1);
        std::string message = msg.substr(msg.find(' ', msg.find(' ') + 1) + 1);

        // display time
        ImGui::Text("%s", time.c_str());

        // display type
        ImGui::SameLine();
        if (type == "E4-Info:")
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", type.c_str());
        else if (type == "E4-Warn:")
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s", type.c_str());
        else if (type == "E4-Error:")
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", type.c_str());
        else
            ImGui::Text("%s", type.c_str());

        // display message
        ImGui::SameLine();
        ImGui::Text("%s", message.c_str());
    }
    ImGui::End();
}