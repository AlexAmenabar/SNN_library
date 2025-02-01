#!/bin/bash

COMMANDS=(
    "./main 0 0 0 ./data/nn1/nn1.txt 100 ./data/nn1/input_spike_times.txt"
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