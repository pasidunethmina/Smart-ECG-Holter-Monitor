import numpy as np

# Load your dataset (3 space-separated columns)
data = np.loadtxt("data_sets\data_ecg_small.txt")

# Extract column 1
col1 = data[:, 0]

# Save column 1 to a new text file
np.savetxt("data_sets/data_ecg_small_ch1.txt", col1, fmt='%d')

print("Column 1 saved to ecg_column1.txt")