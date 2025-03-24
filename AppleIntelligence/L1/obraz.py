import tensorflow as tf
import matplotlib.pyplot as plt

# Załadowanie danych MNIST
(x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()

# Sprawdzanie rozmiaru zbioru
print(f"Zbiór treningowy: {x_train.shape}, Zbiór testowy: {x_test.shape}")

plt.imshow(x_train[1000], cmap='gray')  # Pierwszy obraz w zbiorze treningowym
plt.title(f"Label: {y_train[0]}")  # Etykieta obrazu
plt.show()
plt.savefig('train_img0.png')  # Zapisz wykres do pliku
