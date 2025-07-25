#!/bin/bash

# Job name
#SBATCH --job-name=enas2

# Define the files which will contain the Standard and Error output
#SBATCH --output=output/a1.out
#SBATCH --error=output/a1.err

# Number of tasks that compose the job
#SBATCH --ntasks=1

# Advanced use
##SBATCH --cpus-per-task=1
##SBATCH --threads-per-core=1
##SBATCH --ntasks-per-core=2

# Required memory (Default 2GB)
#SBATCH --mem-per-cpu=300GB

# Select one partition
##CPU
##GPU
#SBATCH --partition=H100
#SBATCH --gpus=0

# If you are using arrays, specify the number of tasks in the array
##SBATCH --array=1-1

# Recorrer la lista de parámetros y ejecutar el programa en segundo plan


./nsga2r 0.25 < input_data/snn_b.in

echo "Todas las tareas han terminado."