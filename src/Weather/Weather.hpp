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

    typedef struct PlotClouds_s {
            std::vector<double> cloudCover;
            std::vector<double> cloudCoverLow;
            std::vector<double> cloudCoverMid;
            std::vector<double> cloudCoverHigh;
            std::vector<double> visibility;
            std::vector<double> time;
    } PlotClouds_t;

    class Weather : public Frame, public ChildConsole
    {
        private:
            CURL *_curl;
            std::string _latitude;
            std::string _longitude;
            std::vector<Location_t> _locations;
            Location_t _selectedLocation;
            PlotClouds_t _plotClouds;

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