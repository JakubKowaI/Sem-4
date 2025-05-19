import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('pr_results.csv')

# Create the plot
plt.plot(data['Modifier'], data['pr_result'], marker='o')

# Add basic labels
plt.xlabel('Modifier')
plt.ylabel('PR Result')
plt.title('PR Results')

# Show the plot
plt.savefig("Results.png")