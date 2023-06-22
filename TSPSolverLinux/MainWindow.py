import json
import math
import os
import platform
import subprocess
import threading
import time
import tkinter
import tkinter.filedialog
import tkinter.messagebox

import customtkinter
import geopy.distance
import numpy as np
import tsplib95
import utm
from matplotlib import pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

import ACOSettings
import MapWindow
import MyAlgoSettings
import NearestNeighborSettings
import nearestneighbor as nn

# Modes: "System" (standard), "Dark", "Light"
customtkinter.set_appearance_mode("System")
# Themes: "blue" (standard), "green", "dark-blue"
customtkinter.set_default_color_theme("e.json")


# class for towns and their distances between
class Place:
    def __init__(self, id, x, y, distance: np.array):
        self.id = id
        self.x = x
        self.y = y
        self.distance = distance


class PlaceEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Place):
            return {"id": obj.id, "x": obj.x, "y": obj.y, "distance": obj.distance.tolist()}
        elif isinstance(obj, np.ndarray):
            return obj.tolist()
        else:
            return super().default(obj)


def to_dict(self):
    return {'id': self.id, 'x': self.x, 'y': self.y, 'distance': self.distance}


class App(customtkinter.CTk):
    WIDTH = 1280
    HEIGHT = 720
    INSTANTLY = -1
    end_file = "exe"
    name = "TSP solver"
    ACO_settings_setup = "1.0\n3.0\n0.1\n20000.0\n0.1\n0.1\n1.0\n50\n100\n2\n1\n"
    MyAlgo_settings_setup = "1\n12\n33\n4\n0\n0\n"
    best_distance = float('inf')
    best_path = []
    slider_value = 500  # default slider value
    # for points
    list_cords = []
    list_places = []
    animation_speed = INSTANTLY
    generation_mode = "Random"
    algorithm_type = "Nearest neighbor"
    running = True  # To close all threads on quit
    stop = False  # for interrupt solving
    is_solving = False  # solution lock, when the next solution is generated or run
    created = 0  # used for updating cuz of memory leaks
    # for checking if sub windows was opened
    map_window_opened = False
    settings_opened = False
    mode = "Light"
    points_color = "black"

    def __init__(self):
        super().__init__()
        self.root_thread = self
        self.thread = self
        # fonts
        logo_font = customtkinter.CTkFont(size=20, weight="bold")

        # configure window
        self.title("TSP test application")
        self.geometry(f"{self.WIDTH}x{self.HEIGHT}")
        self.protocol("WM_DELETE_WINDOW", lambda: self.quit_app())
        self.font_system_ui = customtkinter.CTkFont(family="system-ui", size=25, weight="bold")
        # configure grid layout
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure((0, 1, 2, 3), weight=5)

        # create sidebar frame with widgets
        self.menu = customtkinter.CTkFrame(self, width=200, corner_radius=15)
        self.menu.grid(row=0, column=0, rowspan=12, sticky="nsew")
        self.menu.grid_rowconfigure(20, weight=1)
        self.logo_label = customtkinter.CTkLabel(
            self.menu, text=self.name, font=logo_font)
        self.logo_label.grid(row=0, column=0, padx=20, pady=(20, 10))

        pad_x = 50

        self.place_mode_label = customtkinter.CTkLabel(
            self.menu, text="Place generation mode:")
        self.place_mode_label.grid(row=2, column=0, padx=pad_x)
        self.place_mode_option_menu = customtkinter.CTkOptionMenu(self.menu, values=["Random", "From map", "From file"],
                                                                  command=self.change_generation_mode_event)
        self.place_mode_option_menu.grid(
            row=3, column=0, padx=pad_x, pady=(0, 20))
        self.place_mode_label = customtkinter.CTkLabel(
            self.menu, text="Number of places")
        self.place_mode_label.grid(row=4, column=0, padx=pad_x)

        self.places_entry_value = tkinter.StringVar(
            value=str(self.slider_value))
        self.places_entry = customtkinter.CTkEntry(
            self.menu, textvariable=self.places_entry_value, justify="center")
        self.places_entry.grid(row=5, column=0, padx=pad_x)

        self.generate_slider = customtkinter.CTkSlider(
            self.menu, from_=0, to=1000, number_of_steps=500, command=self.slider_event)
        self.generate_slider.grid(row=6, column=0, padx=20, pady=(0, 20))
        self.generate_button = customtkinter.CTkButton(
            self.menu, text="Generate", command=self.start_thread_generate_points)
        self.generate_button.grid(row=7, column=0, padx=50, pady=(0, 20))

        self.algorithm_label = customtkinter.CTkLabel(
            self.menu, text="Algorithm types:")
        self.algorithm_label.grid(row=8, column=0, padx=pad_x)
        self.algorithm_option_menu = customtkinter.CTkOptionMenu(self.menu,
                                                                 values=["Nearest neighbor", "ACO", "MyAlgo"],
                                                                 command=self.change_algorithm_type)
        self.algorithm_option_menu.grid(
            row=9, column=0, padx=pad_x, pady=(0, 20))

        self.solve_button = customtkinter.CTkButton(
            self.menu, text="Solve", command=self.start_thread_solve)
        self.solve_button.grid(row=10, column=0, padx=50, pady=(0, 20))

        self.algorithm_settings = customtkinter.CTkButton(
            self.menu, text="Stop", command=lambda: self.stop_solving())
        self.algorithm_settings.grid(row=11, column=0, padx=50, pady=(0, 20))
        self.algorithm_settings = customtkinter.CTkButton(
            self.menu, text="Clear roads", command=self.clear_lines)
        self.algorithm_settings.grid(row=12, column=0, padx=50, pady=(0, 20))
        self.algorithm_settings = customtkinter.CTkButton(
            self.menu, text="Algorithm settings", command=lambda: self.open_settings())
        self.algorithm_settings.grid(row=13, column=0, padx=50, pady=(0, 20))

        self.appearance_mode_label = customtkinter.CTkLabel(
            self.menu, text="Appearance mode:")
        self.appearance_mode_label.grid(row=15, column=0, padx=pad_x)
        self.appearance_mode_option_menu = customtkinter.CTkOptionMenu(self.menu, values=["Light", "Dark"],
                                                                       command=self.change_appearance_mode_event)
        self.appearance_mode_option_menu.grid(row=16, column=0, padx=pad_x)

        self.scaling_label = customtkinter.CTkLabel(
            self.menu, text="UI scaling:", anchor="w")
        self.scaling_label.grid(row=17, column=0, padx=pad_x)
        place_holder_scaling_option_menu = tkinter.StringVar(value="100%")
        self.scaling_option_menu = customtkinter.CTkOptionMenu(self.menu, variable=place_holder_scaling_option_menu,
                                                               values=["80%", "90%", "100%", "110%", "125%", "150%"],
                                                               command=self.change_scaling_event)
        self.scaling_option_menu.grid(row=18, column=0, padx=pad_x, pady=(0, 20))

        self.loading_label = customtkinter.CTkLabel(self, text="", font=self.font_system_ui)
        self.loading_label.grid(row=1, column=1)

        # create progres bar to loading
        if platform.system() == "Linux":
            self.end_file = "out"
        self.fig_points([], [])

    def loading(self, text):
        while (1):
            self.loading_label.configure(text=text)
            time.sleep(0.3)
            self.loading_label.configure(text=text + ".")
            time.sleep(0.3)
            self.loading_label.configure(text=text + "..")
            time.sleep(0.3)
            self.loading_label.configure(text=text + "...")
            time.sleep(0.3)
            if not self.is_solving:
                if self.best_distance != float("inf"):
                    self.loading_label.configure(text=str(self.best_distance))
                else:
                    self.loading_label.configure(text="")
                return

    def start_thread_solve(self):
        if len(self.list_cords) == 0:
            tkinter.messagebox.showinfo("showinfo", "You did not generate places before running the solution")
            return
        if self.is_solving:
            tkinter.messagebox.showinfo("showinfo", "You can´t start new solve while still solving")
            return
        self.loading_thread = threading.Thread(target=self.loading, args=("Solving",))
        self.loading_thread.daemon = True
        self.thread = threading.Thread(target=self.solve)
        self.thread.daemon = True
        self.loading_thread.start()
        self.thread.start()

    def start_thread_generate_points(self):
        if self.is_solving:
            tkinter.messagebox.showinfo("showinfo", "You can´t generate new places while still solving")
            return
        self.best_distance = float("inf")  # set float inf cuz reset points
        self.loading_thread = threading.Thread(target=self.loading, args=("Generating",))
        self.loading_thread.daemon = True
        self.thread = threading.Thread(target=self.generate_points)
        self.thread.daemon = True
        self.loading_thread.start()
        self.thread.start()

    # function which change buttons depends on generation_mode
    def change_generation_mode(self):
        padx = 50
        change_counter = 0  # counter for changing buttons
        if self.generation_mode == "Random":
            try:
                self.generate_button_map.destroy()
                self.generate_button_add_places.destroy()
            except:
                change_counter += 1
                try:
                    self.file_button.destroy()
                except:
                    change_counter += 1
            if change_counter == 2:  # no change
                return
            self.places_entry = customtkinter.CTkEntry(
                self.menu, textvariable=self.places_entry_value)
            self.places_entry.grid(row=4, column=0, padx=50)
            self.generate_slider = customtkinter.CTkSlider(
                self.menu, from_=0, to=1000, number_of_steps=500, command=self.slider_event)
            self.generate_slider.grid(row=5, column=0, padx=20, pady=(0, 20))
            self.generate_button = customtkinter.CTkButton(
                self.menu, text="Generate", command=self.generate_points)
            self.generate_button.grid(row=6, column=0, padx=padx, pady=(0, 20))
        elif self.generation_mode == "From map":
            try:
                self.generate_slider.destroy()
                self.generate_button.destroy()
                self.places_entry.destroy()
            except:
                change_counter += 1
                try:
                    self.file_button.destroy()
                except:
                    change_counter += 1
            if change_counter == 2:  # no change
                return
            self.generate_button_add_places = customtkinter.CTkButton(
                self.menu, text="Add places", command=lambda: self.open_map_window(clear=False))
            self.generate_button_add_places.grid(
                row=5, column=0, padx=padx, pady=(0, 20))
            self.generate_button_map = customtkinter.CTkButton(
                self.menu, text="Import places", command=lambda: self.open_map_window(clear=True))
            self.generate_button_map.grid(
                row=6, column=0, padx=padx, pady=(0, 20))
        elif self.generation_mode == "From file":
            change_counter = 0  # counter for changing buttons
            try:
                self.generate_slider.destroy()
                self.generate_button.destroy()
                self.places_entry.destroy()
            except:
                change_counter += 1
                try:
                    self.generate_button_map.destroy()
                    self.generate_button_add_places.destroy()
                except:
                    change_counter += 1
            if change_counter == 2:  # no change
                return
            self.file_button = customtkinter.CTkButton(
                self.menu, text="Select file", command=self.select_file)
            self.file_button.grid(row=6, column=0, padx=padx, pady=(0, 20))

    # function after click on button select file will ask on file and call set_places with places coords
    def select_file(self):
        filetypes = (
            ('tsp files', '*.tsp*'),
            ('all files', '*'),
        )

        filename = tkinter.filedialog.askopenfilename(
            title='Open a file',
            initialdir='/',
            filetypes=filetypes)
        if filename == "":  # if file was not selected
            return
        problem = tsplib95.load(filename)
        problem = problem.as_name_dict()
        try:
            self.start_thread_set_places(problem['node_coords'], "from file")
        except KeyError:
            self.start_thread_set_places(problem['display_data'], "from file")

    def change_algorithm_type(self, algorithm_type: str):
        self.algorithm_type = algorithm_type

    def change_generation_mode_event(self, generation_mode: str):
        self.generation_mode = generation_mode
        self.change_generation_mode()

    def check_update_file(self):
        with open('communicate/best_path.txt') as f:
            lines = f.readlines()
            best_path = lines[0][:-2].split()
            for i in best_path:
                self.best_path.append(int(i))
            self.best_distance = float(lines[1])
            f.close()

    def start_algo(self, algo_type):
        list(map(os.unlink, (os.path.join("communicate", f) for f in os.listdir("communicate"))))
        with open('communicate/algo_input.txt', 'w') as f:
            if algo_type == "colony":
                f.write(self.ACO_settings_setup)
            else:
                f.write(self.MyAlgo_settings_setup)
            for p in self.list_cords:
                dist_string = ""
                for d in p.distance:
                    dist_string += str(d) + " "
                p = to_dict(p)
                f.write(str(p['id']) + " " + str(p['x']) + " " + str(p['y']) + " " + dist_string[:-1])
                f.write('\n')
            f.close()
        with open('communicate/multi_core_arg.txt', 'w') as f:
            if algo_type == "colony":
                f.write("colony\n")
            else:
                f.write("myalgo\n")
            f.write("E\n")
            f.close()
        subprocess.call('python MultiCoreRunner.py', shell=True)
        self.check_update_file()

    # function which will call NearestNeighbor algorithm which will return visited places in right order and distance
    # between them after will call connect points visited places and sleep if animation is enabled
    def solve(self):
        self.is_solving = True
        if self.algorithm_type == "Nearest neighbor":
            visited_places, distance = nn.NearestNeighbor(self, self.list_cords, -1)
            self.best_distance = distance
        elif self.algorithm_type == "ACO":
            self.best_path.clear()
            self.start_algo("colony")
            self.best_path.append(self.best_path[0])
            visited_places, distance = self.best_path, self.best_distance
        elif self.algorithm_type == "MyAlgo":
            self.best_path.clear()
            self.start_algo("myalgo")
            self.best_path.append(self.best_path[0])
            visited_places, distance = self.best_path, self.best_distance

        size = len(visited_places)
        for i in range(1, size):
            if self.stop:
                self.best_distance = ""
                self.stop = False
                self.is_solving = False
                return
            try:
                if self.animation_speed != self.INSTANTLY and self.algorithm_type == "Nearest neighbor":
                    time.sleep(self.animation_speed / 1000)
                    self.connect_points(self.x_cords, self.y_cords,
                                        visited_places[i - 1], visited_places[i], i, size)
                    self.canvas.draw()
                else:
                    self.connect_points(self.x_cords, self.y_cords,
                                        visited_places[i - 1], visited_places[i], i, size)
            except IndexError:
                self.is_solving = False
                print("List index out of range!")
                return
        self.canvas.draw()
        tkinter.messagebox.showinfo("showinfo", "Distance: " + str(distance) +
                                    "\nCount of visisted places: " + str(len(visited_places)))
        self.is_solving = False

    def stop_solving(self):
        if self.is_solving:
            os.system('taskkill /f /im a.' + self.end_file)
            self.stop = True
            self.is_solving = False

    # sleep in tkinter
    def tksleep(self, time: float):
        """
        Emulating `time.sleep(seconds)`
        Created by TheLizzard, inspired by Thingamabobs
        """
        self.after(int(time * 1000), self.quit)
        self.mainloop()

    def clear_lines(self):
        if not self.is_solving:
            self.points.remove()
            self.ax.clear()
            self.fig_points(self.x_cords, self.y_cords)

    # function which will figure places on blue rectangle
    def fig_points(self, x_cords, y_cords):
        self.figure3 = plt.Figure(figsize=(15, 9), dpi=100)
        self.ax = self.figure3.add_subplot(111)
        self.figure3.gca().set_axis_off()
        self.figure3.subplots_adjust(top=1, bottom=0, right=1, left=0,
                                     hspace=0, wspace=0)
        self.figure3.gca().xaxis.set_major_locator(plt.NullLocator())
        self.figure3.gca().yaxis.set_major_locator(plt.NullLocator())
        if self.mode == "Light":
            face_color = "#1ba8f0"
            scatter_color = "white"
            self.points_color = "black"
        else:
            scatter_color = "#ff1c4c"
            face_color = "#100444"
            self.points_color = "#907cb4"
        self.figure3.patch.set_facecolor(face_color)
        self.points = self.ax.scatter(x_cords, y_cords, color=scatter_color, s=10)
        self.x_cords = x_cords
        self.y_cords = y_cords
        self.canvas = FigureCanvasTkAgg(self.figure3, self)
        self.canvas.get_tk_widget().grid(row=0, column=1, padx=(0, 0), pady=(20, 20))
        self.created = 1

        # https://stackoverflow.com/questions/35363444/plotting-lines-connecting-points

    # function which will connect 2 points and update loading bar
    def connect_points(self, x, y, p1, p2, i, size):
        self.update_idletasks()
        x1, x2 = x[p1], x[p2]
        y1, y2 = y[p1], y[p2]
        self.connection_lines = self.ax.plot([x1, x2], [y1, y2], color=self.points_color)

    def slider_event(self, value):
        self.slider_value = int(value)
        self.places_entry_value.set(str(self.slider_value))

    def open_input_dialog_event(self):
        dialog = customtkinter.CTkInputDialog(
            text="Type in a number:", title="CTkInputDialog")

    def change_appearance_mode_event(self, new_appearance_mode: str):
        # wrong chosen in json file and down want to rewrite it
        self.mode = new_appearance_mode
        customtkinter.set_appearance_mode(new_appearance_mode)
        self.fig_points(self.x_cords, self.y_cords)

    def change_scaling_event(self, new_scaling: str):
        new_scaling_float = int(new_scaling.replace("%", "")) / 100
        customtkinter.set_widget_scaling(new_scaling_float)

    def sidebar_button_event(self):
        print("sidebar_button click")

    # destroying all classes of sub windows if quit main window
    def quit_app(self):
        if self.settings_opened:
            try:
                self.nearest_neighbor_settings.destroy()
            except:
                try:
                    self.ACO_settings.destroy()
                except:
                    try:
                        self.MyAlgo_settings.destroy()
                    except:
                        pass
                    pass
        if self.map_window_opened:
            self.map_window.destroy()
        self.destroy()

    # open map window and in one moment can be opened only one instance
    def open_map_window(self, clear):
        if not self.map_window_opened:
            self.map_window = MapWindow.MapWindow(self, clear)
        self.map_window_opened = True

    def open_settings(self):
        if not self.settings_opened:
            if self.algorithm_type == "Nearest neighbor":
                self.nearest_neighbor_settings = NearestNeighborSettings.NearestNeighborSettings(self)
            elif self.algorithm_type == "ACO":
                self.ACO_settings = ACOSettings.ACOSettings(self)
            elif self.algorithm_type == "MyAlgo":
                self.MyAlgo_settings = MyAlgoSettings.MyAlgoSettings(self)
            self.settings_opened = True

    # function which will generate list of class Place for mode from map or from file
    def start_thread_set_places(self, places, mode):
        if self.is_solving:
            tkinter.messagebox.showinfo("showinfo", "You can´t generate new places while still solving")
            return
        self.thread = threading.Thread(target=self.set_places, args=(places, mode))
        self.thread.daemon = True
        self.thread.start()

    def set_places(self, places, mode):
        self.is_solving = True
        self.list_cords.clear()
        x_cords = []
        y_cords = []
        if mode == "from map":
            self.list_places = self.list_places + places
            for i, r in zip(places, range(len(places))):
                dist_list = []
                x, y, zone, ut = utm.from_latlon(i['x'], i['y'])
                x_cords.append(x)
                y_cords.append(y)
                for v in places:
                    coords_1 = (i['x'], i['y'])
                    coords_2 = (v['x'], v['y'])
                    # geography distance in km
                    distance = geopy.distance.geodesic(coords_1, coords_2).km
                    dist_list.append(distance)
                place = Place(r, i['x'], i['y'], np.array(dist_list))
                self.list_cords.append(place)
        elif mode == "from file":
            for i in range(1, len(places) + 1):
                dist_list = []
                x_cords.append(places[i][0])
                y_cords.append(places[i][1])
                for z in range(1, len(places) + 1):
                    if self.MyAlgo_settings_setup[-2] == "1":
                        dist = round(math.sqrt(
                            (places[i][0] - places[z][0]) ** 2 + (places[i][1] - places[z][1]) ** 2))
                    else:
                        dist = (math.sqrt(
                            (places[i][0] - places[z][0]) ** 2 + (places[i][1] - places[z][1]) ** 2))
                    dist_list.append(dist)
                place = Place(i - 1, places[i][0], places[i][1], np.array(dist_list))
                self.list_cords.append(place)

        # normalize into 0-1 range
        x = (np.max(x_cords) - np.min(x_cords))
        y = (np.max(y_cords) - np.min(y_cords))
        dividers = x if x > y else y
        x_cords = (x_cords - np.min(x_cords)) / dividers
        y_cords = (y_cords - np.min(y_cords)) / dividers
        if self.created == 0:  # used for updating cuz of memory leaks
            self.fig_points(x_cords, y_cords)
        else:
            self.points.remove()
            self.ax.clear()
            self.fig_points(x_cords, y_cords)
        self.is_solving = False

    # function which will generate list of class Place for random points
    def generate_points(self):
        self.is_solving = True
        time.sleep(0.01)
        self.list_cords.clear()
        entry_value = self.places_entry_value.get()
        try:  # try if we can convert to int
            entry_value = int(entry_value)
        except:  # if we cant error message
            tkinter.messagebox.showerror("showerror", "Wrong format of places count\n"
                                         + "Right format is [int] > 1")
            self.is_solving = False
            return
        if entry_value < 2:
            tkinter.messagebox.showerror("showerror", "Wrong format of places count\n"
                                         + "Right format is [int] > 1")
            self.is_solving = False
            return

        x_cords = np.random.rand(entry_value)
        y_cords = np.random.rand(entry_value)
        for i in range(len(x_cords)):
            vertex = Place(i, x_cords[i], y_cords[i], [])
            self.list_cords.append(vertex)

        size = len(self.list_cords)
        for i in range(size):
            if self.stop:
                self.stop = False
                self.is_solving = False
                return
            dist_list = []
            for v in self.list_cords:
                dist = math.sqrt((self.list_cords[i].x - v.x) ** 2 + (self.list_cords[i].y - v.y) ** 2) * 100
                dist_list.append(dist)
            self.list_cords[i].distance = np.array(dist_list)
        if self.created == 0:  # used for updating cuz of memory leaks
            self.fig_points(x_cords, y_cords)
        else:
            self.points.remove()
            self.ax.clear()
            self.fig_points(x_cords, y_cords)
        self.is_solving = False


if __name__ == "__main__":
    app = App()
    app.mainloop()
