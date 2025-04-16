#!/bin/bash

COMMANDS=(
    "gcc -Iinclude -o build/main_parallel_learn_simulation src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DOPENMP"
    "gcc -Iinclude -o build/main_parallel_no_learn_simulation src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DOPENMP -DNO_LEARN"
    "gcc -Iinclude -o build/main_parallel_simulation_debug src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DOPENMP -DDEBUG"
    "gcc -Iinclude -o build/main_simulation src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1"
    "gcc -Iinclude -o build/main_simulation_no_learn src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DNO_LEARN"
    "gcc -Iinclude -o build/main_simulation_debug src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DDEBUG"
    "gcc -Iinclude -g -o build/main_parallel_learn_simulation_gdb src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DOPENMP"
    "gcc -Iinclude -o build/main_simulation src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DOPENMP -DNOLEARN"
    "gcc -Iinclude -o build/main_simulation_reorder src/main_new.c src/snn_library.c src/load_data.c src/helpers.c src/neuron_models/lif_neuron.c src/training_rules/stdp.c src/simulations/simulations.c -lm  -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DOPENMP -DNOLEARN -DREORDER"

    "gcc -Iinclude -o build/network_generator src/network_generator/network_generator_main.c src/network_generator/network_generator.c -lm"
)

echo "== Compiling... =="

# ejecutar cada comando
for CMD in "${COMMANDS[@]}"; do
    echo "  Generating executable..."
    # ejecutar el comando
    eval $CMD
done

echo ""
echo "== Code compiled! =="