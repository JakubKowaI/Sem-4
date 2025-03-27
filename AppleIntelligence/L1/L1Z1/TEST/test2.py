import tensorflow as tf
import cv2
import numpy as np
import os
from sklearn.metrics import classification_report
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'

folder = "../../MyNums/PNG"

labels = {}
labels_path = os.path.join(folder, "labels.txt")


if not os.path.exists(labels_path):
    raise FileNotFoundError(f"Plik {labels_path} nie istnieje. Sprawdź ścieżkę!")

with open(labels_path, "r") as f:
    for line in f:
        filename, label = line.strip().split()
        labels[filename] = int(label)

image_files = sorted([f for f in os.listdir(folder) if f.endswith((".png"))])

if not image_files:
    raise ValueError("Nie znaleziono żadnych plików .png ani .jpg w folderze!")

processed_images = []
true_labels = []

for file in image_files:
    img_path = os.path.join(folder, file)

    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print(f" Błąd wczytywania obrazu: {file}, pomijam...")
        continue

    img = cv2.resize(img, (28, 28))  
    
    _, img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY_INV)

    
    #img = 255 - img  # Odwrócenie kolorów
    img = img / 255.0  

    #img = cv2.GaussianBlur(img, (5, 5), 0)

    processed_images.append(img)
    true_labels.append(labels.get(file, -1))

print(f"Przetworzono {len(processed_images)} obrazów")

processed_images = np.array(processed_images).reshape(len(processed_images), 28, 28)
true_labels = np.array(true_labels)


mnist = tf.keras.datasets.mnist

(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0

model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(28, 28)),
  tf.keras.layers.Dense(128, activation='relu'),
  tf.keras.layers.Dropout(0.2),
  tf.keras.layers.Dense(10)
])



loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

model.compile(optimizer='adam',
              loss=loss_fn,
              metrics=['accuracy'])

model.fit(x_train, y_train, epochs=5)

predictions = model.predict(processed_images)

for i in range(len(processed_images)):
    true_label = true_labels[i]
    predicted_label = np.argmax(predictions[i])  
    confidence = np.max(predictions[i])  

    print(f"Test File {i+1}:")
    print(f"True Label: {true_label}, Predicted Label: {predicted_label}, Confidence: {confidence:.4f}")
    print("-" * 40)

model.evaluate(processed_images,  true_labels, verbose=2)

y_pred = np.argmax(model.predict(processed_images), axis=1)
print(classification_report(true_labels, y_pred))
