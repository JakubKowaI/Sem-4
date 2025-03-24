import tensorflow as tf
from tensorflow import keras
import numpy as np
from sklearn.metrics import classification_report

# 1️⃣ Wczytanie zbioru danych MNIST
mnist = keras.datasets.mnist
(x_train, y_train), (x_test, y_test) = mnist.load_data()

# 2️⃣ Normalizacja danych (wartości od 0 do 1)
x_train, x_test = x_train / 255.0, x_test / 255.0

# 3️⃣ Budowa sieci neuronowej
model = keras.Sequential([
    keras.layers.Flatten(input_shape=(28, 28)),  # Spłaszczenie obrazu 28x28 do wektora
    keras.layers.Dense(128, activation='relu'),  # Warstwa ukryta
    keras.layers.Dense(10, activation='softmax') # Warstwa wyjściowa (10 klas dla cyfr 0-9)
])

# 4️⃣ Kompilacja modelu
model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])

# 5️⃣ Trenowanie modelu
model.fit(x_train, y_train, epochs=15, validation_data=(x_test, y_test))

# 6️⃣ Ocena modelu na zbiorze testowym
test_loss, test_accuracy = model.evaluate(x_test, y_test)
print(f"\nDokładność na zbiorze testowym: {test_accuracy:.4f}")

# 7️⃣ Obliczenie czułości i precyzji
y_pred = np.argmax(model.predict(x_test), axis=1)  # Przewidywania modelu
report = classification_report(y_test, y_pred, digits=4)
print("\nKlasyfikacja (czułość i precyzja):\n", report)

# 📀 Zapisanie modelu do pliku
model.save("mnist_model.keras")
print("✅ Model zapisany jako mnist_model.keras")
