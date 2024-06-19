#include "CameraSettings.hpp"

using namespace Explorer400D;

CameraSettings::CameraSettings(CameraWidget *widget, std::string path) : _widget(widget), _path(path)
{
    this->_isInit = false;
    this->_isInitialized = false;
    this->_readonly = 0;

    int ret = GP_OK;

    const char *label;
    const char *name;

    if ((ret = gp_widget_get_type(this->_widget, &this->_widgetType)) != GP_OK) {
        spdlog::warn("Failed to get widget type");
        this->_widgetType = GP_WIDGET_WINDOW;
    }

    if ((ret = gp_widget_get_label(this->_widget, &label)) != GP_OK) {
        spdlog::warn("Failed to get widget label");
        this->_label = "Unknown";
    } else
        this->_label = label;

    if ((ret = gp_widget_get_name(this->_widget, &name)) != GP_OK) {
        spdlog::warn("Failed to get widget name");
        this->_name = "Unknown";
    } else
        this->_name = name;

    if ((ret = gp_widget_get_readonly(this->_widget, &this->_readonly)) != GP_OK) {
        spdlog::warn("Failed to get widget readonly");
        this->_readonly = 0;
    }
}

void CameraSettings::_initText()
{
    char *txt = nullptr;
    int ret = gp_widget_get_value(this->_widget, &txt);

    if (ret != GP_OK) {
        spdlog::warn("Failed to get text widget: {}", this->_name);
        return;
    }

    this->_textValue = txt;
    this->_isInit = false;
    this->_isInitialized = true;
}

void CameraSettings::_initRange()
{
    float value = 0.0f, top = 0.0f, bottom = 0.0f, step = 0.0f;
    int ret = gp_widget_get_range(this->_widget, &bottom, &top, &step);

    if (ret != GP_OK) {
        spdlog::warn("Failed to get range widget: {}", this->_name);
        return;
    }

    ret = gp_widget_get_value(this->_widget, &value);

    if (ret != GP_OK) {
        spdlog::warn("Failed to get range value widget: {}", this->_name);
        return;
    }

    this->_rangeValue = value;
    this->_rangeTop = top;
    this->_rangeBottom = bottom;
    this->_rangeStep = step;
    this->_isInit = false;
    this->_isInitialized = true;
}

void CameraSettings::_initToggle()
{
    int value = 0;
    int ret = gp_widget_get_value(this->_widget, &value);

    if (ret != GP_OK) {
        spdlog::warn("Failed to get toggle widget: {}", this->_name);
        return;
    }

    this->_toggleValue = value;
    this->_isInit = false;
    this->_isInitialized = true;
}

void CameraSettings::_initMenuOrRadio()
{
    char *value = nullptr;
    int ret = gp_widget_get_value(this->_widget, &value);

    if (ret != GP_OK) {
        spdlog::warn("Failed to get menu / radio widget: {}", this->_name);
        return;
    }

    this->_radioValue = value;

    int count = gp_widget_count_choices(this->_widget);

    for (size_t cpt = 0; cpt < count; cpt++) {
        const char *choice = nullptr;

        if (gp_widget_get_choice(this->_widget, cpt, &choice) != GP_OK) {
            spdlog::warn("Failed to get choice: {} from widget: {}", cpt, this->_name);
            continue;
        }

        this->_radioChoices.push_back(choice);
    }

    this->_isInit = false;
    this->_isInitialized = true;
}

void CameraSettings::_initDate()
{
    int value = 0;
    int ret = gp_widget_get_value(this->_widget, &value);

    if (ret != GP_OK) {
        spdlog::warn("Failed to get date widget: {}", this->_name);
        return;
    }

    this->_dateValue = value;

    this->_isInit = false;
    this->_isInitialized = true;
}

void CameraSettings::_initWindow()
{
    this->_isInitialized = true;
}

void CameraSettings::_widgetText()
{
    if (!this->_isInitialized) {
        this->_widgetError();
        return;
    }

    if (!this->_isInit)
        strcpy(this->_inputText, this->_textValue.c_str());

    ImGuiInputTextFlags inputTextFlags;

    if (this->_readonly > 0)
        inputTextFlags = ImGuiInputTextFlags_ReadOnly;
    else
        inputTextFlags = ImGuiInputTextFlags_None;

    ImGui::InputText(this->_label.c_str(), this->_inputText, sizeof(this->_inputText), inputTextFlags);

    if (this->_readonly > 0) {
        ImGui::BeginDisabled();
        // ImGui::Button("Set Value");
        // ImGui::SameLine();
        // ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Readonly");
        ImGui::EndDisabled();
    } else {
        // if (ImGui::Button("Set Value")) {
        //     this->_textValue = this->_inputText;
        //     if (gp_widget_set_value(this->_widget, this->_textValue.c_str()) != GP_OK)
        //         spdlog::warn("Failed to set widget: {} to {}", this->_name, this->_textValue);
        //     else
        //         spdlog::info("Set widget: {} to {}", this->_name, this->_textValue);
        // }
    }

    if (!this->_isInit)
        this->_isInit = true;
}

void CameraSettings::_widgetRange()
{
    if (!this->_isInitialized) {
        this->_widgetError();
        return;
    }

    if (this->_readonly > 0) {
        ImGui::BeginDisabled();
        ImGui::DragFloat(this->_label.c_str(), &this->_rangeValue, this->_rangeStep, this->_rangeBottom, this->_rangeTop, "%.2f");
        // ImGui::Button("Set Value");
        // ImGui::SameLine();
        // ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Readonly");
        ImGui::EndDisabled();
    } else {
        ImGui::DragFloat(this->_label.c_str(), &this->_rangeValue, this->_rangeStep, this->_rangeBottom, this->_rangeTop, "%.2f");
        // if (ImGui::Button("Set Value")) {
        //     if (gp_widget_set_value(this->_widget, &this->_rangeValue) != GP_OK)
        //         spdlog::warn("Failed to set widget: {} to {}", this->_name, this->_rangeValue);
        //     else
        //         spdlog::info("Set widget: {} to {}", this->_name, this->_rangeValue);
        // }
    }

    if (!this->_isInit)
        this->_isInit = true;
}

void CameraSettings::_widgetToggle()
{
    if (!this->_isInitialized) {
        this->_widgetError();
        return;
    }

    if (!this->_isInit)
        this->_toggleBool = this->_toggleValue > 0;

    if (this->_readonly > 0) {
        ImGui::BeginDisabled();
        ImGui::Checkbox(this->_label.c_str(), &this->_toggleBool);
        // ImGui::Button("Set Value");
        // ImGui::SameLine();
        // ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Readonly");
        ImGui::EndDisabled();
    } else {
        ImGui::Checkbox(this->_label.c_str(), &this->_toggleBool);
        // if (ImGui::Button("Set Value")) {
        //     this->_toggleValue = this->_toggleBool ? 1 : 0;
        //     if (gp_widget_set_value(this->_widget, &this->_toggleValue) != GP_OK)
        //         spdlog::warn("Failed to set widget: {} to {}", this->_name, this->_toggleValue);
        //     else
        //         spdlog::info("Set widget: {} to {}", this->_name, this->_toggleValue);
        // }
    }

    if (!this->_isInit)
        this->_isInit = true;
}

void CameraSettings::_widgetMenuOrRadio()
{
    if (!this->_isInitialized) {
        this->_widgetError();
        return;
    }

    if (this->_readonly > 0) {
        ImGui::BeginDisabled();
        if (ImGui::BeginCombo(this->_label.c_str(), this->_radioValue.c_str())) {
            for (size_t cpt = 0; cpt < this->_radioChoices.size(); cpt++) {
                bool isSelected = this->_radioValue == this->_radioChoices[cpt];

                if (ImGui::Selectable(this->_radioChoices[cpt].c_str(), isSelected))
                    this->_radioValue = this->_radioChoices[cpt];
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        // ImGui::Button("Set Value");
        // ImGui::SameLine();
        // ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Readonly");
        ImGui::EndDisabled();
    } else {
        if (ImGui::BeginCombo(this->_label.c_str(), this->_radioValue.c_str())) {
            for (size_t cpt = 0; cpt < this->_radioChoices.size(); cpt++) {
                bool isSelected = this->_radioValue == this->_radioChoices[cpt];

                if (ImGui::Selectable(this->_radioChoices[cpt].c_str(), isSelected))
                    this->_radioValue = this->_radioChoices[cpt];
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // if (ImGui::Button("Set Value")) {
        //     if (gp_widget_set_value(this->_widget, this->_radioValue.c_str()) != GP_OK)
        //         spdlog::warn("Failed to set widget: {} to {}", this->_name, this->_radioValue);
        //     else
        //         spdlog::info("Set widget: {} to {}", this->_name, this->_radioValue);
        // }
    }

    if (!this->_isInit)
        this->_isInit = true;
}

void CameraSettings::_widgetDate()
{
    if (!this->_isInitialized) {
        this->_widgetError();
        return;
    }

    if (this->_readonly > 0) {
        ImGui::BeginDisabled();
        ImGui::Text("Date: %s", ctime(&this->_dateValue));
        // ImGui::Button("Set Value");
        // ImGui::SameLine();
        // ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Readonly");
        ImGui::EndDisabled();
    } else {
        ImGui::Text("Date: %s", ctime(&this->_dateValue));
        // if (ImGui::Button("Set Value")) {
        //     if (gp_widget_set_value(this->_widget, &this->_dateValue) != GP_OK)
        //         spdlog::warn("Failed to set widget: {} to {}", this->_name, this->_dateValue);
        //     else
        //         spdlog::info("Set widget: {} to {}", this->_name, this->_dateValue);
        // }
    }

    if (!this->_isInit)
        this->_isInit = true;
}

void CameraSettings::_widgetWindow()
{
    if (!this->_isInitialized) {
        this->_widgetError();
        return;
    }

    ImGui::Text("Type: GP_WIDGET_WINDOW / GP_WIDGET_SECTION / GP_WIDGET_BUTTON Label: %s Name: %s", this->_label.c_str(), this->_name.c_str());

    if (!this->_isInit)
        this->_isInit = true;
}

void CameraSettings::_widgetError()
{
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: Widget {} is not initialized", this->_name);
}

void CameraSettings::initBinding()
{
    switch (this->_widgetType) {
    case GP_WIDGET_TEXT:
        this->initWidget = std::bind(&CameraSettings::_initText, this);
        this->drawWidget = std::bind(&CameraSettings::_widgetText, this);
        break;
    case GP_WIDGET_RANGE:
        this->initWidget = std::bind(&CameraSettings::_initRange, this);
        this->drawWidget = std::bind(&CameraSettings::_widgetRange, this);
        break;
    case GP_WIDGET_TOGGLE:
        this->initWidget = std::bind(&CameraSettings::_initToggle, this);
        this->drawWidget = std::bind(&CameraSettings::_widgetToggle, this);
        break;
    case GP_WIDGET_MENU:
    case GP_WIDGET_RADIO:
        this->initWidget = std::bind(&CameraSettings::_initMenuOrRadio, this);
        this->drawWidget = std::bind(&CameraSettings::_widgetMenuOrRadio, this);
        break;
    case GP_WIDGET_DATE:
        this->initWidget = std::bind(&CameraSettings::_initDate, this);
        this->drawWidget = std::bind(&CameraSettings::_widgetDate, this);
        break;
    case GP_WIDGET_WINDOW:
    case GP_WIDGET_SECTION:
    case GP_WIDGET_BUTTON:
        this->initWidget = std::bind(&CameraSettings::_initWindow, this);
        this->drawWidget = std::bind(&CameraSettings::_widgetWindow, this);
        break;
    }
}

std::string CameraSettings::getLabel()
{
    return this->_label;
}

std::string CameraSettings::getName()
{
    return this->_name;
}