import pandas as pd

# Read CSV file (no header assumed, change if needed)
df = pd.read_csv(r"C:\Users\acer\Smart-ECG-Holter-Monitor\ML_model\data_sets\input.csv", header=None)

# Function to rotate a row n times
def rotate_row(row):
    row = list(row)
    rotations = []
    for i in range(len(row)):
        rotated = row[-i:] + row[:-i]   # rotate by i
        rotated.append(0.00E+00)
        rotations.append(rotated)
    return rotations

# Generate all rotations for each row
all_rotations = []
for _, row in df.iterrows():
    all_rotations.extend(rotate_row(row))

# Convert back to DataFrame
result = pd.DataFrame(all_rotations)

# Save to CSV
result.to_csv("output.csv", index=False, header=False)

print("✅ Saved all rotations in output.csv")