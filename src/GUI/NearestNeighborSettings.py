import customtkinter


class NearestNeighborSettings(customtkinter.CTk):
    WIDTH = 720
    HEIGHT = 720
    INSTANTLY = -1
    list_ap = []
    animation_lock = False

    def __init__(self, root, **kwargs):
        super().__init__(**kwargs)
        self.root = root
        self.window = customtkinter.CTkToplevel(root)
        self.window.slider_value = 500
        self.window.protocol("WM_DELETE_WINDOW", lambda: self.save())
        self.window.title("TSP test application")
        self.window.geometry(f"{self.WIDTH}x{self.HEIGHT}")
        self.window.grid_columnconfigure(1, weight=1)
        self.window.grid_rowconfigure(7, weight=5)
        animation_label_font = customtkinter.CTkFont(size=25, weight="bold")
        self.window.animation_label = customtkinter.CTkLabel(
            self.window, text="Animation settings", font=animation_label_font)
        self.window.animation_label.grid(
            row=0, column=0, padx=20, pady=(10, 0))
        self.window.animation = customtkinter.CTkRadioButton(self.window, text="Enable animation",
                                                             command=self.animation_event, value=1)
        self.window.animation.grid(row=1, column=0, padx=20, pady=(10, 10))
        self.window.animation_speed_label = customtkinter.CTkLabel(
            self.window, text="")
        self.window.animation_speed_label.grid(
            row=2, column=0, padx=20, pady=(10, 10))

        self.window.save_button = customtkinter.CTkButton(
            self.window, text="Save", command=self.save)
        self.window.save_button.grid(
            row=8, column=2, padx=(10, 50), pady=(0, 40))
        if self.root.animation_speed != -1:
            self.animation_event()

    def save(self):
        if self.animation_lock:
            self.root.animation_speed = self.window.slider_value
        else:
            self.root.animation_speed = self.INSTANTLY
        self.root.settings_opened = False
        self.window.destroy()
        self.destroy()

    def slider_event(self, value):
        self.window.slider_value = int(value)
        self.window.animation_speed_label.configure(
            text="Animation Speed: " + str(int(value)) + "ms")

    def animation_event(self):
        if self.animation_lock:
            self.window.animation.deselect()
            self.window.animation_speed.destroy()
            self.window.animation_speed_label.configure(text="")
            self.animation_lock = False
        else:
            self.window.animation.select()
            self.window.animation_speed = customtkinter.CTkSlider(
                self.window, from_=0, to=1000, number_of_steps=1000, command=self.slider_event)
            self.window.animation_speed.grid(
                row=3, column=0, padx=20, pady=(0, 20))
            self.window.animation_speed_label.configure(
                text="Animation speed: " + str(self.window.slider_value) + "ms")
            self.animation_lock = True
