/* Routine for mergeing two populations */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

/* Routine to merge two populations into one */
void merge(NSGA2Type *nsga2Params,  population *pop1, population *pop2, population *pop3)
{
    int i, k;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        copy_ind (nsga2Params,  &(pop1->ind[i]), &(pop3->ind[i]));
    }
    for (i=0, k=nsga2Params->popsize; i<nsga2Params->popsize; i++, k++)
    {
        copy_ind (nsga2Params,  &(pop2->ind[i]), &(pop3->ind[k]));
    }
    return;
}

/* Routine to copy an individual 'ind1' into another individual 'ind2' */
void copy_ind (NSGA2Type *nsga2Params, individual *ind1, individual *ind2)
{
    int i;
    ind2->rank = ind1->rank;
    ind2->constr_violation = ind1->constr_violation;
    ind2->crowd_dist = ind1->crowd_dist;
    
    // copy dynamic lists
    copy_individual(ind1, ind2);
    
    for (i=0; i<nsga2Params->nobj; i++)
        ind2->obj[i] = ind1->obj[i];

    return;
}
