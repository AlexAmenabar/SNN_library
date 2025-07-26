import matplotlib.pyplot as plt

def read_generations(filename):
    generations = []
    with open(filename, 'r') as f:
        current_gen = []
        for line in f:
            line = line.strip()
            if not line:
                if current_gen:
                    generations.append(current_gen)
                    current_gen = []
            else:
                obj_values = list(map(float, line.split()))
                current_gen.append(obj_values)
        if current_gen:
            generations.append(current_gen)
    return generations

def compute_means(generations):
    mean_obj1 = []
    mean_obj2 = []
    for gen in generations:
        gen_array = list(zip(*gen))  # Transpose
        mean_obj1.append(sum(gen_array[0]) / len(gen_array[0]))
        mean_obj2.append(sum(gen_array[1]) / len(gen_array[1]))
    return mean_obj1, mean_obj2

def plot_means_separately(mean_obj1, mean_obj2):
    generations = list(range(1, len(mean_obj1) + 1))

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6), sharex=True)

    ax1.plot(generations, mean_obj1, marker='o', color='blue')
    ax1.set_ylabel('Mean Obj1')
    ax1.set_title('Mean of Objective 1 per Generation')
    ax1.grid(True)

    ax2.plot(generations, mean_obj2, marker='s', color='green')
    ax2.set_xlabel('Generation')
    ax2.set_ylabel('Mean Obj2')
    ax2.set_title('Mean of Objective 2 per Generation')
    ax2.grid(True)

    plt.tight_layout()
    plt.show()

# Main
if __name__ == '__main__':
    filename = './experiments/objvalues_par.txt'
    generations = read_generations(filename)
    mean_obj1, mean_obj2 = compute_means(generations)
    plot_means_separately(mean_obj1, mean_obj2)
