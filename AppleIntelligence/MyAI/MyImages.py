import numpy as np
import cv2
import os
import matplotlib.pyplot as plt

folder = "../L1/MyNums/PNG"

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

# Load and process images
for file in image_files:
    img_path = os.path.join(folder, file)

    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print(f" Błąd wczytywania obrazu: {file}, pomijam...")
        continue

    img = cv2.resize(img, (28, 28))  # Resize to 28x28
    
    _, img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY_INV)  # Apply binary inverse threshold

    img = img / 255.0  # Normalize to [0, 1]

    processed_images.append(img)
    true_labels.append(labels.get(file, -1))

print(f"Przetworzono {len(processed_images)} obrazów")

processed_images = np.array(processed_images)

# Display first 5 images
plt.figure(figsize=(10, 5))
for i in range(min(5, len(processed_images))):
    plt.subplot(1, 5, i + 1)
    plt.imshow(processed_images[i], cmap='gray')  # Show image in grayscale
    plt.title(f"Label: {true_labels[i]}")  # Show label
    plt.axis('off')

# Save the plot as a PNG file
plt.savefig('my_images.png')

# Optionally, close the plot to release memory
plt.close()
