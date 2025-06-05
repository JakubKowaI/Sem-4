from sklearn.cluster import DBSCAN
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import confusion_matrix
from sklearn.datasets import fetch_openml
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from collections import Counter, defaultdict

# Wczytanie zbioru MNIST (EMNIST digits można pobrać analogicznie z OpenML)
mnist = fetch_openml('mnist_784', version=1)
X = mnist.data
y_true = mnist.target.astype(int)

# Redukcja wymiaru do 30 dla przyspieszenia i odszumienia
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

pca = PCA(n_components=30)
X_pca = pca.fit_transform(X_scaled)

min_eps = None
min_min_samples = None
min_clusters = None
min_accuracy = None
min_misclassified = None
min_noise = 1.0
# Można dobrać eps i min_samples empirycznie
for eps in [2.5, 3.0, 3.5, 4.0, 4.5,5.0,5.5,6.0,6.5,7.0,7.5,8.0,8.5,9.0,9.5,10.0,10.5,11.0,11.5]:
    for min_samples in [3,4, 5,6, 7,8,9, 10,11,12]:
        
        dbscan = DBSCAN(eps=eps, min_samples=min_samples)
        labels = dbscan.fit_predict(X_pca)

        # Szum to etykieta -1
        n_noise = np.sum(labels == -1)
        n_clusters = len(set(labels)) - (1 if -1 in labels else 0)
        if n_clusters < 10:
            continue
        elif n_clusters>30:
            continue

        print(f"eps: {eps}")
        print(f"min_samples: {min_samples}")
        
        # Przypisz klasę większościową do każdego klastra


        label_map = defaultdict(lambda: -1)
        for cluster_id in set(labels):
            if cluster_id == -1:
                continue
            indices = np.where(labels == cluster_id)[0]
            majority_label = Counter(y_true[indices]).most_common(1)[0][0]
            label_map[cluster_id] = majority_label

        # Ocena klasyfikacji punktów nie będących szumem
        predicted_labels = np.array([label_map[lab] if lab != -1 else -1 for lab in labels])
        mask = predicted_labels != -1
        accuracy = np.mean(predicted_labels[mask] == y_true[mask])
        misclassified = np.sum(predicted_labels[mask] != y_true[mask]) / len(y_true[mask])
        noise_ratio = n_noise / len(X)

        if noise_ratio<min_noise:
            min_eps=eps
            min_min_samples=min_samples
            min_clusters=n_clusters
            min_accuracy=accuracy
            min_misclassified=misclassified
            min_noise=noise_ratio


        print(f"Liczba klastrów: {n_clusters}")
        print(f"Dokładność klasyfikacji (bez szumu): {accuracy:.4f}")
        print(f"Odsetek błędów (w klastrach): {misclassified:.4f}")
        print(f"Procent szumu: {noise_ratio:.4f}")

print(f"\nNajlepsze eps: {min_eps}")
print(f"Najlepsze min_samples: {min_min_samples}")
print(f"Liczba klastrów: {min_clusters}")
print(f"Dokładność klasyfikacji (bez szumu): {min_accuracy:.4f}")
print(f"Odsetek błędów (w klastrach): {min_misclassified:.4f}")
print(f"Procent szumu: {min_noise:.4f}")
