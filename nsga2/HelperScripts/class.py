import numpy as np
import matplotlib.pyplot as plt
from sklearn.manifold import MDS
import matplotlib.cm as cm

import numpy as np
import matplotlib.pyplot as plt
from sklearn.manifold import MDS
import matplotlib.cm as cm

def parse_distance_file(filename):
    with open(filename, 'r') as f:
        lines = [line.strip() for line in f if line.strip()]

    try:
        population_size = int(lines[0])
        num_reps = int(lines[1])
        num_samples = int(lines[2])
    except IndexError:
        raise ValueError("File does not contain enough header lines.")

    idx = 3
    datasets = []
    total_blocks = population_size * num_reps
    expected_lines_per_block = 1 + 1 + num_samples  # score + labels + matrix

    for block in range(total_blocks):
        if idx + expected_lines_per_block > len(lines):
            raise ValueError(
                f"File ends too early. Expected {total_blocks} blocks, "
                f"but only found {block} full blocks. "
                f"Stopped at line {idx}."
            )

        try:
            score = float(lines[idx])
            idx += 1

            class_labels = list(map(int, lines[idx].split()))
            if len(class_labels) != num_samples:
                raise ValueError(f"Block {block+1}: Expected {num_samples} class labels, got {len(class_labels)}.")
            idx += 1

            dist_matrix = []
            for _ in range(num_samples):
                row = list(map(float, lines[idx].split()))
                if len(row) != num_samples:
                    raise ValueError(f"Block {block+1}: Distance matrix row has {len(row)} elements, expected {num_samples}.")
                dist_matrix.append(row)
                idx += 1

            datasets.append((score, class_labels, np.array(dist_matrix)))

        except Exception as e:
            print(f"Error parsing block {block+1} at line {idx}: {e}")
            raise

    return datasets, num_samples


def plot_mds(datasets, num_samples):
    for i, (score, labels, dist_matrix) in enumerate(datasets): 
        mds = MDS(n_components=2, dissimilarity='precomputed', random_state=42)
        coords = mds.fit_transform(dist_matrix)

        unique_classes = sorted(set(labels))
        cmap = cm.get_cmap('tab10', len(unique_classes))

        plt.figure(figsize=(6, 5))
        for j, cls in enumerate(unique_classes):
            indices = [k for k, label in enumerate(labels) if label == cls]
            plt.scatter(coords[indices, 0], coords[indices, 1],
                        label=f'Class {cls}', color=cmap(j))

        plt.title(f'MDS Projection - Dataset {i + 1}\nScore: {score:.4f}')
        plt.xlabel('Component 1')
        plt.ylabel('Component 2')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.show()


# === Usage ===
filename = './experiments/classification.txt'  # Replace with actual filename
datasets, num_samples = parse_distance_file(filename)
plot_mds(datasets, num_samples)
