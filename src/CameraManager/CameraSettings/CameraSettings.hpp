#ifndef CAMERASETTINGS_HPP_
#define CAMERASETTINGS_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class CameraSettings
    {
        private:
            CameraWidget *_widget;
            CameraWidgetType _widgetType;

            std::string _path;
            std::string _label;
            std::string _name;

            int _readonly;

            bool _isInitialized;
            bool _isInit;

            char _inputText[64] = "";
            std::string _textValue;
            void _initText();
            void _widgetText();

            float _rangeValue = 0.0f, _rangeTop = 0.0f, _rangeBottom = 0.0f, _rangeStep = 0.0f;
            void _initRange();
            void _widgetRange();

            int _toggleValue = 0;
            bool _toggleBool = false;
            void _initToggle();
            void _widgetToggle();

            std::vector<std::string> _radioChoices;
            std::string _radioValue;
            void _initMenuOrRadio();
            void _widgetMenuOrRadio();

            time_t _dateValue;
            void _initDate();
            void _widgetDate();

            void _initWindow();
            void _widgetWindow();

            void _widgetError();

        public:
            CameraSettings(CameraWidget *widget, std::string path);
            ~CameraSettings() = default;

            void initBinding();
            std::function<void()> initWidget;
            std::function<void()> drawWidget;

            std::string getLabel();
            std::string getName();
    };
} // namespace Explorer400D

#endif /* !CAMERASETTINGS_HPP_ */