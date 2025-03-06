# Rutas de CUDA (ajusta según tu instalación)
CUDA_ROOT_DIR = /opt/ohpc/pub/containers/NGC-pytorch-23.12-py3.sif/usr/local/cuda-12.3

# Compiladores y opciones
CC        = gcc
NVCC      = apptainer exec -B /pfs --nv /opt/ohpc/pub/containers/NGC-pytorch-23.12-py3.sif nvcc -x cu
CFLAGS    = -O2 -Iinclude -lm -DINPUT_SYNAPSES=1 -DINPUT_NEURON_BEHAVIOUR=2 -DINPUT_WEIGHTS=1 -DINPUT_DELAYS=2 -DINPUT_TRAINING_ZONES=1 -DCUDA
NVCC_FLAGS= -O2 -Iinclude

# Directorios del proyecto
SRC_DIR   = src
OBJ_DIR   = bin
INC_DIR   = include

# Directorios y librerías de CUDA
CUDA_LIB_DIR   = -L$(CUDA_ROOT_DIR)/targets/x86_64-linux/lib
CUDA_INC_DIR   = -I$(CUDA_ROOT_DIR)/include
CUDA_LINK_LIBS = -L/usr/local/cuda-12.3/targets/x86_64-linux/lib/libcudart.so
#CUDA_LINK_LIBS = -lcudart

# Lista de ficheros fuente
C_SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')
CU_SOURCES = $(shell find $(SRC_DIR) -type f -name '*.cu')

# Generación de los ficheros objeto correspondientes
C_OBJS  = $(patsubst $(SRC_DIR)/%.c,  $(OBJ_DIR)/%.o,  $(C_SOURCES))
CU_OBJS = $(patsubst $(SRC_DIR)/%.cu, $(OBJ_DIR)/%.o, $(CU_SOURCES))
OBJS    = $(C_OBJS) $(CU_OBJS)

# Nombre del ejecutable final
BIN = programa

# Regla por defecto
all: $(BIN)

# Regla de linking: enlaza todos los objetos con gcc
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(CUDA_LIB_DIR) $(CUDA_LINK_LIBS)

# Regla para compilar archivos C
# Se asume que el header asociado está en include con la misma estructura
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para compilar archivos CUDA
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu
	@mkdir -p $(dir $@)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@

# Limpieza de objetos y ejecutable
clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
