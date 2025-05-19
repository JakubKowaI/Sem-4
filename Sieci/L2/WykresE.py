import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('Results_E.csv')

# Create the plot
plt.plot(data['Modifier'], data['pr_result'], marker='o')

# Add basic labels
plt.xlabel('Number of additional edges')
plt.ylabel('PR Result')
plt.title('PR Results for additional random edges')

# Show the plot
plt.savefig("Results_E.png")