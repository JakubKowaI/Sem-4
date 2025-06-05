import pandas as pd
import matplotlib.pyplot as plt

# Load CSV file
df = pd.read_csv("results_operations.csv", header=None, names=["n", "operation_index", "comparisons"])

# Ensure numeric type for sorting and plotting
df["operation_index"] = pd.to_numeric(df["operation_index"], errors="coerce")
df["comparisons"] = pd.to_numeric(df["comparisons"], errors="coerce")

# Sort by operation index
df = df.sort_values(by="operation_index")

# Get first unique n
n_value = df["n"].unique()[0]

# Plot
plt.figure(figsize=(20, 6))
plt.plot(df["operation_index"], df["comparisons"], linestyle='-', linewidth=0.5, color='blue')

plt.title(f"Comparisons per Operation (n={n_value})")
plt.xlabel("i-th Operation")
plt.ylabel("Comparisons")
plt.grid(True)

plt.tight_layout()
plt.savefig("comparisons_plot.png")
print("Plot saved as comparisons_plot.png")
