#ifndef MAP_HPP_
#define MAP_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"
#include "WebFetch/WebFetch.hpp"

namespace Explorer400D
{
    class Location
    {
        public:
            Location() = default;
            ~Location() = default;

            float latitude = 0.0f;
            float longitude = 0.0f;
            std::string name = "";
            std::string country = "";
    };

    class Map : public Module
    {
        private:
            Settings *_settings;
            WebFetch *_webFetch;

        public:
            Map(Settings &settings, WebFetch &webFetch);
            ~Map() = default;

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
} // namespace Explorer400D

#endif /* !MAP_HPP_ */
