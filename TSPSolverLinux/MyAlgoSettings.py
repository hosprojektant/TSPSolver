import tkinter
import tkinter.messagebox

import customtkinter


class MyAlgoSettings():
    WIDTH = 1800
    HEIGHT = 600
    INSTANTLY = -1
    list_ap = []
    animation_lock = False
    rounding = "0\n"
    segments = "0\n"

    def __init__(self, root, **kwargs):
        super().__init__(**kwargs)
        self.root = root
        self.window = customtkinter.CTkToplevel(root)
        self.window.slider_value = 500
        self.window.protocol("WM_DELETE_WINDOW", lambda: self.quit())
        self.window.title("TSP test application")
        self.window.geometry(f"{self.WIDTH}x{self.HEIGHT}")
        self.window.grid_columnconfigure(4, weight=1)
        self.window.grid_rowconfigure(7, weight=5)
        self.font_system_ui = customtkinter.CTkFont(family="system-ui", size=25, weight="bold")

        pad_x = 50
        pad_y = 10
        split_setup = self.root.MyAlgo_settings_setup.split("\n")
        self.start_nn_size_value = tkinter.StringVar(value=split_setup[0])
        self.start_nn_size_label = customtkinter.CTkLabel(
            self.window, text="Start NN Size(int)", font=self.font_system_ui)
        self.start_nn_size_label.grid(row=0, column=0, padx=pad_x, pady=pad_y)
        self.start_nn_size_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.start_nn_size_value, justify="center", font=self.font_system_ui)
        self.start_nn_size_entry.grid(row=0, column=1, padx=pad_x, pady=pad_y)

        self.middle_nn_size_value = tkinter.StringVar(value=split_setup[1])
        self.middle_nn_size_label = customtkinter.CTkLabel(
            self.window, text="Middle NN Size(int)", font=self.font_system_ui)
        self.middle_nn_size_label.grid(row=1, column=0, padx=pad_x, pady=pad_y)
        self.middle_nn_size_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.middle_nn_size_value, justify="center", font=self.font_system_ui)
        self.middle_nn_size_entry.grid(row=1, column=1, padx=pad_x, pady=pad_y)

        self.last_nn_size_value = tkinter.StringVar(value=split_setup[2])
        self.last_nn_size_label = customtkinter.CTkLabel(
            self.window, text="Last NN Size(int)", font=self.font_system_ui)
        self.last_nn_size_label.grid(row=2, column=0, padx=pad_x, pady=pad_y)
        self.last_nn_size_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.last_nn_size_value, justify="center", font=self.font_system_ui)
        self.last_nn_size_entry.grid(row=2, column=1, padx=pad_x, pady=pad_y)

        self.start_nn_count_value = tkinter.StringVar(value=split_setup[3])
        self.start_nn_count_label = customtkinter.CTkLabel(
            self.window, text="Start NN Count(int)", font=self.font_system_ui)
        self.start_nn_count_label.grid(row=3, column=0, padx=pad_x, pady=pad_y)
        self.start_nn_count_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.start_nn_count_value, justify="center", font=self.font_system_ui)
        self.start_nn_count_entry.grid(row=3, column=1, padx=pad_x, pady=pad_y)

        self.rounding_ee_label = customtkinter.CTkLabel(
            self.window, text="Rounding", font=self.font_system_ui)
        self.rounding_ee_label.grid(row=4, column=0, padx=pad_x, pady=pad_y)
        self.rounding_ee = customtkinter.CTkRadioButton(self.window, text="",
                                                        command=self.rounding_event, value=1)
        self.rounding_ee.grid(row=4, column=1, padx=(75, 0), pady=(10, 10))

        self.segments_ee_label = customtkinter.CTkLabel(
            self.window, text="Segments", font=self.font_system_ui)
        self.segments_ee_label.grid(row=5, column=0, padx=pad_x, pady=pad_y)
        self.segments_ee = customtkinter.CTkRadioButton(self.window, text="",
                                                        command=self.segments_event, value=1)
        self.segments_ee.grid(row=5, column=1, padx=(75, 0), pady=(10, 10))
        if split_setup[4] == "1":
            self.rounding_ee.select()
            self.rounding = "1\n"
        if split_setup[5] == "1":
            self.segments_ee.select()
            self.segments = "1\n"

        self.window.save_button = customtkinter.CTkButton(
            self.window, text="Save", command=self.save)
        self.window.save_button.grid(
            row=7, column=3, padx=(10, 50), pady=(0, 40))

    def segments_event(self):
        if self.segments == "1\n":
            self.segments_ee.deselect()
            self.segments = "0\n"
        else:
            self.segments_ee.select()
            self.segments = "1\n"

    def rounding_event(self):
        if self.rounding == "1\n":
            self.rounding_ee.deselect()
            self.rounding = "0\n"
        else:
            self.rounding_ee.select()
            self.rounding = "1\n"

    def quit(self):
        self.root.settings_opened = False
        self.window.destroy()
        self.destroy()

    def save(self):
        widgets = [self.start_nn_size_value, self.middle_nn_size_value, self.last_nn_size_value,
                   self.start_nn_count_value, ]
        # float check
        wrong_format = False

        int_values = []
        ints = ["Start NN Size", "Middle NN Size", "Last NN Size", "Start NN Count"]
        for w, s in zip(widgets, ints):
            try:
                int_values.append(int(w.get()))
            except ValueError:  # cant convert
                tkinter.messagebox.showerror("showerror", "Wrong format of " + s + "\n"
                                             + "Right format is int value like 130")
                wrong_format = True

        self.window.deiconify()  # maximize
        if not wrong_format:
            returned_string = ""
            for i, s in zip(int_values, ints):
                if i < 0:
                    tkinter.messagebox.showerror("showerror", "Wrong format of " + s + "\n"
                                                 + "Right format is value >= 0 ")
                    returned_string = self.root.ACO_settings_setup
                    break
                returned_string += str(i) + "\n"
            self.root.MyAlgo_settings_setup = returned_string + self.rounding + self.segments
            self.quit()
        # int check
