#ifndef WEATHER_HPP_
#define WEATHER_HPP_

#include "Includes.hpp"
#include "Map/Map.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"
#include "WeatherGraph/WeatherGraph.hpp"
#include "WebFetch/WebFetch.hpp"

namespace Explorer400D
{
    static const std::vector<int> ForecastDays = {1, 3, 7, 14, 16};

    class Weather : public Module
    {
        private:
            Settings *_settings;
            WebFetch *_webFetch;

            Location _location;
            std::vector<WeatherGraph> _weatherGraphs;

            void _setLocation();

            void _fetchLocationSearch(char location[64]);

        public:
            Weather(Settings &settings, WebFetch &webFetch);
            ~Weather() = default;

            void setLocation(Location location);

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
} // namespace Explorer400D

#endif /* !WEATHER_HPP_ */
