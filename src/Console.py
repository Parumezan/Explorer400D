import datetime
import dearpygui.dearpygui as dpg
import os
from io import open


class Console:
    def __init__(self, name="Console") -> None:
        self._name = name
        self._history = []
        self._infoCount = 0
        self._warnCount = 0
        self._errorCount = 0
        self._settings = None

        self._append(f"{self._getNowTime()} {self._name} created")

    def __del__(self) -> None:
        self._append(f"{self._getNowTime()} {self._name} will be destroyed...")
        self._append(
            f"{self._getNowTime()} {self._name}: {self._infoCount} info, {self._warnCount} warn, {self._errorCount} error")
        try:
            with open(os.path.join(self._settings._configPath, "console.log"), "w") as file:
                for line in self._history:
                    file.write(line + "\n")
        except Exception as error:
            self.error(error)

    def _getNowTime(self) -> str:
        # return datetime.datetime.now().strftime("%Y:%m:%d %H:%M:%S")
        return datetime.datetime.now().isoformat()

    def _print(self, message) -> None:
        print(message)

    def _append(self, message) -> None:
        self._history.append(message)
        self._print(message)

    def _setSettings(self, settings) -> None:
        self._settings = settings

    def info(self, message) -> None:
        self._infoCount += 1
        msg = f"{self._getNowTime()} {self._name}-Info: {message}"
        self._append(msg)

    def warn(self, message) -> None:
        self._warnCount += 1
        msg = f"{self._getNowTime()} {self._name}-Warn: {message}"
        self._append(msg)

    def error(self, message) -> None:
        self._errorCount += 1
        msg = f"{self._getNowTime()} {self._name}-Error: {message}"
        self._append(msg)

    def frame(self, uuid) -> None:
        with dpg.window(label="Console", tag=uuid):
            for line in self._history:
                time, message = line.split(" ", 1)
                typeMsg, message = message.split(" ", 1)

                if typeMsg == self._name + "-Info:":
                    color = (0, 255, 0, 255)
                elif typeMsg == self._name + "-Warn:":
                    color = (255, 255, 0, 255)
                elif typeMsg == self._name + "-Error:":
                    color = (255, 0, 0, 255)
                else:
                    color = (255, 255, 255, 255)

                with dpg.group(horizontal=True):
                    dpg.add_text(time)
                    dpg.add_text(typeMsg, color=color)
                    dpg.add_text(message)
