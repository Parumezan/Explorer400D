#ifndef WEATHER_HPP_
#define WEATHER_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"
#include "WebFetch/WebFetch.hpp"

namespace Explorer400D
{
    class Weather : public Module
    {
        private:
            Settings *_settings;
            WebFetch *_webFetch;

            void _setLocation();

        public:
            Weather(Settings &settings, WebFetch &webFetch);
            ~Weather() = default;

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
} // namespace Explorer400D

#endif /* !WEATHER_HPP_ */
