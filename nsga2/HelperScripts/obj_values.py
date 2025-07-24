import matplotlib.pyplot as plt

def read_generations(file_path):
    generations = []
    with open(file_path, 'r') as f:
        gen = []
        for line in f:
            stripped = line.strip()
            if not stripped:
                if gen:
                    generations.append(gen)
                    gen = []
            else:
                values = list(map(float, stripped.split()))
                gen.append(values)
        if gen:  # add last generation if no trailing newline
            generations.append(gen)
    return generations

def compute_best_worst(generations):
    best_obj1, worst_obj1 = [], []
    best_obj2, worst_obj2 = [], []

    for gen in generations:
        obj1 = [ind[0] for ind in gen]
        obj2 = [ind[1] for ind in gen]

        best_obj1.append(min(obj1))
        worst_obj1.append(max(obj1))
        best_obj2.append(min(obj2))
        worst_obj2.append(max(obj2))

    return best_obj1, worst_obj1, best_obj2, worst_obj2

def plot_objectives(best1, worst1, best2, worst2):
    generations = range(len(best1))

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.plot(generations, best1, label='Best value', color='blue')
    plt.plot(generations, worst1, label='Worst value', color='red')
    plt.title('Classification')
    plt.xlabel('Generation')
    plt.ylabel('Value')
    plt.legend()

    plt.subplot(1, 2, 2)
    plt.plot(generations, best2, label='Best value', color='blue')
    plt.plot(generations, worst2, label='Worst value', color='red')
    plt.title('Generated spikes')
    plt.xlabel('Generation')
    plt.ylabel('Value')
    plt.legend()

    plt.tight_layout()
    plt.show()

# === Main Execution ===
file_path = './experiments/objvalues.txt'  # Replace with your actual filename
generations = read_generations(file_path)
best1, worst1, best2, worst2 = compute_best_worst(generations)
plot_objectives(best1, worst1, best2, worst2)
