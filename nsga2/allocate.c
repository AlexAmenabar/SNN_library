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
// PROBABLY MUST CHANGE THIS FUNCTION
void allocate_memory_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    ind->obj = (double *)malloc(nsga2Params->nobj*sizeof(double));
    ind->snn = (spiking_nn_t *)malloc(sizeof(spiking_nn_t));

    /*if (nsga2Params->nreal != 0)
    {
        ind->xreal = (double *)malloc(nsga2Params->nreal*sizeof(double));
    }
    if (nsga2Params->nbin != 0)
    {
        ind->xbin = (double *)malloc(nsga2Params->nbin*sizeof(double));
        ind->gene = (int **)malloc(nsga2Params->nbin*sizeof(int *));
        for (j=0; j<nsga2Params->nbin; j++)
        {
            ind->gene[j] = (int *)malloc(nsga2Params->nbits[j]*sizeof(int));
        }
    }*/
    /*if (nsga2Params->ncon != 0)
    {
        ind->constr = (double *)malloc(nsga2Params->ncon*sizeof(double));
    }*/
    
    //ind->obj = (double *)malloc(nsga2Params->nobj*sizeof(double)); // allocate memory for objective function results
    

    // ME
    // allocate memory for snn
    //ind->snn = (spiking_nn_t *)malloc(sizeof(spiking_nn_t));
    //ind->genotype = (genotype *)malloc(sizeof(genotype));
    // TODO

    //ind->genotype = (int *)malloc(nsga2Params-> * sizeof(int));

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
void deallocate_memory_ind (NSGA2Type *nsga2Params, individual *ind)
{
    free(ind->obj);

    // TODO: the deallocation of the snn structure is more complex than only this, first is necessary to deallocate other things
    deallocate_memory_snn(ind->snn);
    free(ind->snn);


    /*int j;
    if (nsga2Params->nreal != 0)
    {
        free(ind->xreal);
    }
    if (nsga2Params->nbin != 0)
    {
        for (j=0; j<nsga2Params->nbin; j++)
        {
            free(ind->gene[j]);
        }
        free(ind->xbin);
        free(ind->gene);
    }
    free(ind->obj);
    if (nsga2Params->ncon != 0)
    {
        free(ind->constr);
    }*/
    return;
}

//TODO
/* Function to deallocate memory to a Spiking Neural Network structure */
void deallocate_memory_snn(spiking_nn_t *snn){

}
