import numpy as np
import cv2
import os
import tensorflow as tf

folder = "L1/MyNums/PNG"

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

#processed_images = np.array(processed_images).reshape(len(processed_images), 28, 28)
processed_images = np.array(processed_images).reshape(len(processed_images), 28 * 28)  # Flatten 28x28 to 784
#processed_images = processed_images / 255.0  # Normalize to [0, 1]

true_labels = np.array(true_labels)

# One-hot encode labels
def one_hot(y, num_classes=10):
    return np.eye(num_classes)[y]

true_labels_oh = one_hot(true_labels)

mnist = tf.keras.datasets.mnist

(x_train, y_train), (x_test, y_test) = mnist.load_data()

# Normalize input data and flatten 28x28 -> 784
x_train = x_train.reshape(-1, 28*28) / 255.0
x_test = x_test.reshape(-1, 28*28) / 255.0



y_train_oh = one_hot(y_train)
y_test_oh = one_hot(y_test)

class Dense:
    def __init__(self, input_size, output_size):
        self.W = np.random.randn(input_size, output_size) * 0.01
        self.b = np.zeros((1, output_size))
        
    def forward(self, X):
        self.input = X
        return np.dot(X, self.W) + self.b

    def backward(self, grad_output, learning_rate):
        grad_input = np.dot(grad_output, self.W.T)
        grad_W = np.dot(self.input.T, grad_output)
        grad_b = np.sum(grad_output, axis=0, keepdims=True)
        
        # Gradient descent step
        self.W -= learning_rate * grad_W
        self.b -= learning_rate * grad_b

        return grad_input

class ReLU:
    def forward(self, X):
        self.input = X
        return np.maximum(0, X)

    def backward(self, grad_output, learning_rate):
        return grad_output * (self.input > 0)

class Softmax:
    def forward(self, X):
        exps = np.exp(X - np.max(X, axis=1, keepdims=True))
        self.out = exps / np.sum(exps, axis=1, keepdims=True)
        return self.out

    def backward(self, grad_output, learning_rate):
        # Assume cross-entropy loss handles this
        return grad_output

class CrossEntropyLoss:
    def forward(self, preds, labels):
        # Clip predictions to prevent log(0)
        self.preds = np.clip(preds, 1e-9, 1 - 1e-9)
        self.labels = labels
        return -np.mean(np.sum(labels * np.log(self.preds), axis=1))

    def backward(self):
        return (self.preds - self.labels) / self.labels.shape[0]

# Generate dummy data (10 samples, 2 features, 2 classes)
X = np.random.randn(10, 2)
y = np.array([[1, 0] if x[0] > 0 else [0, 1] for x in X])  # simple class separation

#X = x_train

# Initialize layers
layer1 = Dense(784, 128)
activation1 = ReLU()
layer2 = Dense(128, 10)
activation2 = Softmax()
loss_fn = CrossEntropyLoss()

# Training loop
epochs = 10
batch_size = 64
learning_rate = 0.1

for epoch in range(epochs):
    # Shuffle dataset
    indices = np.arange(x_train.shape[0])
    np.random.shuffle(indices)
    x_train, y_train_oh = x_train[indices], y_train_oh[indices]

    for i in range(0, x_train.shape[0], batch_size):
        X_batch = x_train[i:i+batch_size]
        y_batch = y_train_oh[i:i+batch_size]

        # Forward
        out1 = layer1.forward(X_batch)
        out2 = activation1.forward(out1)
        out3 = layer2.forward(out2)
        preds = activation2.forward(out3)

        # Loss
        loss = loss_fn.forward(preds, y_batch)

        # Backward
        grad_loss = loss_fn.backward()
        grad_out3 = activation2.backward(grad_loss, learning_rate)
        grad_out2 = layer2.backward(grad_out3, learning_rate)
        grad_out1 = activation1.backward(grad_out2, learning_rate)
        _ = layer1.backward(grad_out1, learning_rate)

    print(f"Epoch {epoch+1}, Loss: {loss:.4f}")


# Function to make predictions on new data (e.g., test data)
def predict(X):
    out1 = layer1.forward(X)              # Forward pass through layer 1
    out2 = activation1.forward(out1)      # Apply ReLU activation
    out3 = layer2.forward(out2)           # Forward pass through layer 2
    preds = activation2.forward(out3)     # Apply Softmax to get probabilities
    return preds

# Make predictions on processed_images
processed_preds = predict(processed_images)

# Get the predicted class (the one with the highest probability)
predicted_classes = np.argmax(processed_preds, axis=1)

# Compare with true labels (the one-hot encoded true_labels)
true_classes = np.argmax(true_labels_oh, axis=1)

# Calculate accuracy
for pred, true in zip(predicted_classes, true_classes):
    print(f"{pred:8} | {true:4}")
accuracy = np.mean(predicted_classes == true_classes)
print(f"Prediction Accuracy on processed_images: {accuracy * 100:.2f}%")

