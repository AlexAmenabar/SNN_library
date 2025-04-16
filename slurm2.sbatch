#!/bin/bash

# Job name
#SBATCH --job-name=test_snn_library

# Define the files which will contain the Standard and Error output
#SBATCH --output=output/a.out
#SBATCH --error=output/a.err

# Number of tasks that compose the job
#SBATCH --ntasks=1

# Advanced use
##SBATCH --cpus-per-task=20
##SBATCH --threads-per-core=2
##SBATCH --ntasks-per-core=2

# Required memory (Default 2GB)
#SBATCH --mem-per-cpu=2000G

# Select one partition
##CPU
##GPU
#SBATCH --partition=H100
#SBATCH --gpus=0

# If you are using arrays, specify the number of tasks in the array
##SBATCH --array=1-1

PARAMETROS=(
    "./conf.toml"
)

# Recorrer la lista de parámetros y ejecutar el programa en segundo plan
for PARAMS in "${PARAMETROS[@]}"; do
	./build/main_simulation_reorder ./conf2.toml
done

echo "Todas las tareas han terminado."