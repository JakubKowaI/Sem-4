from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.datasets import fetch_openml
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter, defaultdict

def region_query(X, point_idx, eps):
    distances = np.linalg.norm(X - X[point_idx], axis=1)
    return np.where(distances <= eps)[0]

def expand_cluster(X, labels, point_idx, neighbors, cluster_id, eps, min_samples):
    labels[point_idx] = cluster_id
    i = 0
    while i < len(neighbors):
        neighbor_idx = neighbors[i]
        if labels[neighbor_idx] == -1:  # szum
            labels[neighbor_idx] = cluster_id
        elif labels[neighbor_idx] == 0:  # niezbadany
            labels[neighbor_idx] = cluster_id
            neighbor_neighbors = region_query(X, neighbor_idx, eps)
            if len(neighbor_neighbors) >= min_samples:
                neighbors = np.concatenate((neighbors, neighbor_neighbors))
        i += 1

def dbscan_custom(X, eps, min_samples):
    n = X.shape[0]
    labels = np.zeros(n, dtype=int)  # 0 = niezbadany, -1 = szum, >0 = ID klastra
    cluster_id = 0
    for point_idx in range(n):
        if labels[point_idx] != 0:
            continue
        neighbors = region_query(X, point_idx, eps)
        if len(neighbors) < min_samples:
            labels[point_idx] = -1  # szum
        else:
            cluster_id += 1
            expand_cluster(X, labels, point_idx, neighbors, cluster_id, eps, min_samples)
    return labels

# Wczytanie zbioru MNIST
mnist = fetch_openml('mnist_784', version=1)
X = mnist.data
y_true = mnist.target.astype(int)

# Redukcja wymiaru
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
pca = PCA(n_components=30)
X_pca = pca.fit_transform(X_scaled)

# Szukanie najlepszych parametrów
min_eps = None
min_min_samples = None
min_clusters = None
min_accuracy = None
min_misclassified = None
min_noise = 1.0

for eps in [3.0, 5.0, 7.0]:
    for min_samples in [5, 10]:

        print(f"eps: {eps}, min_samples: {min_samples}")
        labels = dbscan_custom(X_pca, eps=eps, min_samples=min_samples)

        n_noise = np.sum(labels == -1)
        unique_clusters = set(labels)
        if -1 in unique_clusters:
            unique_clusters.remove(-1)
        n_clusters = len(unique_clusters)
        if n_clusters < 10 or n_clusters > 30:
            continue

        label_map = defaultdict(lambda: -1)
        for cluster_id in unique_clusters:
            indices = np.where(labels == cluster_id)[0]
            majority_label = Counter(y_true[indices]).most_common(1)[0][0]
            label_map[cluster_id] = majority_label

        predicted_labels = np.array([label_map[lab] if lab != -1 else -1 for lab in labels])
        mask = predicted_labels != -1
        accuracy = np.mean(predicted_labels[mask] == y_true[mask])
        misclassified = np.sum(predicted_labels[mask] != y_true[mask]) / len(y_true[mask])
        noise_ratio = n_noise / len(X)

        if noise_ratio < min_noise:
            min_eps = eps
            min_min_samples = min_samples
            min_clusters = n_clusters
            min_accuracy = accuracy
            min_misclassified = misclassified
            min_noise = noise_ratio

        print(f"Liczba klastrów: {n_clusters}")
        print(f"Dokładność (bez szumu): {accuracy:.4f}")
        print(f"Błędy (w klastrach): {misclassified:.4f}")
        print(f"Szum: {noise_ratio:.4f}")

print("\nNajlepsze parametry:")
print(f"eps: {min_eps}")
print(f"min_samples: {min_min_samples}")
print(f"Liczba klastrów: {min_clusters}")
print(f"Dokładność: {min_accuracy:.4f}")
print(f"Błędy: {min_misclassified:.4f}")
print(f"Szum: {min_noise:.4f}")
