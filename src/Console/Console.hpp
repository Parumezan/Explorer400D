#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"

namespace Explorer400D
{
    class Console : public Module
    {
        private:
            Settings *_settings;
            std::ostringstream _consoleLogStream;

        public:
            Console(Settings &settings);
            ~Console() = default;

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
}

#endif /* !CONSOLE_HPP_ */
