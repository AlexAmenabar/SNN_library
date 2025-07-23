import numpy as np
import matplotlib.pyplot as plt
from sklearn.manifold import MDS
from sklearn.preprocessing import LabelEncoder

# Example distance matrix (n_samples x n_samples)
# Replace this with your actual distance matrix
distance_matrix = np.array([
    [0.0, 1.0, 2.0, 3.0],
    [1.0, 0.0, 2.5, 3.5],
    [2.0, 2.5, 0.0, 1.5],
    [3.0, 3.5, 1.5, 0.0]
])

# Example class labels for each point
# Replace this with your actual class labels
labels = ['A', 'A', 'B', 'B']

# Convert distance matrix to 2D coordinates using MDS
mds = MDS(n_components=2, dissimilarity="precomputed", random_state=42)
coords = mds.fit_transform(distance_matrix)

# Encode class labels to integers
le = LabelEncoder()
class_ids = le.fit_transform(labels)

# Create a scatter plot
plt.figure(figsize=(8, 6))
scatter = plt.scatter(coords[:, 0], coords[:, 1], c=class_ids, cmap='tab10', s=100)

# Add legend with class names
legend_labels = le.classes_
handles = [plt.Line2D([0], [0], marker='o', color='w',
                      markerfacecolor=scatter.cmap(scatter.norm(i)), markersize=10)
           for i in range(len(legend_labels))]
plt.legend(handles, legend_labels, title="Class")

# Optional: add text labels to points
for i, label in enumerate(labels):
    plt.text(coords[i, 0], coords[i, 1], f'{i}', fontsize=9, ha='right')

plt.title("MDS Projection of Distance Matrix")
plt.xlabel("Component 1")
plt.ylabel("Component 2")
plt.grid(True)
plt.show()