from flask import Flask, request, jsonify, render_template
import boto3
import json
import numpy as np
import joblib

app = Flask(__name__)

target_len = 120

scaler = joblib.load("scaler.pkl")  # saved after training

# Initialize SageMaker client
sagemaker_runtime = boto3.client("sagemaker-runtime", region_name="ap-south-1")

# Replace with your SageMaker endpoint name
ENDPOINT_NAME = "ecg-cnn-endpoint-20250824-152427"

@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":
        try:
            # Example: input as comma-separated ECG values
            raw_input = request.form["ecg_values"]
            ecg_data = np.array([float(x) for x in raw_input.split(",")])

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

            # Reshape & prepare payload (adjust to your model input format)
            payload = ecg_data.reshape(1, target_len, 1)

            payload = {"instances": payload.tolist()}

            response = sagemaker_runtime.invoke_endpoint(
                EndpointName=ENDPOINT_NAME,
                ContentType="application/json",
                Body=json.dumps(payload)
            )

            result = json.loads(response["Body"].read().decode())
            return jsonify({"prediction": result})

        except Exception as e:
            return jsonify({"error": str(e)})

    return '''
        <form method="post">
            <label>Enter ECG values (comma-separated):</label><br>
            <input type="text" name="ecg_values" size="100"><br><br>
            <input type="submit" value="Predict">
        </form>
    '''

if __name__ == "__main__":
    app.run(debug=True)
