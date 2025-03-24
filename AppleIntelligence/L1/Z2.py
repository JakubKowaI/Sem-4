import tensorflow as tf
import numpy as np
import cv2
import os
import matplotlib.pyplot as plt

# 📂 Folder z własnymi zdjęciami
folder = "MyNums/PNG"

# Wczytanie etykiet z pliku labels.txt
labels = {}
with open(os.path.join(folder, "labels.txt"), "r") as f:
    for line in f:
        filename, label = line.strip().split()  # Rozdzielanie nazwy pliku i cyfry
        labels[filename] = int(label)  # Konwersja cyfry na liczbę całkowitą

# Lista nazw plików (tylko te z obrazami)
image_files = sorted([f for f in os.listdir(folder) if f.endswith(".png") or f.endswith(".jpg")])

# Lista do przechowywania przetworzonych obrazów
processed_images = []
true_labels = []  # Lista rzeczywistych etykiet

# Wczytanie i przetworzenie obrazów
for file in image_files:
    img_path = os.path.join(folder, file)

    # 📥 Wczytanie obrazu w skali szarości
    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)

    # 🔄 Skalowanie do 28x28 (jeśli nie jest w tym rozmiarze)
    img = cv2.resize(img, (28, 28))

    # 🌓 Odwrócenie kolorów (czarne cyfry na białym tle, jak w MNIST)
    img = 255 - img

    # 🔍 Normalizacja wartości (0-1)
    img = img / 255.0

    # 📏 Zmiana kształtu
    processed_images.append(img)
    true_labels.append(labels[file])  # Pobranie rzeczywistej etykiety dla obrazu

plt.imshow(processed_images[0], cmap="gray")
plt.show()

# 📊 Konwersja do formatu NumPy i dostosowanie wymiarów
processed_images = np.array(processed_images).reshape(len(processed_images), 28, 28)

print(f"Kształt processed_images: {processed_images.shape}")


# 🔮 Przewidywanie cyfr za pomocą modelu
model = tf.keras.models.load_model("mnist_model.keras")  # Wczytanie modelu
predictions = np.argmax(model.predict(processed_images), axis=1)

# 📢 Wyniki i ocena dokładności
correct = 0
total = len(predictions)

print("\n📊 Wyniki klasyfikacji:")
for file, prediction, true_label in zip(image_files, predictions, true_labels):
    print(f"Plik: {file} → Przewidziana cyfra: {prediction}, Prawdziwa cyfra: {true_label}")
    if prediction == true_label:
        correct += 1

accuracy = correct / total * 100
print(f"\n✅ Dokładność modelu na własnych zdjęciach: {accuracy:.2f}%")
