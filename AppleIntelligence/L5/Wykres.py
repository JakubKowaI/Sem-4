import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("results.csv")

# Replace missing or ambiguous Normalize_func values with string "None"
df['Normalize_func'] = df['Normalize_func'].fillna("None").replace(["", "nan", None], "None")

# Optional: round learning rates for cleaner X-axis (optional)
df['Learning_rate'] = df['Learning_rate'].round(3)

# Group and calculate mean accuracy
grouped = df.groupby(['Normalize_func', 'Learning_rate'])['Accuracy'].mean().reset_index()

# Plot
plt.figure(figsize=(10, 6))
for norm_func in grouped['Normalize_func'].unique():
    subset = grouped[grouped['Normalize_func'] == norm_func]
    plt.plot(subset['Learning_rate'], subset['Accuracy'], label=norm_func)

plt.title('Mean Accuracy vs Learning Rate by Normalization Function using Relu in the hidden layer and Sigmoid in the output layer')
plt.xlabel('Learning Rate')
plt.ylabel('Mean Accuracy')
plt.legend(title='Normalization')
plt.grid(True)
plt.tight_layout()
plt.savefig("accuracy_vs_learning_rate.png")
print("Plot saved to accuracy_vs_learning_rate.png")
