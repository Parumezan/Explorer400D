#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"

namespace Explorer400D
{
    class Settings : public Module
    {
        private:
            json _settings;

        public:
            Settings() = default;
            ~Settings() = default;

            void setupStyle();

            void setSetting(const std::string &key, const json &value);
            json getSetting(const std::string &key);
            void deleteSetting(const std::string &key);

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
} // namespace Explorer400D

#endif /* !SETTINGS_HPP_ */