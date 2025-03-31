import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

df = pd.read_csv("resultsDG.csv")

plt.figure(figsize=(10, 5))
sns.barplot(x="Array Size", y="Comparisons", hue="Algorithm", data=df)
plt.title("Number of Comparisons in Sorting Algorithms")
plt.ylabel("Comparisons")
plt.xlabel("Array Size")
plt.legend(title="Algorithm")
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.savefig("ComparisonsDG.png", dpi=300, bbox_inches="tight")

plt.figure(figsize=(10, 5))
sns.barplot(x="Array Size", y="Swaps", hue="Algorithm", data=df)
plt.title("Number of Swaps in Sorting Algorithms")
plt.ylabel("Swaps")
plt.xlabel("Array Size")
plt.legend(title="Algorithm")
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.savefig("SwapsDG.png", dpi=300, bbox_inches="tight")
