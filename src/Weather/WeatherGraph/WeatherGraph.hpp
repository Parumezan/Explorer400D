#ifndef WEATHERGRAPH_HPP_
#define WEATHERGRAPH_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class WeatherGraph
    {
        private:
            std::string _name;
            std::string _unit;
            std::pair<double, double> _range;
            bool _isShaded;
            std::vector<double> _time;
            std::map<std::pair<std::string, std::string>, std::vector<double>> _data;

        public:
            WeatherGraph(std::string name, std::string unit, std::pair<double, double> range = {0.0, 0.0}, bool isShaded = false);
            ~WeatherGraph() = default;

            std::string getName();

            std::map<std::pair<std::string, std::string>, std::vector<double>> getData();

            void addData(std::pair<std::string, std::string> key, std::vector<double> data = {});
            void addTime(std::vector<double> time);

            void setData(std::map<std::pair<std::string, std::string>, std::vector<double>> data);
            void setTime(std::vector<double> time);

            void clearData();

            void drawGraph();
    };
} // namespace Explorer400D

#endif /* !WEATHERGRAPH_HPP_ */
