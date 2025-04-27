"""
TensorFlow Lite Model Conversion Example
This script demonstrates how to convert a TensorFlow model to TensorFlow Lite format.
It includes a simple matrix multiplication model and shows how to save the converted model.
"""
import tensorflow as tf

# Create a simple MatMul model
class MatMulModel(tf.Module):
    @tf.function(input_signature=[
        tf.TensorSpec(shape=[3, 4], dtype=tf.float32),
        tf.TensorSpec(shape=[4, 3], dtype=tf.float32)
    ])
    def matmul(self, a, b):
        return tf.matmul(a, b)

# Instantiate the model
model = MatMulModel()

# Convert to TFLite
converter = tf.lite.TFLiteConverter.from_concrete_functions([model.matmul.get_concrete_function()])
tflite_model = converter.convert()

# Save the TFLite model
with open("matmul_model.tflite", "wb") as f:
    f.write(tflite_model)