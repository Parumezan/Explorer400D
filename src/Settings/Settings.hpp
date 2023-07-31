#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "ChildConsole.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class Settings : public Frame, public ChildConsole
    {
        private:
            std::string _settingsPath;

        public:
            Settings(std::shared_ptr<Console> console);
            ~Settings();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !SETTINGS_HPP_ */