#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class Location
    {
        private:
            float _latitude = 0.0f;
            float _longitude = 0.0f;
            std::string _name = "";
            std::string _country = "";

        public:
            Location(float latitude = 0.0f, float longitude = 0.0f, std::string name = "", std::string country = "");
            Location(json obj);
            ~Location() = default;

            std::string getName();
            std::string getCountry();
            std::pair<float, float> getCoordinates();
            json getJson();
    };
} // namespace Explorer400D

#endif /* !LOCATION_HPP_ */
