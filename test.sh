#!/bin/bash

COMPILE=(
    "./compile.sh"
)

TEST=(

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
echo "== Test finished! =="