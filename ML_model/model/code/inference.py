import json
import tensorflow as tf
from tensorflow import keras

def model_fn(model_dir):
    model = keras.models.load_model(f"{model_dir}/ecg_cnn_model.keras")
    return model

def input_fn(request_body, content_type='application/json'):
    data = json.loads(request_body)
    return tf.convert_to_tensor(data['inputs'])

def predict_fn(input_data, model):
    return model.predict(input_data)

def output_fn(prediction, accept='application/json'):
    return json.dumps(prediction.tolist())