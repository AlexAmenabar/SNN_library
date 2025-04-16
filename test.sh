#!/bin/bash

COMPILE=(
    "./compile.sh"
)

TEST=(
    "./build/main_simulation_no_learn ./testing/test1_conf.toml"
)

COMPARISON=(
    "diff testing/output/test1_generated_spikes.txt testing/expected_results/results.txt"
)

PRINTING=(
    "./build/main_simulation_no_learn ./testing/test1_conf.toml"
)



# ejecutar cada comando
for CMD in "${COMPILE[@]}"; do
    # ejecutar el comando
    eval $CMD
done

echo ""
echo "== Starting test... =="

# ejecutar cada comando
for CMD in "${TEST[@]}"; do
    echo "  Running test..."
    # ejecutar el comando
    eval $CMD
done

echo ""
echo "== Comparing obtained results and expected ones... =="

# ejecutar cada comando
for CMD in "${COMPARISON[@]}"; do
    echo "  Comparing results..."
    # ejecutar el comando
    eval $CMD
done


echo ""
echo "== Test finished! =="