import tkinter as tk
from tkinter import filedialog
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt

def load_and_plot():
    file_path = filedialog.askopenfilename(filetypes=[("CSV files", "*.csv")])
    if not file_path:
        return

    try:
        # Read data
        with open(file_path, 'r') as file:
            content = file.read()
            # Convert comma-separated values to a list of floats
            data = [float(val.strip()) for val in content.replace('\n', ',').split(',') if val.strip()]

        # Clear previous plot
        print(data)
        ax.clear()
        ax.plot(data, label="ECG Signal")
        ax.set_title("ECG Plot")
        ax.set_xlabel("Time step")
        ax.set_ylabel("Amplitude")
        ax.grid(True)
        ax.legend()

        canvas.draw()
    except Exception as e:
        print("Error loading file:", e)

# Create GUI
root = tk.Tk()
root.title("ECG Viewer")

# Create matplotlib figure
fig, ax = plt.subplots(figsize=(8, 4))
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack()

# Load button
btn_load = tk.Button(root, text="Load ECG Data", command=load_and_plot)
btn_load.pack(pady=10)

# Run app
root.mainloop()
