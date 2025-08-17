import numpy as np

# Load your file (space-separated, adjust if needed)
data = np.loadtxt(r"C:\Users\acer\Smart-ECG-Holter-Monitor\ML_model\data_sets\data_ecg_small.txt")

# Select column 1 (index 0 if first column, 1 if second, etc.)
col1 = data[:, 0]

# Save column as a single comma-separated row
np.savetxt(
    r"C:\Users\acer\Smart-ECG-Holter-Monitor\ML_model\data_sets\data_ecg_small_ch1.txt",
    [col1],           # wrap in list to make it a row instead of column
    fmt='%d',
    delimiter=','
)