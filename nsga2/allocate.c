/* Memory allocation and deallocation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

/* Function to allocate memory to a population */
void allocate_memory_pop (NSGA2Type *nsga2Params,  population *pop, int size)
{
    int i;
    pop->ind = (individual *)malloc(size*sizeof(individual));
    for (i=0; i<size; i++)
    {
        allocate_memory_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to allocate memory to an individual */
void allocate_memory_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    ind->obj = (double *)malloc(nsga2Params->nobj*sizeof(double));
    ind->snn = (spiking_nn_t *)malloc(sizeof(spiking_nn_t));

    return;
}

/* Function to deallocate memory to a population */
void deallocate_memory_pop (NSGA2Type *nsga2Params, population *pop, int size)
{
    int i;
    for (i=0; i<size; i++)
    {
        deallocate_memory_ind (nsga2Params, &(pop->ind[i]));
    }
    free (pop->ind);

    return;
}

/* Function to deallocate memory to an individual */
// TODO: This function must be revised
void deallocate_memory_ind (NSGA2Type *nsga2Params, individual *ind)
{
    free(ind->obj);

    // TODO: the deallocation of the snn structure is more complex than only this, first is necessary to deallocate other things
    deallocate_memory_snn(ind->snn);
    free(ind->snn);

    return;
}

//TODO
/* Function to deallocate memory to a Spiking Neural Network structure */
void deallocate_memory_snn(spiking_nn_t *snn){

}
