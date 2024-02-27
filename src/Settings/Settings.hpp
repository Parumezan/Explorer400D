#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "Console.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class Settings : public Frame
    {
        private:
            std::shared_ptr<Console> _console;
            std::string _settingsPath;
            sqlite3 *_db;

            void initSettings();
            void initDatabase();
            void closeDatabase();
            void saveWidgetStatus(std::map<std::string, bool> widgetStatus);

        public:
            Settings(std::shared_ptr<Console> console);
            ~Settings();
            void saveConfig();
            void saveSettings(std::map<std::string, bool> widgetStatus);
            void setSettingsPath(std::string &path);

            std::string getSettingsPath();
            std::map<std::string, bool> getWidgetsStatus();

            std::string getSettingsVar(std::string varName);
            void setSettingsVar(std::string varName, std::string varValue);

            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !SETTINGS_HPP_ */