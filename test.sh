#!/bin/bash

COMMANDS=(
    "./snn 0 0 0 ../data/new_nn_format/nn1.txt 100 ../data/new_nn_format/input_spike_times.txt"
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