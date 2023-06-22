import customtkinter
import tkintermapview


class MapWindow(customtkinter.CTk):
    WIDTH = 1280
    HEIGHT = 720
    list_ap = []

    def __init__(self, root, clear):
        super().__init__()
        window_name = "Map"
        self.root = root
        self.window = customtkinter.CTkToplevel(root)
        self.window.protocol("WM_DELETE_WINDOW", lambda: self.import_marks())
        self.window.title("TSP test application")
        self.window.geometry(f"{self.WIDTH}x{self.HEIGHT}")
        # configure grid layout
        self.window.grid_columnconfigure(1, weight=1)

        self.window.grid_rowconfigure((0, 1, 2, 3), weight=5)

        # create sidebar frame with widgets
        logo_font = customtkinter.CTkFont(size=20, weight="bold")
        self.window.menu = customtkinter.CTkFrame(
            self.window, width=200, corner_radius=15)
        self.window.menu.grid(row=0, column=0, rowspan=12, sticky="nsew")
        self.window.menu.grid_rowconfigure(18, weight=1)

        self.window.logo_label = customtkinter.CTkLabel(
            self.window.menu, text=window_name, font=logo_font)
        self.window.logo_label.grid(row=0, column=0, padx=20, pady=(20, 10))

        pad_x = 50
        self.window.button_remove_last_mark = customtkinter.CTkButton(
            self.window.menu, text="Remove last marker", command=self.remove_last_marker)
        self.window.button_remove_last_mark.grid(
            row=2, column=0, padx=pad_x, pady=(0, 20))
        self.window.button_remove_last_mark = customtkinter.CTkButton(
            self.window.menu, text="Remove all marks", command=self.remove_all_marks)
        self.window.button_remove_last_mark.grid(
            row=3, column=0, padx=pad_x, pady=(0, 20))
        self.window.button_remove_last_mark = customtkinter.CTkButton(
            self.window.menu, text="Import marks", command=self.import_marks)
        self.window.button_remove_last_mark.grid(
            row=4, column=0, padx=pad_x, pady=(0, 20))

        self.window.map_widget = tkintermapview.TkinterMapView(
            self.window, width=1000, height=1100, corner_radius=0)
        self.window.map_widget.grid(row=0, column=1, sticky="nsew")
        self.window.map_widget.add_left_click_map_command(self.add_marker_event)

        self.window.map_label = customtkinter.CTkLabel(
            self.window.menu, text="Tile server:", anchor="w")
        self.window.map_label.grid(row=19, column=0, padx=(20, 20), pady=(20, 0))
        self.window.map_option_menu_server = customtkinter.CTkOptionMenu(self.window.menu,
                                                                         values=["OpenStreetMap", "Google normal",
                                                                                 "Google satellite"],
                                                                         command=self.change_map)
        self.window.map_option_menu_server.grid(
            row=20, column=0, padx=(20, 20), pady=(0, 50))
        if clear:
            self.list_ap.clear()

    def change_map(self, new_map: str):
        if new_map == "OpenStreetMap":
            self.window.map_widget.set_tile_server(
                "https://a.tile.openstreetmap.org/{z}/{x}/{y}.png")
        elif new_map == "Google normal":
            self.window.map_widget.set_tile_server(
                "https://mt0.google.com/vt/lyrs=m&hl=en&x={x}&y={y}&z={z}&s=Ga", max_zoom=22)
        elif new_map == "Google satellite":
            self.window.map_widget.set_tile_server(
                "https://mt0.google.com/vt/lyrs=s&hl=en&x={x}&y={y}&z={z}&s=Ga", max_zoom=22)

    # destroy window and self if import marks button was clicked or close was clicked
    def import_marks(self):
        if len(self.list_ap) > 1:
            self.root.set_places(self.list_ap, "from map")
        self.root.map_window_opened = False
        self.window.destroy()
        self.destroy()

    # remove last marker if exist
    def remove_last_marker(self):
        if len(self.list_ap) > 0:
            self.list_ap[-1]['marker'].delete()
            self.list_ap.pop(-1)

    # remove all marks on map
    def remove_all_marks(self):
        for i in self.list_ap:
            if not i['marker']:
                return
            i['marker'].delete()
        self.list_ap.clear()

    def add_marker_event(self, coords):
        new_marker = self.window.map_widget.set_marker(
            coords[0], coords[1], text="Place" + str(len(self.list_ap)))
        cords = {
            'x': coords[0],
            'y': coords[1],
            'marker': new_marker,
        }
        self.list_ap.append(cords)
