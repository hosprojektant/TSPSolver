import multiprocessing as mp
import tkinter
import tkinter.messagebox
from idlelib.tooltip import Hovertip

import customtkinter


class ACOSettings(customtkinter.CTk):
    WIDTH = 1800
    HEIGHT = 600
    INSTANTLY = -1
    list_ap = []
    animation_lock = False

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
        split_setup = self.root.ACO_settings_setup.split("\n")
        print(self.root.ACO_settings_setup)
        self.Alpha_value = tkinter.StringVar(value=split_setup[0])
        self.Alpha_label = customtkinter.CTkLabel(
            self.window, text="Alpha(float)", font=self.font_system_ui)
        self.Alpha_label.grid(row=0, column=0, padx=pad_x, pady=pad_y)
        self.Alpha_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Alpha_value, justify="center", font=self.font_system_ui)
        self.Alpha_entry.grid(row=0, column=1, padx=pad_x, pady=pad_y)
        Hovertip(self.Alpha_entry, 'Range is (0,N)\nOptimal is (0.2,3)')

        self.Beta_value = tkinter.StringVar(value=split_setup[1])
        self.Beta_label = customtkinter.CTkLabel(
            self.window, text="Beta(float)", font=self.font_system_ui, )
        self.Beta_label.grid(row=1, column=0, padx=pad_x, pady=pad_y)
        self.Beta_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Beta_value, justify="center", font=self.font_system_ui)
        self.Beta_entry.grid(row=1, column=1, padx=pad_x, pady=pad_y)
        Hovertip(self.Beta_entry, 'Range is (0,N)\nOptimal is (1,30)')

        self.Tau0_value = tkinter.StringVar(value=split_setup[5])
        self.Tau0_label = customtkinter.CTkLabel(
            self.window, text="Tau0(float)", font=self.font_system_ui)
        self.Tau0_label.grid(row=2, column=0, padx=pad_x, pady=pad_y)
        self.Tau0_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Tau0_value, justify="center", font=self.font_system_ui)
        self.Tau0_entry.grid(row=2, column=1, padx=pad_x, pady=pad_y)
        Hovertip(self.Tau0_entry, 'Range is (0,1)\nOptimal is (0.0.5,0.4)')

        self.Rho_value = tkinter.StringVar(value=split_setup[2])
        self.Rho_label = customtkinter.CTkLabel(
            self.window, text="Rho(float)", font=self.font_system_ui)
        self.Rho_label.grid(row=3, column=0, padx=pad_x, pady=pad_y)
        self.Rho_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Rho_value, justify="center", font=self.font_system_ui)
        self.Rho_entry.grid(row=3, column=1, padx=pad_x, pady=pad_y)
        Hovertip(self.Rho_entry, 'Range is (0,1)\nOptimal is (0.1,0.5)')

        self.Q_value = tkinter.StringVar(value=split_setup[3])
        self.Q_label = customtkinter.CTkLabel(
            self.window, text="Q(float)", font=self.font_system_ui)
        self.Q_label.grid(row=4, column=0, padx=pad_x, pady=pad_y)
        self.Q_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Q_value, justify="center", font=self.font_system_ui)
        self.Q_entry.grid(row=4, column=1, padx=pad_x, pady=pad_y)
        Hovertip(self.Q_entry, 'Range is (0,N)\nOptimal is (1,100)')

        self.Q0_value = tkinter.StringVar(value=split_setup[4])
        self.Q0_label = customtkinter.CTkLabel(
            self.window, text="Q0(float)", font=self.font_system_ui)
        self.Q0_label.grid(row=5, column=0, padx=pad_x, pady=pad_y)
        self.Q0_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Q0_value, justify="center", font=self.font_system_ui)
        self.Q0_entry.grid(row=5, column=1, padx=pad_x, pady=pad_y)
        Hovertip(self.Q0_entry, 'Range is (0,1)\nOptimal is 0.2')

        self.Iterations_value = tkinter.StringVar(value=split_setup[7])
        self.Iterations_label = customtkinter.CTkLabel(
            self.window, text="Iterations(int)", font=self.font_system_ui)
        self.Iterations_label.grid(row=0, column=2, padx=pad_x, pady=pad_y)
        self.Iterations_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.Iterations_value, justify="center", font=self.font_system_ui)
        self.Iterations_entry.grid(row=0, column=3, padx=pad_x, pady=pad_y)
        Hovertip(self.Iterations_entry, 'Range is (0,N)\nOptimal is (200,2000)')

        self.agents_value = tkinter.StringVar(value=split_setup[8])
        self.agents_label = customtkinter.CTkLabel(
            self.window, text="Count of agents(int)", font=self.font_system_ui)
        self.agents_label.grid(row=1, column=2, padx=pad_x, pady=pad_y)
        self.agents_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.agents_value, justify="center", font=self.font_system_ui)
        self.agents_entry.grid(row=1, column=3, padx=pad_x, pady=pad_y)
        Hovertip(self.agents_entry, 'Range is (0,N)\nOptimal is (5,100)')

        self.NN_iteration_value = tkinter.StringVar(value=split_setup[6])
        self.NN_iteration = customtkinter.CTkLabel(
            self.window, text="NN iterations multiplayer(float)", font=self.font_system_ui)
        self.NN_iteration.grid(row=2, column=2, padx=pad_x, pady=pad_y)
        self.NN_iteration_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.NN_iteration_value, justify="center", font=self.font_system_ui)
        self.NN_iteration_entry.grid(row=2, column=3, padx=pad_x, pady=pad_y)
        Hovertip(self.NN_iteration_entry, 'places count * multiplayer\nRange is (0,N)\nOptimal is 1')

        self.NN_length_value = tkinter.StringVar(value=split_setup[9])
        self.NN_length = customtkinter.CTkLabel(
            self.window, text="NN length of path(int)", font=self.font_system_ui)
        self.NN_length.grid(row=3, column=2, padx=pad_x, pady=pad_y)
        self.NN_length_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.NN_length_value, justify="center", font=self.font_system_ui)
        self.NN_length_entry.grid(row=3, column=3, padx=pad_x, pady=pad_y)
        Hovertip(self.NN_length_entry, 'Range is (0,places count+1)\nOptimal is (1,4)')

        n_cores = mp.cpu_count()
        self.N_cores_value = tkinter.StringVar(value=split_setup[10])
        self.N_cores = customtkinter.CTkLabel(
            self.window, text="Number of cores(int)", font=self.font_system_ui)
        self.N_cores.grid(row=4, column=2, padx=pad_x, pady=pad_y)
        self.N_cores_entry = customtkinter.CTkEntry(
            self.window, textvariable=self.N_cores_value, justify="center", font=self.font_system_ui)
        self.N_cores_entry.grid(row=4, column=3, padx=pad_x, pady=pad_y)
        Hovertip(self.N_cores_entry, 'Range is (0,' + str(n_cores) + ')\nOptimal is ' + str(n_cores))

        self.window.save_button = customtkinter.CTkButton(
            self.window, text="Save", command=self.save)
        self.window.save_button.grid(
            row=7, column=3, padx=(10, 50), pady=(0, 40))

    def quit(self):
        self.root.settings_opened = False
        self.window.destroy()
        self.destroy()

    def save(self):
        widgets = [self.Alpha_value, self.Beta_value, self.Rho_value, self.Q_value, self.Q0_value, self.Tau0_value,
                   self.NN_iteration_value, self.Iterations_value, self.agents_value, self.NN_length_value,
                   self.N_cores_value]

        # float check
        float_values = []
        wrong_format = False
        floats = ["Alpha", "Beta", "Rho", "Q", "Q0", "Tau0", "NN iteration"]
        for w, s in zip(widgets[0:7], floats):
            try:
                float_values.append(float(w.get()))
            except ValueError:  # cant convert
                tkinter.messagebox.showerror("showerror", "Wrong format of " + s + "\n"
                                             + "Right format is float value like 2.344")
                wrong_format = True
        int_values = []
        ints = ["Iteration", "Agents", "NN length", "Number of cores"]
        for w, s in zip(widgets[7:], ints):
            try:
                int_values.append(int(w.get()))
            except ValueError:  # cant convert
                tkinter.messagebox.showerror("showerror", "Wrong format of " + s + "\n"
                                             + "Right format is int value like 130")
                wrong_format = True

        self.window.deiconify()  # maximize
        if not wrong_format:
            returned_string = ""
            for i, s in zip(float_values + int_values, floats + ints):
                if i < 0:
                    tkinter.messagebox.showerror("showerror", "Wrong format of " + s + "\n"
                                                 + "Right format is value >= 0 ")
                    returned_string = self.root.ACO_settings_setup
                    break
                returned_string += str(i) + "\n"
            self.root.ACO_settings_setup = returned_string
            self.quit()
        # int check
