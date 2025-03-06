#ifndef LIF_NEURON_CUH
#define LIF_NEURON_CUH

#include <stdbool.h>

#define cudaCheckError() {                                          \
    cudaError_t e=cudaGetLastError();                                 \
    if(e!=cudaSuccess) {                                              \
      printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e));           \
      exit(0); \
    }                                                                 \
   }
   
   #define gpuErrchk(call)                                 \
     do {                                                        \
       cudaError_t err = call;                                   \
       if (err != cudaSuccess) {                                 \
         printf("CUDA error at %s %d: %s\n", __FILE__, __LINE__, \
                cudaGetErrorString(err));                        \
         exit(EXIT_FAILURE);                                     \
       }                                                         \
     } while (0)

double process_simulation_lif_neuron(spiking_nn_t *snn, int n, int m, int time_steps);

#endif