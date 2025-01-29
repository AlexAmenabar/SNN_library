#!/bin/bash

COMMANDS=(
    "./snn 0 0 0 6 3 1 ../data/nn1/synapse_matrix.txt ../data/nn1/synapse_weight_matrix.txt ../data/nn1/synapse_delay_matrix.txt ../data/nn1/neuron_ex_in.txt 100 ../data/nn1/input_spike_times.txt > output.txt"
)

echo "== Ejecutando tests =="

# ejecutar cada comando
for CMD in "${COMMANDS[@]}"; do
    echo ""
    # ejecutar el comando
    eval $CMD
done

echo ""
echo "== Todos los tests han finalizado =="