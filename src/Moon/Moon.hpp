#ifndef MOON_HPP_
#define MOON_HPP_

#include "Console.hpp"
#include "Frame.hpp"
#include "ImgLoader.hpp"
#include "Includes.hpp"
#include "Settings.hpp"
#include "WebFetch.hpp"

namespace Explorer400D
{
    typedef struct MoonData_s {
            std::string time;
            float phase;
            float obscuration;
            float age;
            float diameter;
            float distance;
            float j2000_ra;
            float j2000_dec;
            float subsolar_lon;
            float subsolar_lat;
            float subearth_lon;
            float subearth_lat;
            float posangle;
    } MoonData_t;

    class Moon : public Frame
    {
        private:
            std::shared_ptr<Console> _console;
            std::shared_ptr<Settings> _settings;
            std::shared_ptr<WebFetch> _webFetch;
            std::shared_ptr<ImgLoader> _moonImg;
            MoonData_t _moonData;
            std::time_t _lastFetch;

            void initMoonData(std::string stringMoonData);
            void fetchMoonImg(std::string url);
            void fetchMoonData(bool highres);
            void showMoonData();

        public:
            Moon(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings);
            ~Moon();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !MOON_HPP_ */
