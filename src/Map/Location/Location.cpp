#include "Location.hpp"

using namespace Explorer400D;

Location::Location(float latitude, float longitude, std::string name, std::string country) : _latitude(latitude), _longitude(longitude), _name(name), _country(country)
{
}

Location::Location(json obj)
{
    this->_latitude = obj["latitude"].get<float>();
    this->_longitude = obj["longitude"].get<float>();
    this->_name = obj["name"].get<std::string>();
    this->_country = obj["country"].get<std::string>();
}

std::string Location::getName()
{
    return this->_name;
}

std::string Location::getCountry()
{
    return this->_country;
}

std::pair<float, float> Location::getCoordinates()
{
    return std::make_pair(this->_latitude, this->_longitude);
}

json Location::getJson()
{
    json obj = nullptr;

    obj["latitude"] = this->_latitude;
    obj["longitude"] = this->_longitude;
    obj["name"] = this->_name;
    obj["country"] = this->_country;

    return obj;
}