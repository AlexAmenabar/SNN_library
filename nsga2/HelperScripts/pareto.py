import matplotlib.pyplot as plt

def read_generations(filename):
    generations = []
    current_gen = []

    with open(filename, 'r') as f:
        for line in f:
            stripped = line.strip()
            if not stripped:
                if current_gen:
                    generations.append(current_gen)
                    current_gen = []
            else:
                obj1, obj2 = map(float, stripped.split())
                current_gen.append((obj1, obj2))
        if current_gen:
            generations.append(current_gen)  # Add last generation

    return generations

def plot_pareto_fronts(generations, show_all=True):
    plt.figure(figsize=(8, 6))

    if show_all:
        for i, gen in enumerate(generations):
            x, y = zip(*gen)
            plt.scatter(x, y, label=f'Gen {i+1}', alpha=0.6)
    else:
        x, y = zip(*generations[-1])
        plt.scatter(x, y, color='red', label='Final Gen')

    plt.xlabel('Objective 1')
    plt.ylabel('Objective 2')
    plt.title('Pareto Fronts')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

# === Usage ===
filename = './experiments/objvalues.txt'  # Replace with your actual file
generations = read_generations(filename)
plot_pareto_fronts(generations, show_all=False)  # Set show_all=False to plot only final front
