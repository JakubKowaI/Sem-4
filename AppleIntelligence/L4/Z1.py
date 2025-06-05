from sklearn.cluster import KMeans
from sklearn.datasets import fetch_openml
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import pairwise_distances_argmin_min
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import math

# Wczytaj dane
mnist = fetch_openml('mnist_784', version=1, as_frame=False)
X = mnist.data
y = mnist.target.astype(int)

# Normalizacja
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# Lista wartości k
for clusters in [10, 15, 20, 30]:
    best_kmeans = None
    min_inertia = float("inf")

    # Wybierz najlepszy model (najmniejsza inercja)
    for _ in range(10):
        kmeans = KMeans(n_clusters=clusters, init='k-means++', n_init=1)
        kmeans.fit(X_scaled)
        if kmeans.inertia_ < min_inertia:
            min_inertia = kmeans.inertia_
            best_kmeans = kmeans

    print(f"[k={clusters}] Najmniejsza inercja: {min_inertia:.2f}")

    # Przypisanie do klastrów
    labels = best_kmeans.labels_
    df = pd.DataFrame({'label': y, 'cluster': labels})

    # Macierz procentowego przydziału
    conf_matrix = pd.crosstab(df['label'], df['cluster'], normalize='index') * 100

    plt.figure(figsize=(clusters, 8))
    sns.heatmap(conf_matrix, annot=True, fmt=".1f", cmap="viridis")
    plt.title(f"Procentowy przydział cyfr do klastrów (k={clusters})")
    plt.xlabel("Klaster")
    plt.ylabel("Cyfra rzeczywista")
    plt.tight_layout()
    plt.savefig(f"Klastry_{clusters}.png", dpi=500)
    plt.close()

    # Przywrócenie oryginalnej skali centroidów
    centroids_unscaled = scaler.inverse_transform(best_kmeans.cluster_centers_)

    # Rysowanie centroidów
    cols = 5
    rows = math.ceil(clusters / cols)
    plt.figure(figsize=(cols * 2, rows * 2.2))
    for i, centroid in enumerate(centroids_unscaled):
        plt.subplot(rows, cols, i + 1)
        plt.imshow(centroid.reshape(28, 28), cmap='gray')
        plt.title(f'Centroid {i}')
        plt.axis('off')
    plt.suptitle(f'Obrazy centroidów (k={clusters})')
    plt.tight_layout()
    plt.savefig(f"Centroidy_{clusters}.png", dpi=500)
    plt.close()
