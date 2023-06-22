import tkinter as tk


class LoadingCircle:
    def __init__(self, root, x, y, radius, thickness, text):
        self.root = root
        self.x = x
        self.hide_ = False
        self.y = y
        self.radius = radius
        self.thickness = thickness
        self.angle = 0
        self.visible = True
        self.text = text
        self.canvas = tk.Canvas(self.root, width=2 * self.radius, height=2 * self.radius)
        self.canvas.place(x=self.x - self.radius, y=self.y - self.radius)
        self.text_label = tk.Label(self.root, text=self.text)
        self.text_label.grid(row=1, column=1)
        self.draw()
        self.animate()

    def draw(self):
        if self.visible:
            self.canvas.delete("all")
            self.canvas.create_arc(self.thickness, self.thickness, 2 * self.radius - self.thickness,
                                   2 * self.radius - self.thickness, start=self.angle, extent=120, style="arc",
                                   width=self.thickness, outline="#408cd4")
            self.canvas.grid(row=1, column=1, )

    def animate(self):
        if self.hide_:
            return
        self.angle += 5
        if self.angle == 360:
            self.angle = 0
        self.draw()
        self.root.after(50, self.animate)

    def set_text(self, text):
        self.text = text

    def hide(self):
        self.canvas.delete("all")
        self.hide_ = True

    def show(self):
        self.canvas = tk.Canvas(self.root, width=2 * self.radius, height=2 * self.radius)
        self.hide_ = False
