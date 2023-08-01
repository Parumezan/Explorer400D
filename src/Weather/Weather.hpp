#ifndef WEATHER_HPP_
#define WEATHER_HPP_

#include "ChildConsole.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class Weather : public Frame, public ChildConsole
    {
        private:
            std::string _apiUrl;
            CURL *_curl;

            std::string _latitude;
            std::string _longitude;

            std::string fetchData(std::string url);

        public:
            Weather(std::shared_ptr<Console> console);
            ~Weather();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !WEATHER_HPP_ */