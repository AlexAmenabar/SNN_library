import numpy as np
import matplotlib.pyplot as plt
import argparse
import os

def parse_generation_line(line):
    return list(map(float, line.strip().replace(',', '.').split()))

def process_file(file_path, num_generations, num_individuals):
    bests, worsts, means = [], [], []
    with open(file_path, 'r') as file:
        for gen_index, line in enumerate(file):
            if gen_index >= num_generations:
                break
            values = parse_generation_line(line)
            if len(values) != num_individuals:
                raise ValueError(f"Line {gen_index+1} in file '{file_path}' does not contain {num_individuals} individuals.")
            bests.append(np.max(values))
            worsts.append(np.min(values))
            means.append(np.mean(values))
    return bests, worsts, means

def plot_all_experiments_single_window(experiments, num_generations, num_individuals):
    generations = list(range(1, num_generations + 1))
    colors = ['tab:blue','tab:red','tab:green','tab:orange','tab:purple','tab:brown']
    linestyles = ['--','-','-.']

    num_experiments = len(experiments)
    ncols = 2
    nrows = (num_experiments + 1) // ncols
    fig, axes = plt.subplots(nrows, ncols, figsize=(16, 6*nrows), sharex=True)
    axes = axes.flatten() if num_experiments > 1 else [axes]

    for exp_index, (folder_path, file_paths) in enumerate(experiments):
        ax = axes[exp_index]
        all_labels = []
        for obj_index, file_path in enumerate(file_paths):
            bests, worsts, means = process_file(file_path, num_generations, num_individuals)
            color = colors[obj_index % len(colors)]

            # Use primary axis for first objective, secondary for second
            if obj_index == 0:
                axis_to_use = ax
                axis_to_use.set_ylabel(f'Objective {obj_index+1}', color=color)
                axis_to_use.tick_params(axis='y', labelcolor=color)
            elif obj_index == 1:
                axis_to_use = ax.twinx()
                axis_to_use.set_ylabel(f'Objective {obj_index+1}', color=color)
                axis_to_use.tick_params(axis='y', labelcolor=color)
            else:
                # More than two objectives → you could stack extra axes using spines
                axis_to_use = ax.twinx()
                axis_to_use.spines.right.set_position(("axes", 1 + 0.1 * (obj_index - 1)))
                axis_to_use.set_ylabel(f'Objective {obj_index+1}', color=color)
                axis_to_use.tick_params(axis='y', labelcolor=color)

            l1, = axis_to_use.plot(generations, bests, linestyle=linestyles[0], color=color, alpha=0.8, label=f'Best Obj{obj_index+1}')
            l2, = axis_to_use.plot(generations, means, linestyle=linestyles[1], color=color, alpha=0.6, label=f'Mean Obj{obj_index+1}')
            l3, = axis_to_use.plot(generations, worsts, linestyle=linestyles[2], color=color, alpha=0.4, label=f'Worst Obj{obj_index+1}')

            all_labels += [l1, l2, l3]

        ax.set_ylabel('Objective Value')
        ax.set_title(f'Experiment: {os.path.basename(folder_path)}')
        ax.legend(handles=all_labels, loc='upper right')

    for ax in axes[num_experiments:]:
        ax.axis('off')

    axes[-1].set_xlabel('Generation')
    plt.tight_layout()
    plt.show()

def find_experiment_folders(root_folder, target_files):
    folders = []
    for root, dirs, files in os.walk(root_folder):
        if all(name in files for name in target_files):
            paths = [os.path.join(root, name) for name in target_files]
            folders.append((root, paths))
    return folders

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot evolutionary algorithm objectives')
    subparsers = parser.add_subparsers(dest='mode')

    parser_files = subparsers.add_parser('files')
    parser_files.add_argument('num_generations', type=int)
    parser_files.add_argument('num_individuals', type=int)
    parser_files.add_argument('file_paths', nargs='+')

    parser_folder = subparsers.add_parser('folder')
    parser_folder.add_argument('root_folder')
    parser_folder.add_argument('num_generations', type=int)
    parser_folder.add_argument('num_individuals', type=int)
    parser_folder.add_argument('target_files', nargs='+')

    args = parser.parse_args()

    if args.mode is None:
        parser.print_help()
        exit(1)

    if args.mode == 'files':
        plot_all_experiments_single_window([('.', args.file_paths)], args.num_generations, args.num_individuals)
    elif args.mode == 'folder':
        experiments = find_experiment_folders(args.root_folder, args.target_files)
        if not experiments:
            print('No experiment folders found containing all target files.')
        else:
            plot_all_experiments_single_window(experiments, args.num_generations, args.num_individuals)
