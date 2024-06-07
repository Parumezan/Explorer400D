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
            std::map<std::string, std::vector<double>> _data;

        public:
            WeatherGraph(std::string name, std::string unit, std::pair<double, double> range = {0.0, 0.0}, bool isShaded = false);
            ~WeatherGraph() = default;

            void addData(std::string key, std::vector<double> data);
            void addTime(std::vector<double> time);

            void drawGraph();
    };
} // namespace Explorer400D

#endif /* !WEATHERGRAPH_HPP_ */
