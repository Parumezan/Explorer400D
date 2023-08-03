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

    typedef struct PlotTemperature_s {
            std::vector<double> temp;
            std::vector<double> tempApparent;
            std::vector<double> tempDewPoint;
            std::vector<double> time;
    } PlotTemperature_t;

    typedef struct PlotPrecipitation_s {
            std::vector<double> preciProbability;
            std::vector<double> time;
    } PlotPrecipitation_t;

    typedef struct PlotWind_s {
            std::vector<double> windGusts10m;
            std::vector<double> windSpeed10m;
            std::vector<double> windSpeed80m;
            std::vector<double> windSpeed120m;
            std::vector<double> windSpeed180m;
            std::vector<double> time;
    } PlotWind_t;

    class Weather : public Frame, public ChildConsole
    {
        private:
            CURL *_curl;
            std::string _latitude;
            std::string _longitude;
            std::vector<Location_t> _locations;
            Location_t _selectedLocation;
            std::string _selectedLocationName;
            PlotClouds_t _plotClouds;
            PlotTemperature_t _plotTemp;
            PlotPrecipitation_t _plotPreci;
            PlotWind_t _plotWind;
            bool _isFit;

            std::string fetchData(std::string url);
            void fetchLocationSearch(char location[64]);
            void fetchClouds(std::string url, std::string settings);
            void fetchTemperature(std::string url, std::string settings);
            void fetchPrecipitation(std::string url, std::string settings);
            void fetchWind(std::string url, std::string settings);
            void drawClouds();
            void drawTemp();
            void drawPreci();
            void drawWind();
            void fetchWeather();
            void setLocation();

        public:
            Weather(std::shared_ptr<Console> console);
            ~Weather();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !WEATHER_HPP_ */