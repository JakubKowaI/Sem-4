import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf

# Load the MNIST dataset
mnist = tf.keras.datasets.mnist
(x_train, y_train), (x_test, y_test) = mnist.load_data()

# Plot the first 5 images from the training set
plt.figure(figsize=(20, 15))
for i in range(15):
    plt.subplot(2, 15, i+1)  # 1 row, 5 columns
    plt.imshow(x_train[i+1000], cmap='gray')  # Display image
    plt.title(f"Label: {y_train[i+1000]}")    # Display label
    plt.axis('off')  # Hide axes

# Save the plot as a PNG file
plt.savefig('mnist_images2.png')

# Optionally, close the plot to release memory
plt.close()
