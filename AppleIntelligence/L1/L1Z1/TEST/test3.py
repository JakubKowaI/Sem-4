from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.datasets import fetch_openml
from sklearn.metrics import accuracy_score, classification_report
import os
import cv2
import numpy as np

# Ścieżka do folderu z obrazami i etykietami
folder = "../../MyNums/PNG"

# Wczytywanie etykiet
labels = {}
labels_path = os.path.join(folder, "labels.txt")

if not os.path.exists(labels_path):
    raise FileNotFoundError(f"Plik {labels_path} nie istnieje. Sprawdź ścieżkę!")

with open(labels_path, "r") as f:
    for line in f:
        filename, label = line.strip().split()
        labels[filename] = int(label)

# Wczytywanie plików obrazów
image_files = sorted([f for f in os.listdir(folder) if f.endswith((".png"))])

if not image_files:
    raise ValueError("Nie znaleziono żadnych plików .png w folderze!")

# Przygotowanie danych testowych
processed_images = []
true_labels = []

for file in image_files:
    img_path = os.path.join(folder, file)

    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print(f"Błąd wczytywania obrazu: {file}, pomijam...")
        continue

    img = cv2.resize(img, (28, 28))  # Dopasowanie rozmiaru obrazu do 28x28
    _, img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY_INV)
    
    img = img / 255.0  # Normalizacja obrazów (0-1)

    processed_images.append(img)
    true_labels.append(labels.get(file, -1))

print(f"Przetworzono {len(processed_images)} obrazów")

# Spłaszczenie obrazów z 28x28 do wektorów 784
processed_images = np.array(processed_images).reshape(len(processed_images), 28*28)
true_labels = np.array(true_labels)

# Wczytywanie zbioru MNIST
mnist = fetch_openml("mnist_784", version=1, as_frame=False)

x, y = mnist.data, mnist.target.astype(int)

# Podział danych MNIST na zbiór treningowy i testowy
X_train, X_test, y_train, y_test = train_test_split(x, y, test_size=0.2, random_state=42)

# Trenowanie modelu RandomForest na zbiorze MNIST
clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(X_train, y_train)

# Predykcja na własnych danych testowych
predictions = clf.predict(processed_images)

# Ocena modelu
accuracy = accuracy_score(true_labels, predictions)
print(f'Accuracy: {accuracy:.4f}')

# Raport klasyfikacji
report = classification_report(true_labels, predictions, output_dict=True, zero_division=1)

precision = report['weighted avg']['precision']
recall = report['weighted avg']['recall']

print(f'Precyzja: {precision:.4f}')
print(f'Czułość: {recall:.4f}')
