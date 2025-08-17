import tkinter as tk
from tkinter import filedialog
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt

def to_signed24(x):
            if x & 0x800000:  # if sign bit (bit 23) is set
                x -= 0x1000000  # subtract 2^24 to sign-extend
            return x*2.4*2/(2**23)

def load_and_plot():
    file_path = filedialog.askopenfilename(filetypes=[("Text files", "*.txt"), ("Log files", "*.log")])
    if not file_path:
        return

    try:
        # Read and parse the 3-channel data
        ch1, ch2, ch3 = [], [], []
        with open(file_path, 'r') as file:
            for line in file:
                parts = line.strip().split()
                if len(parts) == 3:
                    ch1.append(to_signed24(int(parts[0])))
                    ch2.append(to_signed24(int(parts[1])))
                    ch3.append(to_signed24(int(parts[2])))

        # Clear previous plot
        ax.clear()
        ax.plot(ch1, label="Channel 1")
        ax.plot(ch2, label="Channel 2")
        ax.plot(ch3, label="Channel 3")
        ax.set_title("ECG Data - 3 Channels")
        ax.set_xlabel("Sample Index")
        ax.set_ylabel("Amplitude")
        ax.grid(True)
        ax.legend()

        canvas.draw()

    except Exception as e:
        print("Error loading file:", e)

# Create GUI
root = tk.Tk()
root.title("3-Channel ECG Viewer")

# Create matplotlib figure
fig, ax = plt.subplots(figsize=(10, 4))
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack()

# Buttons
btn_load = tk.Button(root, text="Load ECG Data", command=load_and_plot)
btn_load.pack(pady=10)

btn_exit = tk.Button(root, text="Exit", command=root.quit)
btn_exit.pack(pady=5)

# Run app
root.mainloop()
