import numpy as np
from sagemaker.tensorflow import TensorFlowPredictor
import joblib  # for loading saved scaler

ENDPOINT_NAME = "ecg-cnn-endpoint-20250824-152427"
target_len = 120

# Step 2: Load saved scaler
# ----------------------------
scaler = joblib.load("scaler.pkl")  # saved after training

# Example ECG data
ecg_data = np.array([3037440,3037480,3037411,3037476,3037499,3037525,3037453,3037476,3037492,3037508,3037524,3037507,3037477,3037488,3037474,3037492,3037457,3037463,3037487,3037440,3037420,3037695,3037824,3037813,3037773,3037690,3037432,3037524,3037496,3037498,3037408,3037502,3037365,3040060,3038121,3037300,3037523,3037482,3037469,3037494,3037535,3037555,3037564,3037618,3037639,3037725,3037703,3037855,3037900,3038028,3038199,3038125,3038114,3037886,3037559,3037496,3037499,3037450,3037530,3037444,3037441,3037496,3037485,3037408,3037461,3037450,3037413,3037467,3037449], dtype=float)

# -----------------------------
# Step 1: Normalize ECG data (min-max)
# -----------------------------
ecg_data = (ecg_data - np.min(ecg_data)) / (np.max(ecg_data) - np.min(ecg_data))

# -----------------------------
# Step 2: Resize to model input length
# -----------------------------
def resize_signal(signal, target_len):
    old_indices = np.linspace(0, 1, len(signal))
    new_indices = np.linspace(0, 1, target_len)
    return np.interp(new_indices, old_indices, signal)

ecg_data = resize_signal(ecg_data, target_len)
ecg_data_2d = ecg_data.reshape(1, -1)        # reshape for scaler
ecg_data_scaled = scaler.transform(ecg_data_2d)
ecg_data = ecg_data_scaled.flatten()

input_data = ecg_data.reshape(1, target_len, 1)  # shape: [batch, steps, channels]

payload = {"instances": input_data.tolist()}

predictor = TensorFlowPredictor(endpoint_name=ENDPOINT_NAME)

try:
    # Call the endpoint
    result = predictor.predict(payload)
    print("Raw result:", result)

    # Extract predictions
    predictions = np.array(result['predictions'])  # shape: (1,1) for your single ECG

    # Apply threshold for binary classification
    predicted_labels = (predictions > 0.5).astype(int)
    label = 'Abnormal' if predicted_labels == 1 else 'Normal'
    print("Predicted labels:", predicted_labels)
    print("ECG signal prediction:", label)
except Exception as e:
    print("Error calling endpoint:", e)
