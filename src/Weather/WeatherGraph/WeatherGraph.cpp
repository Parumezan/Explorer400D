#include "WeatherGraph.hpp"

using namespace Explorer400D;

WeatherGraph::WeatherGraph(std::string name, std::string unit, std::pair<double, double> range, bool isShaded) : _name(name), _unit(unit), _range(range), _isShaded(isShaded)
{
}

std::string WeatherGraph::getName()
{
    return this->_name;
}

void WeatherGraph::addData(std::pair<std::string, std::string> key, std::vector<double> data)
{
    this->_data[key] = data;
}

void WeatherGraph::addTime(std::vector<double> time)
{
    this->_time = time;
}

std::map<std::pair<std::string, std::string>, std::vector<double>> WeatherGraph::getData()
{
    return this->_data;
}

void WeatherGraph::setData(std::map<std::pair<std::string, std::string>, std::vector<double>> data)
{
    this->_data = data;
}

void WeatherGraph::setTime(std::vector<double> time)
{
    this->_time = time;
}

void WeatherGraph::clearData()
{
    for (auto &data : this->_data)
        data.second.clear();
    this->_time.clear();
}

void WeatherGraph::drawGraph()
{
    if (ImPlot::BeginPlot(this->_name.c_str(), ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
        if (this->_time.size() == 0) {
            ImPlot::EndPlot();
            return;
        }

        ImPlot::SetupAxes("Time", this->_unit.c_str(), 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisLimits(ImAxis_X1, this->_time[0], this->_time[this->_time.size() - 1]);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, this->_time[0], this->_time[this->_time.size() - 1]);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, this->_range.first, this->_range.second);

        for (auto &data : this->_data)
            ImPlot::PlotLine(data.first.first.c_str(), this->_time.data(), data.second.data(), this->_time.size());

        if (this->_isShaded) {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.50f);

            for (auto &data : this->_data)
                ImPlot::PlotShaded(data.first.first.c_str(), this->_time.data(), data.second.data(), this->_time.size(), -INFINITY);
        }

        double actualTime = (double)std::time(0);
        ImPlot::PlotInfLines("Now", &actualTime, 1);

        ImPlot::EndPlot();
    }
}