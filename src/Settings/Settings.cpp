#include "Settings.hpp"

using namespace Explorer400D;

// TODO: Implement sqlite database for settings

Settings::Settings(std::shared_ptr<Console> console) : _console(console)
{
    this->_console->info("Initializing Settings module...");
    this->_settingsPath = std::string(getenv("HOME")) + "/.config/Explorer400D";
    try {
        this->initSettings();
        this->initDatabase();
    } catch (std::exception &error) {
        this->_console->error(error.what());
    }
    this->_console->info("Settings module initialized");
}

Settings::~Settings()
{
    this->_console->info("Closing Settings module...");
    this->closeDatabase();
    this->_console->info("Settings module closed");
}

void Settings::initSettings()
{
    if (mkdir(this->_settingsPath.c_str(), 0777) == -1)
        this->_console->info("Settings directory already exists");
    else
        this->_console->info("Settings directory created");
}

void Settings::initDatabase()
{
    if (sqlite3_open((this->_settingsPath + "/settings.db").c_str(), &this->_db) != SQLITE_OK)
        throw this->_console->throwException("Failed to open database: " + std::string(sqlite3_errmsg(this->_db)));

    std::string sql = "CREATE TABLE IF NOT EXISTS WindowStatus ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "NAME TEXT NOT NULL,"
                      "VALUE INTEGER NOT NULL"
                      ");";

    if (sqlite3_exec(this->_db, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to create table: " + std::string(sqlite3_errmsg(this->_db)));

    sql = "CREATE TABLE IF NOT EXISTS Settings ("
          "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
          "NAME TEXT NOT NULL,"
          "VALUE TEXT NOT NULL"
          ");";

    if (sqlite3_exec(this->_db, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to create table: " + std::string(sqlite3_errmsg(this->_db)));

    // insert if not exists WidgetStatus
    sql = "INSERT INTO WindowStatus (NAME, VALUE) SELECT ?, ? WHERE NOT EXISTS (SELECT 1 FROM WindowStatus WHERE NAME = ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

    for (size_t cpt = 0; cpt < WidgetNames.size(); cpt++) {
        sqlite3_bind_text(stmt, 1, WidgetNames[cpt].c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, 0);
        sqlite3_bind_text(stmt, 3, WidgetNames[cpt].c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw this->_console->throwException("Failed to step statement: " + std::string(sqlite3_errmsg(this->_db)));
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
}

void Settings::closeDatabase()
{
    sqlite3_close(this->_db);
}

void Settings::saveWidgetStatus(std::map<std::string, bool> widgetStatus)
{
    // find and update by name WidgetStatus
    std::string sql = "UPDATE WindowStatus SET VALUE = ? WHERE NAME = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

    for (size_t cpt = 0; cpt < WidgetNames.size(); cpt++) {
        sqlite3_bind_int(stmt, 1, widgetStatus[WidgetNames[cpt]]);
        sqlite3_bind_text(stmt, 2, WidgetNames[cpt].c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw this->_console->throwException("Failed to step statement: " + std::string(sqlite3_errmsg(this->_db)));
        sqlite3_reset(stmt);
    }

    this->_console->info("Widget status saved in db");
}

void Settings::saveConfig()
{
    this->_console->info("Save suplementary settings...");
    this->setSettingsVar("IMPLOT_UseLocalTime", ImPlot::GetStyle().UseLocalTime ? "true" : "false");
    this->setSettingsVar("IMPLOT_UseISO8601", ImPlot::GetStyle().UseISO8601 ? "true" : "false");
    this->setSettingsVar("IMPLOT_Use24HourClock", ImPlot::GetStyle().Use24HourClock ? "true" : "false");
}

void Settings::saveSettings(std::map<std::string, bool> widgetStatus)
{
    this->_console->saveHistory(this->_settingsPath);

    this->saveWidgetStatus(widgetStatus);
    this->saveConfig();
    this->_console->info("Settings saved");
}

void Settings::setSettingsPath(std::string &path)
{
    // TODO : Copy the old settings and Remove the old settings
    this->_settingsPath = path;
    try {
        this->initSettings();
    } catch (std::exception &error) {
        this->_console->error(error.what());
    }
}

std::string Settings::getSettingsPath()
{
    return this->_settingsPath;
}

std::map<std::string, bool> Settings::getWidgetsStatus()
{
    std::map<std::string, bool> widgetStatus;

    std::string sql = "SELECT NAME, VALUE FROM WindowStatus;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        widgetStatus[(const char *)sqlite3_column_text(stmt, 0)] = sqlite3_column_int(stmt, 1);
    }

    sqlite3_finalize(stmt);

    return widgetStatus;
}

std::string Settings::getSettingsVar(std::string varName)
{
    std::string sql = "SELECT VALUE FROM Settings WHERE NAME = ?;";
    std::string value = "";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

    sqlite3_bind_text(stmt, 1, varName.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        value = (const char *)sqlite3_column_text(stmt, 0);
    } else {
        value = "false";
    }

    sqlite3_finalize(stmt);

    this->_console->info("Get settings var " + varName + " : " + value);
    return value;
}

void Settings::setSettingsVar(std::string varName, std::string varValue)
{
    bool exist = false;
    std::string sql = "SELECT EXISTS(SELECT 1 FROM Settings WHERE NAME = ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

    sqlite3_bind_text(stmt, 1, varName.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exist = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (exist) {
        sql = "UPDATE Settings SET VALUE = ? WHERE NAME = ?;";

        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
            throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

        sqlite3_bind_text(stmt, 1, varValue.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, varName.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw this->_console->throwException("Failed to step statement: " + std::string(sqlite3_errmsg(this->_db)));
        sqlite3_reset(stmt);

        sqlite3_finalize(stmt);
    } else {
        sql = "INSERT INTO Settings (NAME, VALUE) VALUES (?, ?);";

        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(this->_db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
            throw this->_console->throwException("Failed to prepare statement: " + std::string(sqlite3_errmsg(this->_db)));

        sqlite3_bind_text(stmt, 1, varName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, varValue.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw this->_console->throwException("Failed to step statement: " + std::string(sqlite3_errmsg(this->_db)));
        sqlite3_reset(stmt);

        sqlite3_finalize(stmt);
    }

    this->_console->info("Settings var " + varName + " set to " + varValue);
}

void Settings::frameLoop()
{
    ImGui::Begin("Settings", &this->state);

    ImGui::SeparatorText("Plot Time Display");
    ImGui::Checkbox("Local Time", &ImPlot::GetStyle().UseLocalTime);
    ImGui::SameLine();
    ImGui::Checkbox("ISO 8601", &ImPlot::GetStyle().UseISO8601);
    ImGui::SameLine();
    ImGui::Checkbox("24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);

    ImGui::End();
}