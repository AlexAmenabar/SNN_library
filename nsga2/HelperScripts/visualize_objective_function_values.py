import numpy as np
import matplotlib.pyplot as plt
import argparse

def parse_generation_line(line):
    return list(map(float, line.strip().replace(",", " ").split()))

def process_file(file_path, num_generations, num_individuals):
    bests, worsts, means = [], [], []
    with open(file_path, 'r') as file:
        for gen_index, line in enumerate(file):
            if gen_index >= num_generations:
                break
            values = parse_generation_line(line)
            if len(values) != num_individuals:
                raise ValueError(f"Line {gen_index+1} in file '{file_path}' does not contain {num_individuals} individuals.")
            
            max_value = np.max(values)
            min_value = np.min(values)
            mean_value = np.mean(values)

            if max_value < 0: 
                max_value = -max_value
            if min_value < 0: 
                min_value = -min_value
            if mean_value < 0: 
                mean_value = -mean_value

            bests.append(max_value)
            worsts.append(min_value)
            means.append(mean_value)

    return bests, worsts, means

def plot_multiple_objectives(file_paths, num_generations, num_individuals):
    num_objectives = len(file_paths)
    generations = list(range(1, num_generations + 1))

    fig, host = plt.subplots(figsize=(14, 6))
    axes = [host]

    # Create additional y-axes
    for i in range(1, num_objectives):
        new_axis = host.twinx()
        new_axis.spines.right.set_position(("axes", 1 + 0.1 * (i - 1)))
        axes.append(new_axis)

    colors = ['tab:blue', 'tab:red', 'tab:green', 'tab:orange', 'tab:purple', 'tab:brown']
    linestyles = ['--', '-', '-.']

    all_labels = []

    for i, file_path in enumerate(file_paths):
        bests, worsts, means = process_file(file_path, num_generations, num_individuals)
        ax = axes[i]
        color = colors[i % len(colors)]

        l1, = ax.plot(generations, bests, linestyle=linestyles[0], color=color, label=f'Best (Obj{i+1})')
        l2, = ax.plot(generations, means, linestyle=linestyles[1], color=color, alpha=0.7, label=f'Mean (Obj{i+1})')
        l3, = ax.plot(generations, worsts, linestyle=linestyles[2], color=color, alpha=0.5, label=f'Worst (Obj{i+1})')

        ax.set_ylabel(f'Objective {i+1}', color=color)
        ax.tick_params(axis='y', labelcolor=color)
        all_labels += [l1, l2, l3]

    host.set_xlabel("Generation")
    plt.title("Evolutionary Algorithm - Multi-objective Progress")
    fig.legend(all_labels, [l.get_label() for l in all_labels], loc='upper center', ncol=3)
    plt.tight_layout()
    plt.show()

# Example usage:
# plot_multiple_objectives(
#     ["objective1.txt", "objective2.txt", "objective3.txt"],
#     num_generations=50,
#     num_individuals=30
# )

# Main
if __name__ == "__main__":
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Two integers followed by a list of strings")

    # Two required positional integers
    parser.add_argument("num1", type=int, help="First integer")
    parser.add_argument("num2", type=int, help="Second integer")

    # Remaining arguments as a list of strings
    parser.add_argument("items", nargs='*', help="List of string inputs")

    args = parser.parse_args()

    plot_multiple_objectives(args.items, num_generations=args.num1, num_individuals=args.num2)

    
    #plot_multiple_objectives(
    #    ["./experiments_snn_test/my_metric.txt", "./experiments_snn_test/n_spikes_per_neuron.txt"],
    #    num_generations=10,
    #    num_individuals=8
    #)