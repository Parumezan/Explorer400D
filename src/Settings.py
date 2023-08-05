import os
import dearpygui.dearpygui as dpg


class Settings:
    def __init__(self, name, console) -> None:
        self._name = name
        self._console = console
        self._console.info("Initializing settings module...")

        self._configPath = os.path.join(
            os.path.expanduser("~"), ".config", self._name)
        self._initPath = os.path.join(self._configPath, "pygui.ini")

        self._initConfig()
        self._console.info("Settings module started")

    def __del__(self) -> None:
        self._console.info("Settings module destroyed")

    def _initConfig(self) -> None:
        try:
            if not os.path.exists(self._configPath):
                self._console.info("Creating config folder...")
                os.makedirs(self._configPath)
                self._console.info("Config folder created")
            else:
                self._console.info("Config folder already exists")
                self._console.info("Checking for settings file...")
                if not os.path.exists(self._initPath):
                    self._console.info("Settings file not found")
                    self._createConfig()
                else:
                    self._console.info("Settings file found")
                    self._loadConfig()
        except Exception as error:
            self._console.error(error)

    def _createConfig(self) -> None:
        try:
            self._console.info("Creating settings file...")
            self._console.info("Settings file created")
        except Exception as error:
            self._console.error(error)

    def _loadConfig(self) -> None:
        try:
            self._console.info("Loading settings...")
            self._console.info("Settings loaded")
        except Exception as error:
            self._console.error(error)

    def saveFrame(self) -> None:
        try:
            self._console.info("Saving settings...")
            dpg.save_init_file(self._initPath)
            self._console.info("Settings saved")
        except Exception as error:
            self._console.error(error)

    def frame(self, uuid) -> None:
        with dpg.window(label="Settings", tag=uuid):
            pass
