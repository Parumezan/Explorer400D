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

    enum class WeatherSearchState {
        NONE,
        FOUND,
        SELECTED,
        BAD
    };

    class Weather : public Module
    {
        private:
            Settings *_settings;
            WebFetch *_webFetch;

            Location _location;
            std::vector<Location> _locations;
            bool _locationSet = false;

            WeatherSearchState _searchState = WeatherSearchState::NONE;
            std::string _searchStateText = "";

            std::vector<WeatherGraph> _weatherGraphs;

            bool _initGraphs = false;
            bool _fetchable = false;
            bool _autoFetch = false;
            int _sliderTime = 0;
            int _forecastDays = 3;
            std::time_t _lastFetch;

            void _setLocation();

            void _fetchLocationSearch(char location[64]);
            void _fetchWeatherData();

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