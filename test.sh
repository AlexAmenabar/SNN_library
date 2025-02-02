#!/bin/bash

COMMANDS=(
    "./main 0 0 0 ./data/nn250/snn250.txt 5000 ./data/nn250/input_spike_times.txt"
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