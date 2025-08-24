# inference.py - UPDATED FOR SAVEDMODEL FORMAT
import json
import os
import tensorflow as tf
from tensorflow import keras
import numpy as np
import logging
import sys

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    stream=sys.stdout
)
logger = logging.getLogger(__name__)

def model_fn(model_dir):
    """
    Load the model from SavedModel format
    """
    try:
        logger.info(f"🚀 Loading model from: {model_dir}")
        
        # List all files for debugging
        all_files = []
        for root, dirs, files in os.walk(model_dir):
            for file in files:
                all_files.append(os.path.join(root, file))
        logger.info(f"📁 Files in model directory: {all_files}")
        
        # Look for SavedModel directory
        saved_model_path = os.path.join(model_dir, 'saved_model')
        if not os.path.exists(saved_model_path):
            # Check if model_dir itself is the SavedModel
            if any(f.endswith('saved_model.pb') for f in all_files):
                saved_model_path = model_dir
            else:
                raise FileNotFoundError(f"No SavedModel found. Available files: {all_files}")
        
        logger.info(f"🔍 Loading SavedModel from: {saved_model_path}")
        model = keras.models.load_model(saved_model_path)
        logger.info("✅ Model loaded successfully!")
        
        # Test the model
        try:
            if hasattr(model, 'input_shape') and model.input_shape:
                sample_shape = [1] + [dim if dim is not None else 1 for dim in model.input_shape[1:]]
                sample_input = tf.random.normal(sample_shape, mean=0.0, stddev=0.1)
                prediction = model.predict(sample_input)
                logger.info(f"🔥 Warm-up successful! Output shape: {prediction.shape}")
        except Exception as e:
            logger.warning(f"⚠️ Warm-up failed: {e}")
        
        return model
        
    except Exception as e:
        logger.error(f"💥 MODEL LOADING FAILED: {str(e)}", exc_info=True)
        raise

def input_fn(request_body, content_type='application/json'):
    try:
        logger.info(f"📥 Received content_type: {content_type}")
        
        # Handle health check
        if not request_body.strip() or request_body.strip().lower() in ['ping', 'healthcheck']:
            logger.info("🏥 Health check request")
            return tf.constant([[0.0]], dtype=tf.float32)
        
        if content_type == 'application/json':
            data = json.loads(request_body)
            
            # Extract inputs
            if 'instances' in data:
                inputs = data['instances']
            elif 'inputs' in data:
                inputs = data['inputs']
            else:
                inputs = data
            
            input_tensor = tf.convert_to_tensor(inputs, dtype=tf.float32)
            logger.info(f"📐 Input shape: {input_tensor.shape}")
            
            return input_tensor
            
        else:
            raise ValueError(f"Unsupported content type: {content_type}")
            
    except Exception as e:
        logger.error(f"💥 INPUT ERROR: {str(e)}", exc_info=True)
        raise

def predict_fn(input_data, model):
    try:
        logger.info(f"🔮 Predict with input shape: {input_data.shape}")
        
        # Handle health check
        if input_data.shape == (1, 1) and input_data.numpy()[0][0] == 0.0:
            logger.info("🏥 Health check - returning success")
            return tf.constant([[1.0]], dtype=tf.float32)
        
        # Add batch dimension if needed
        if len(input_data.shape) == 1:
            input_data = tf.expand_dims(input_data, 0)
            logger.info(f"📦 Added batch dimension: {input_data.shape}")
        
        # Make prediction
        prediction = model.predict(input_data)
        logger.info(f"✅ Prediction successful! Shape: {prediction.shape}")
        
        return prediction
        
    except Exception as e:
        logger.error(f"💥 PREDICTION ERROR: {str(e)}", exc_info=True)
        raise

def output_fn(prediction, accept='application/json'):
    try:
        logger.info(f"📤 Output with prediction shape: {prediction.shape}")
        
        if accept == 'application/json':
            if hasattr(prediction, 'numpy'):
                prediction = prediction.numpy()
            
            response = {'predictions': prediction.tolist()}
            return json.dumps(response)
            
        else:
            raise ValueError(f"Unsupported accept type: {accept}")
            
    except Exception as e:
        logger.error(f"💥 OUTPUT ERROR: {str(e)}", exc_info=True)
        raise