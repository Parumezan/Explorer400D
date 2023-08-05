from src import Console, Settings
import dearpygui.dearpygui as dpg
import dearpygui.demo as demo


class Explorer400D:
    def __init__(self, width=1600, height=900) -> None:
        self._name = "Explorer400D"
        self._diminutive = "E4"
        self._version = "0.0.1"
        self._console = Console.Console(self._diminutive)
        self._settings = Settings.Settings(self._name, self._console)

        self._console._setSettings(self._settings)
        self._startDearPyGui(width, height)

    def _menuBar(self) -> None:
        uuidSettings = dpg.generate_uuid()
        uuidConsole = dpg.generate_uuid()

        with dpg.viewport_menu_bar():
            with dpg.menu(label="File"):
                dpg.add_menu_item(label="Save Settings",
                                  callback=self._settings.saveFrame)
                dpg.add_menu_item(label="Settings",
                                  callback=self._settings.frame(uuidSettings))
                dpg.add_menu_item(label="Exit", callback=dpg.stop_dearpygui)

            with dpg.menu(label="Tools"):
                dpg.add_menu_item(label="Console",
                                  callback=self._console.frame(uuidConsole))

    def _startDearPyGui(self, width, height) -> None:
        try:
            dpg.create_context()
            dpg.configure_app(docking=True, docking_space=True,
                              init_file=self._settings._initPath, load_init_file=True)
            dpg.create_viewport(title=self._name, width=width, height=height)

            self._menuBar()
            demo.show_demo()

            dpg.setup_dearpygui()
            dpg.show_viewport()
            dpg.start_dearpygui()

            # while dpg.is_dearpygui_running():  # replace by dpg.start_dearpygui()
            #     print("this will run every frame")
            #     dpg.render_dearpygui_frame()

            dpg.destroy_context()
        except Exception as error:
            self._console.error(error)


if __name__ == "__main__":
    explorer = Explorer400D()
