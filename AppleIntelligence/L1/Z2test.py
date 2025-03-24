import tensorflow as tf
import numpy as np
import cv2
import os
import matplotlib.pyplot as plt

# 📂 Folder z własnymi zdjęciami
folder = "MyNums/PNG"

# Wczytanie etykiet z pliku labels.txt
labels = {}
labels_path = os.path.join(folder, "labels.txt")

if not os.path.exists(labels_path):
    raise FileNotFoundError(f"Plik {labels_path} nie istnieje. Sprawdź ścieżkę!")

with open(labels_path, "r") as f:
    for line in f:
        filename, label = line.strip().split()
        labels[filename] = int(label)

# Lista nazw plików (tylko te z obrazami)
image_files = sorted([f for f in os.listdir(folder) if f.endswith((".png", ".jpg"))])

if not image_files:
    raise ValueError("Nie znaleziono żadnych plików .png ani .jpg w folderze!")

# Listy do przechowywania obrazów i etykiet
processed_images = []
true_labels = []

# Wczytanie i przetworzenie obrazów
for file in image_files:
    img_path = os.path.join(folder, file)

    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print(f"⚠️ Błąd wczytywania obrazu: {file}, pomijam...")
        continue

    img = cv2.resize(img, (28, 28))  # Skalowanie do 28x28
    
    # Zastosowanie progowania (thresholding)
    # Oznaczymy tło na czarne (wartości poniżej progu na 0), a cyfrę na białą (wartości powyżej progu na 255)
    _, img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY_INV)

    
    #img = 255 - img  # Odwrócenie kolorów
    img = img / 255.0  # Normalizacja

    processed_images.append(img)
    true_labels.append(labels.get(file, -1))  # Pobranie etykiety (jeśli brak, -1)

# Sprawdzenie, czy przetworzono obrazy
print(f"Przetworzono {len(processed_images)} obrazów")

# Konwersja do tablicy NumPy
processed_images = np.array(processed_images).reshape(len(processed_images), 28, 28)

# 📌 Wyświetlenie obrazów w siatce
fig, axes = plt.subplots(4, 5, figsize=(10, 8))  # 4x5 siatka

# Jeśli masz mniej niż 20 obrazów, dostosuj liczbę wierszy i kolumn w siatce
num_images = len(processed_images)
for i, ax in enumerate(axes.flat):
    if i < num_images:
        ax.imshow(processed_images[i], cmap="gray")
        ax.set_title(f"{image_files[i]}\nLabel: {true_labels[i]}")
    ax.axis("off")

plt.tight_layout()
plt.show()
plt.savefig('output_image.png')  # Zapisz wykres do pliku


print(f"Kształt processed_images: {processed_images.shape}")

# 🔮 Przewidywanie cyfr za pomocą modelu
model_path = "mnist_model.keras"
if not os.path.exists(model_path):
    raise FileNotFoundError(f"Model {model_path} nie istnieje!")

model = tf.keras.models.load_model(model_path)
predictions = np.argmax(model.predict(processed_images), axis=1)

# 📢 Wyniki klasyfikacji
correct = sum(p == t for p, t in zip(predictions, true_labels))
total = len(predictions)

print("\n📊 Wyniki klasyfikacji:")
for file, prediction, true_label in zip(image_files, predictions, true_labels):
    print(f"Plik: {file} → Przewidziana cyfra: {prediction}, Prawdziwa cyfra: {true_label}")

accuracy = correct / total * 100
print(f"\n✅ Dokładność modelu na własnych zdjęciach: {accuracy:.2f}%")
