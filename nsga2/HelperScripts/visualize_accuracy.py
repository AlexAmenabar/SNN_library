import numpy as np
import matplotlib.pyplot as plt
import argparse

def process_file_generation_blocks(file_path):
    bests, worsts, means = [], [], []
    current_gen = []

    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip()

            if line == '':
                if current_gen:
                    first_values = [ind[0] for ind in current_gen]
                    bests.append(max(first_values))
                    worsts.append(min(first_values))
                    means.append(np.mean(first_values))
                    current_gen = []
            else:
                parts = list(map(float, line.replace(',', ' ').split()))
                current_gen.append(parts)

        # Handle last generation if not followed by empty line
        if current_gen:
            first_values = [ind[0] for ind in current_gen]
            bests.append(max(first_values))
            worsts.append(min(first_values))
            means.append(np.mean(first_values))

    return bests, worsts, means

def plot_accuracy(file_paths):
    num_objectives = len(file_paths)
    fig, host = plt.subplots(figsize=(14, 6))
    axes = [host]

    for i in range(1, num_objectives):
        new_axis = host.twinx()
        new_axis.spines.right.set_position(("axes", 1 + 0.1 * (i - 1)))
        axes.append(new_axis)

    colors = ['tab:blue', 'tab:green', 'tab:red', 'tab:orange', 'tab:purple', 'tab:brown']
    linestyles = ['-', '--', '-.']

    all_labels = []

    for i, file_path in enumerate(file_paths):
        bests, worsts, means = process_file_generation_blocks(file_path)
        generations = list(range(1, len(means) + 1))
        ax = axes[i]
        color = colors[i % len(colors)]

        l1, = ax.plot(generations, bests, linestyle=linestyles[0], color=color, label=f'Best (Obj{i+1})')
        l2, = ax.plot(generations, means, linestyle=linestyles[1], color=color, alpha=0.8, label=f'Mean (Obj{i+1})')
        l3, = ax.plot(generations, worsts, linestyle=linestyles[2], color=color, alpha=0.6, label=f'Worst (Obj{i+1})')

        ax.set_ylabel(f'Objective {i+1}', color=color)
        ax.tick_params(axis='y', labelcolor=color)
        all_labels += [l1, l2, l3]

    host.set_xlabel("Generation")
    plt.title("Evolutionary Algorithm Progress - Block Format Input")
    fig.legend(all_labels, [l.get_label() for l in all_labels], loc='upper center', ncol=3)
    plt.tight_layout()
    plt.show()

# Example usage:
# plot_block_formatted_objectives(["obj1_block.txt", "obj2_block.txt"])

if __name__ == "__main__":
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Accept a list of string inputs")
    parser.add_argument("inputs", nargs='+', help="List of input strings")
    args = parser.parse_args()
    plot_accuracy(args.inputs)