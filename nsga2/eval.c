/* Routine for evaluating population members  */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

/* Routine to evaluate objective function values and constraints for a population */
void evaluate_pop (NSGA2Type *nsga2Params, population *pop, void *inp, void *out)
{
    int i;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        printf("Individual %d:\n", i);
        evaluate_ind (nsga2Params, &(pop->ind[i]), inp, out);
        printf("\n");
    }
    return;
}

/* Routine to evaluate objective function values and constraints for an individual */
void evaluate_ind (NSGA2Type *nsga2Params, individual *ind, void *inp, void *out)
{
    int i, j;
    

    ind->obj[0] = -ind->snn->n_synapses;
    ind->obj[1] = ind->snn->n_neurons;


    printf("Obj 1 = %f, obj 2 = %f\n", ind->obj[0], ind->obj[1]);

    // provisional 
    
    /*test_problem (ind->xreal, ind->xbin, ind->gene, ind->obj, ind->constr);
    
    if (nsga2Params->ncon==0)
    {
        ind->constr_violation = 0.0;
    }
    else
    {
        ind->constr_violation = 0.0;
        for (j=0; j<nsga2Params->ncon; j++)
        {
            if (ind->constr[j]<0.0)
            {
                ind->constr_violation += ind->constr[j];
            }
        }
    }*/

    return;
}
