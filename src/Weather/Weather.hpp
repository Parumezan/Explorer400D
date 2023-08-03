#ifndef WEATHER_HPP_
#define WEATHER_HPP_

#include "ChildConsole.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    typedef struct Location_s {
            std::string name;
            float latitude;
            float longitude;
            std::string country;
    } Location_t;

    class Weather : public Frame, public ChildConsole
    {
        private:
            CURL *_curl;
            std::string _latitude;
            std::string _longitude;
            std::vector<Location_t> _locations;
            Location_t _selectedLocation;

            std::vector<double> _cloudCover;
            std::vector<double> _cloudCoverLow;
            std::vector<double> _cloudCoverMid;
            std::vector<double> _cloudCoverHigh;
            std::vector<double> _visibility;
            std::vector<double> _time;

            std::string fetchData(std::string url);
            void fetchLocationSearch(char location[64]);
            void fetchWeather();
            void drawClouds();

        public:
            Weather(std::shared_ptr<Console> console);
            ~Weather();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !WEATHER_HPP_ */