/* NSGA-II routine (implementation of the 'main' function) */

# include "nsga2.h"
# include "rand.h"

FILE *fpt1;
FILE *fpt2;
FILE *fpt3;
FILE *fpt4;
FILE *fpt5;
FILE *gp;
population *parent_pop;
population *child_pop;
population *mixed_pop;
motif_t *motifs_data; // list of motifs
int n_motifs; // number of motifs that can be used

// dataset // TODO: This is only temporal
image_dataset_t image_dataset;

NSGA2Type ReadParameters(int argc, char **argv){
    int i;
    NSGA2Type nsga2Params;
    
	printf("== Read input parameters == \n");

    if (argc<2)
    {
        printf("\n Usage ./nsga2r random_seed \n");
        exit(1);
    }
    nsga2Params.seed = (double)atof(argv[1]);
    if (nsga2Params.seed<=0.0 || nsga2Params.seed>=1.0)
    {
        printf("\n Entered seed value is wrong, seed value must be in (0,1) \n");
        exit(1);
    }
    printf("\n Enter the problem relevant and algorithm relevant parameters ... ");
    
    // Population size
    printf("\n Enter the population size (a multiple of 4) : ");
    scanf("%d",&nsga2Params.popsize);
    if (nsga2Params.popsize<4 || (nsga2Params.popsize%4)!= 0)
    {
        printf("\n population size read is : %d",nsga2Params.popsize);
        printf("\n Wrong population size entered, hence exiting \n");
        exit (1);
    }

    // Number of generations
    printf("\n Enter the number of generations : ");
    scanf("%d",&nsga2Params.ngen);
    if (nsga2Params.ngen<1)
    {
        printf("\n number of generations read is : %d",nsga2Params.ngen);
        printf("\n Wrong nuber of generations entered, hence exiting \n");
        exit (1);
    }

    // Number of objective functions
    printf("\n Enter the number of objectives : ");
    scanf("%d",&nsga2Params.nobj);
    if (nsga2Params.nobj<1)
    {
        printf("\n number of objectives entered is : %d",nsga2Params.nobj);
        printf("\n Wrong number of objectives entered, hence exiting \n");
        exit (1);
    }

    // NUmber of constraints
    printf("\n Enter the number of constraints : ");
    scanf("%d",&nsga2Params.ncon);
    if (nsga2Params.ncon<0)
    {
        printf("\n number of constraints entered is : %d",nsga2Params.ncon);
        printf("\n Wrong number of constraints enetered, hence exiting \n");
        exit (1);
    }

    
    // I think I don't use real and binary variables
    printf("\n Enter the number of real variables : ");
    scanf("%d",&nsga2Params.nreal);
    if (nsga2Params.nreal<0)
    {
        printf("\n number of real variables entered is : %d",nsga2Params.nreal);
        printf("\n Wrong number of variables entered, hence exiting \n");
        exit (1);
    }
    if (nsga2Params.nreal != 0)
    {
        nsga2Params.min_realvar = (double *)malloc(nsga2Params.nreal*sizeof(double));
        nsga2Params.max_realvar = (double *)malloc(nsga2Params.nreal*sizeof(double));
        for (i=0; i<nsga2Params.nreal; i++)
        {
            printf ("\n Enter the lower limit of real variable %d : ",i+1);
            scanf ("%lf",&nsga2Params.min_realvar[i]);
            printf ("\n Enter the upper limit of real variable %d : ",i+1);
            scanf ("%lf",&nsga2Params.max_realvar[i]);
            if (nsga2Params.max_realvar[i] <= nsga2Params.min_realvar[i])
            {
                printf("\n Wrong limits entered for the min and max bounds of real variable, hence exiting \n");
                exit(1);
            }
        }
        printf ("\n Enter the probability of crossover of real variable (0.6-1.0) : ");
        scanf ("%lf",&nsga2Params.pcross_real);
        if (nsga2Params.pcross_real<0.0 || nsga2Params.pcross_real>1.0)
        {
            printf("\n Probability of crossover entered is : %e",nsga2Params.pcross_real);
            printf("\n Entered value of probability of crossover of real variables is out of bounds, hence exiting \n");
            exit (1);
        }
        printf ("\n Enter the probablity of mutation of real variables (1/nsga2Params.nreal) : ");
        scanf ("%lf",&nsga2Params.pmut_real);
        if (nsga2Params.pmut_real<0.0 || nsga2Params.pmut_real>1.0)
        {
            printf("\n Probability of mutation entered is : %e",nsga2Params.pmut_real);
            printf("\n Entered value of probability of mutation of real variables is out of bounds, hence exiting \n");
            exit (1);
        }
        printf ("\n Enter the value of distribution index for crossover (5-20): ");
        scanf ("%lf",&nsga2Params.eta_c);
        if (nsga2Params.eta_c<=0)
        {
            printf("\n The value entered is : %e",nsga2Params.eta_c);
            printf("\n Wrong value of distribution index for crossover entered, hence exiting \n");
            exit (1);
        }
        printf ("\n Enter the value of distribution index for mutation (5-50): ");
        scanf ("%lf",&nsga2Params.eta_m);
        if (nsga2Params.eta_m<=0)
        {
            printf("\n The value entered is : %e",nsga2Params.eta_m);
            printf("\n Wrong value of distribution index for mutation entered, hence exiting \n");
            exit (1);
        }
    }
    printf("\n Enter the number of binary variables : ");
    scanf("%d",&nsga2Params.nbin);
    if (nsga2Params.nbin<0)
    {
        printf ("\n number of binary variables entered is : %d",nsga2Params.nbin);
        printf ("\n Wrong number of binary variables entered, hence exiting \n");
        exit(1);
    }
    if (nsga2Params.nbin != 0)
    {
        nsga2Params.nbits = (int *)malloc(nsga2Params.nbin*sizeof(int));
        nsga2Params.min_binvar = (double *)malloc(nsga2Params.nbin*sizeof(double));
        nsga2Params.max_binvar = (double *)malloc(nsga2Params.nbin*sizeof(double));
        for (i=0; i<nsga2Params.nbin; i++)
        {
            printf ("\n Enter the number of bits for binary variable %d : ",i+1);
            scanf ("%d",&nsga2Params.nbits[i]);
            if (nsga2Params.nbits[i] < 1)
            {
                printf("\n Wrong number of bits for binary variable entered, hence exiting");
                exit(1);
            }
            printf ("\n Enter the lower limit of binary variable %d : ",i+1);
            scanf ("%lf",&nsga2Params.min_binvar[i]);
            printf ("\n Enter the upper limit of binary variable %d : ",i+1);
            scanf ("%lf",&nsga2Params.max_binvar[i]);
            if (nsga2Params.max_binvar[i] <= nsga2Params.min_binvar[i])
            {
                printf("\n Wrong limits entered for the min and max bounds of binary variable entered, hence exiting \n");
                exit(1);
            }
        }
        printf ("\n Enter the probability of crossover of binary variable (0.6-1.0): ");
        scanf ("%lf",&nsga2Params.pcross_bin);
        if (nsga2Params.pcross_bin<0.0 || nsga2Params.pcross_bin>1.0)
        {
            printf("\n Probability of crossover entered is : %e",nsga2Params.pcross_bin);
            printf("\n Entered value of probability of crossover of binary variables is out of bounds, hence exiting \n");
            exit (1);
        }
        printf ("\n Enter the probability of mutation of binary variables (1/nsga2Params.nbits): ");
        scanf ("%lf",&nsga2Params.pmut_bin);
        if (nsga2Params.pmut_bin<0.0 || nsga2Params.pmut_bin>1.0)
        {
            printf("\n Probability of mutation entered is : %e",nsga2Params.pmut_bin);
            printf("\n Entered value of probability  of mutation of binary variables is out of bounds, hence exiting \n");
            exit (1);
        }
    }
    if (nsga2Params.nreal==0 && nsga2Params.nbin==0)
    {
        printf("\n Number of real as well as binary variables, both are zero, hence exiting \n");
        exit(1);
    }
    
    
    nsga2Params.choice=0;
    printf("\n Do you want to use gnuplot to display the results realtime (0 for NO) (1 for yes) : ");
    scanf("%d",&nsga2Params.choice);
    if (nsga2Params.choice!=0 && nsga2Params.choice!=1)
    {
        printf("\n Entered the wrong nsga2Params.choice, hence exiting, nsga2Params.choice entered was %d\n",nsga2Params.choice);
        exit(1);
    }
    if (nsga2Params.choice==1)
    {
        gp = popen(GNUPLOT_COMMAND,"w");
        if (gp==NULL)
        {
            printf("\n Could not open a pipe to gnuplot, check the definition of GNUPLOT_COMMAND in file global.h\n");
            printf("\n Edit the string to suit your system configuration and rerun the program\n");
            exit(1);
        }
        if (nsga2Params.nobj==2)
        {
            printf("\n Enter the objective for X axis display : ");
            scanf("%d",&nsga2Params.obj1);
            if (nsga2Params.obj1<1 || nsga2Params.obj1>nsga2Params.nobj)
            {
                printf("\n Wrong value of X objective entered, value entered was %d\n",nsga2Params.obj1);
                exit(1);
            }
            printf("\n Enter the objective for Y axis display : ");
            scanf("%d",&nsga2Params.obj2);
            if (nsga2Params.obj2<1 || nsga2Params.obj2>nsga2Params.nobj)
            {
                printf("\n Wrong value of Y objective entered, value entered was %d\n",nsga2Params.obj2);
                exit(1);
            }
            nsga2Params.obj3 = -1;
        }
        else
        {
            printf("\n #obj > 2, 2D display or a 3D display ?, enter 2 for 2D and 3 for 3D :");
            scanf("%d",&nsga2Params.choice);
            if (nsga2Params.choice!=2 && nsga2Params.choice!=3)
            {
                printf("\n Entered the wrong nsga2Params.choice, hence exiting, nsga2Params.choice entered was %d\n",nsga2Params.choice);
                exit(1);
            }
            if (nsga2Params.choice==2)
            {
                printf("\n Enter the objective for X axis display : ");
                scanf("%d",&nsga2Params.obj1);
                if (nsga2Params.obj1<1 || nsga2Params.obj1>nsga2Params.nobj)
                {
                    printf("\n Wrong value of X objective entered, value entered was %d\n",nsga2Params.obj1);
                    exit(1);
                }
                printf("\n Enter the objective for Y axis display : ");
                scanf("%d",&nsga2Params.obj2);
                if (nsga2Params.obj2<1 || nsga2Params.obj2>nsga2Params.nobj)
                {
                    printf("\n Wrong value of Y objective entered, value entered was %d\n",nsga2Params.obj2);
                    exit(1);
                }
                nsga2Params.obj3 = -1;
            }
            else
            {
                printf("\n Enter the objective for X axis display : ");
                scanf("%d",&nsga2Params.obj1);
                if (nsga2Params.obj1<1 || nsga2Params.obj1>nsga2Params.nobj)
                {
                    printf("\n Wrong value of X objective entered, value entered was %d\n",nsga2Params.obj1);
                    exit(1);
                }
                printf("\n Enter the objective for Y axis display : ");
                scanf("%d",&nsga2Params.obj2);
                if (nsga2Params.obj2<1 || nsga2Params.obj2>nsga2Params.nobj)
                {
                    printf("\n Wrong value of Y objective entered, value entered was %d\n",nsga2Params.obj2);
                    exit(1);
                }
                printf("\n Enter the objective for Z axis display : ");
                scanf("%d",&nsga2Params.obj3);
                if (nsga2Params.obj3<1 || nsga2Params.obj3>nsga2Params.nobj)
                {
                    printf("\n Wrong value of Z objective entered, value entered was %d\n",nsga2Params.obj3);
                    exit(1);
                }
                printf("\n You have chosen 3D display, hence location of eye required \n");
                printf("\n Enter the first angle (an integer in the range 0-180) (if not known, enter 60) :");
                scanf("%d",&nsga2Params.angle1);
                if (nsga2Params.angle1<0 || nsga2Params.angle1>180)
                {
                    printf("\n Wrong value for first angle entered, hence exiting \n");
                    exit(1);
                }
                printf("\n Enter the second angle (an integer in the range 0-360) (if not known, enter 30) :");
                scanf("%d",&nsga2Params.angle2);
                if (nsga2Params.angle2<0 || nsga2Params.angle2>360)
                {
                    printf("\n Wrong value for second angle entered, hence exiting \n");
                    exit(1);
                }
            }
        }
    }

    // my parameters -- general
    scanf("%d",&nsga2Params.neuron_type);    
    scanf("%d",&nsga2Params.max_motifs);
    scanf("%d",&nsga2Params.min_motifs);
    scanf("%d",&nsga2Params.max_multiple_connections);
    scanf("%d",&nsga2Params.min_multiple_connections);
    scanf("%lf",&nsga2Params.pcross);
    scanf("%lf",&nsga2Params.pmut);

    // read parameters related to neurons
    scanf("%lf",&nsga2Params.max_vthres);
    scanf("%lf",&nsga2Params.min_vthres);
    scanf("%lf",&nsga2Params.max_vrest);
    scanf("%lf",&nsga2Params.min_vrest);
    scanf("%d",&nsga2Params.max_refracttime);
    scanf("%d",&nsga2Params.min_refracttime);
    scanf("%d",&nsga2Params.max_R);
    scanf("%d",&nsga2Params.min_R);

    // read parameters realted to synapses
    scanf("%d",&nsga2Params.max_latency);
    scanf("%d",&nsga2Params.min_latency);
    scanf("%d",&nsga2Params.max_learning_rule);
    scanf("%d",&nsga2Params.min_learning_rule);
    scanf("%lf",&nsga2Params.max_weight);
    scanf("%lf",&nsga2Params.min_weight);

    // read dataset parameters
    scanf("%d",&nsga2Params.dataset_type);
    scanf("%s",&nsga2Params.train_dataset_dir);
    scanf("%s",&nsga2Params.train_labels_dir);
    scanf("%s",&nsga2Params.test_dataset_dir);
    scanf("%s",&nsga2Params.test_labels_dir);
    scanf("%d",&nsga2Params.n_train_samples);
    scanf("%d",&nsga2Params.n_test_samples);
    scanf("%d",&nsga2Params.n_classes);
    scanf("%d",&nsga2Params.image_size);
    scanf("%d",&nsga2Params.bins);


    // simulations repetitions and number of samples configuration
    scanf("%d",&nsga2Params.mode);
    scanf("%d",&nsga2Params.n_repetitions);
    scanf("%d",&nsga2Params.n_samples);
    

    // DEBUG levels are used to manage what messages will be printed

#ifdef DEBUG1

    printf("\n\n = == === ===== === == = \n Printing input parameters...\n = == === ===== === == = \n");
    printf(" > Population size: %d\n", nsga2Params.popsize);
    printf(" > Number of generations: %d\n", nsga2Params.ngen);
    printf(" > Number of objective functions: %d\n", nsga2Params.nobj);

    printf(" > Neuron type: %d\n", nsga2Params.neuron_type);
    printf(" > Max motifs: %d\n", nsga2Params.max_motifs);
    printf(" > Min motifs: %d\n", nsga2Params.min_motifs);
    printf(" > Max multiple connections: %d\n", nsga2Params.max_multiple_connections);
    printf(" > Min multiple connections: %d\n", nsga2Params.min_multiple_connections);
    printf(" > P cross: %e\n", nsga2Params.pcross);
    printf(" > P mut: %e\n", nsga2Params.pmut);

    printf(" > Max V threshold: %e\n", nsga2Params.max_vthres);
    printf(" > Min V threshold: %e\n", nsga2Params.min_vthres);
    printf(" > Max V rest: %e\n", nsga2Params.max_vrest);
    printf(" > Min V rest: %e\n", nsga2Params.min_vrest);
    printf(" > Max refractary time: %d\n", nsga2Params.max_refracttime);
    printf(" > Min refractary time: %d\n", nsga2Params.min_refracttime);
    printf(" > Max R: %d\n", nsga2Params.max_R);
    printf(" > Min R: %d\n", nsga2Params.min_R);

    printf(" > Max latency: %d\n", nsga2Params.max_latency);
    printf(" > Min latency: %d\n", nsga2Params.min_latency);
    printf(" > Max learning rule: %d\n", nsga2Params.max_learning_rule);
    printf(" > Min learning rule: %d\n", nsga2Params.min_learning_rule);
    printf(" > Max weight: %e\n", nsga2Params.max_weight);
    printf(" > Min weight: %e\n", nsga2Params.min_weight);

    printf(" > Dataset type: %d\n", nsga2Params.dataset_type);
    printf(" > Dataset train samples directory: %s\n", nsga2Params.train_dataset_dir);
    printf(" > Dataset train labels directory: %s\n", nsga2Params.train_labels_dir);
    printf(" > Dataset test samples directory: %s\n", nsga2Params.test_dataset_dir);
    printf(" > Dataset test labels directory: %s\n", nsga2Params.test_labels_dir);
    printf(" > N train samples: %d\n", nsga2Params.n_train_samples);
    printf(" > N test samples: %d\n", nsga2Params.n_test_samples);
    printf(" > N classes: %d\n", nsga2Params.n_classes);
    printf(" > Image size: %d\n", nsga2Params.image_size);
    printf(" > Bins: %d\n", nsga2Params.bins);

    printf(" > Mode: %d\n", nsga2Params.mode);
    printf(" > N repetitions: %d\n", nsga2Params.n_repetitions);
    printf(" > N samples: %d\n", nsga2Params.n_samples);

    printf("\n = == === ===== === == = \n Input parameters printed!\n = == === ===== === == = \n");

#endif


    return nsga2Params;
}



void InitNSGA2(NSGA2Type *nsga2Params, void *inp, void *out)
{
    int i, j ,l;
    
#ifdef DEBUG1
    printf("\n= == === ===== === == = \n Initializing NSGAII \n = == === ===== === == = \n");
#endif    

    // Initialize the files...
    fpt1 = fopen("initial_pop.out","w"); // file to store initial population
    fpt2 = fopen("final_pop.out","w"); // file to store final population
    fpt3 = fopen("best_pop.out","w"); // file to store best population
    fpt4 = fopen("all_pop.out","w"); // file to store all populations
    fpt5 = fopen("params.out","w"); // file to store execution parameters
    fprintf(fpt1,"# This file contains the data of initial population\n");
    fprintf(fpt2,"# This file contains the data of final population\n");
    fprintf(fpt3,"# This file contains the data of final feasible population (if found)\n");
    fprintf(fpt4,"# This file contains the data of all generations\n");
    fprintf(fpt5,"# This file contains information about inputs as read by the program\n");
    
    fprintf(fpt5,"\n Population size = %d",nsga2Params->popsize);
    fprintf(fpt5,"\n Number of generations = %d",nsga2Params->ngen);
    fprintf(fpt5,"\n Number of objective functions = %d",nsga2Params->nobj);
    fprintf(fpt5,"\n Number of constraints = %d",nsga2Params->ncon);
    fprintf(fpt5,"\n Number of real variables = %d",nsga2Params->nreal);
    if (nsga2Params->nreal!=0)
    {
        for (i=0; i<nsga2Params->nreal; i++)
        {
            fprintf(fpt5,"\n Lower limit of real variable %d = %e",i+1,nsga2Params->min_realvar[i]);
            fprintf(fpt5,"\n Upper limit of real variable %d = %e",i+1,nsga2Params->max_realvar[i]);
        }
        fprintf(fpt5,"\n Probability of crossover of real variable = %e",nsga2Params->pcross_real);
        fprintf(fpt5,"\n Probability of mutation of real variable = %e",nsga2Params->pmut_real);
        fprintf(fpt5,"\n Distribution index for crossover = %e",nsga2Params->eta_c);
        fprintf(fpt5,"\n Distribution index for mutation = %e",nsga2Params->eta_m);
    }
    fprintf(fpt5,"\n Number of binary variables = %d",nsga2Params->nbin);
    if (nsga2Params->nbin!=0)
    {
        for (i=0; i<nsga2Params->nbin; i++)
        {
            fprintf(fpt5,"\n Number of bits for binary variable %d = %d",i+1,nsga2Params->nbits[i]);
            fprintf(fpt5,"\n Lower limit of binary variable %d = %e",i+1,nsga2Params->min_binvar[i]);
            fprintf(fpt5,"\n Upper limit of binary variable %d = %e",i+1,nsga2Params->max_binvar[i]);
        }
        fprintf(fpt5,"\n Probability of crossover of binary variable = %e",nsga2Params->pcross_bin);
        fprintf(fpt5,"\n Probability of mutation of binary variable = %e",nsga2Params->pmut_bin);
    }
    fprintf(fpt5,"\n Seed for random number generator = %e",nsga2Params->seed);
    nsga2Params->bitlength = 0;
    if (nsga2Params->nbin!=0)
    {
        for (i=0; i<nsga2Params->nbin; i++)
        {
            nsga2Params->bitlength += nsga2Params->nbits[i];
        }
    }
    fprintf(fpt1,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nsga2Params->nobj,nsga2Params->ncon,nsga2Params->nreal,nsga2Params->bitlength);
    fprintf(fpt2,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nsga2Params->nobj,nsga2Params->ncon,nsga2Params->nreal,nsga2Params->bitlength);
    fprintf(fpt3,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nsga2Params->nobj,nsga2Params->ncon,nsga2Params->nreal,nsga2Params->bitlength);
    fprintf(fpt4,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nsga2Params->nobj,nsga2Params->ncon,nsga2Params->nreal,nsga2Params->bitlength);
    nsga2Params->nbinmut = 0;
    nsga2Params->nrealmut = 0;
    nsga2Params->nbincross = 0;
    nsga2Params->nrealcross = 0;
    nsga2Params->ncon = 0;
    

    //  =========================  //
    // == MY CHANGES START HERE == //
    //  =========================  //


    /* Initialize structures with existing motifs information: number of neurons, internal connectivity... */
    initialize_motifs();



#ifdef DEBUG1
    printf(" > Motifs initialized!\n\n");
#endif

    /* Allocate memory for populations: parent population, children population, and mixed population */
    parent_pop = (population *)calloc(1, sizeof(population));
    child_pop = (population *)calloc(1, sizeof(population));
    mixed_pop = (population *)calloc(1, sizeof(population));

#ifdef DEBUG1
    printf(" > Allocating memory for populations...\n");
#endif

    allocate_memory_pop (nsga2Params, parent_pop, nsga2Params->popsize);
    allocate_memory_pop (nsga2Params, child_pop, nsga2Params->popsize);
    allocate_memory_pop (nsga2Params, mixed_pop, 2*nsga2Params->popsize);

#ifdef DEBUG1
    printf(" > Memory allocated!\n\n");
#endif

    // randomize execution
    randomize(nsga2Params->seed);


    // initialize the parent population
#ifdef DEBUG1
    printf(" > Initializing first population...\n");
#endif


    /* Initialize the parent population */
    initialize_pop (nsga2Params,  parent_pop);


#ifdef DEBUG1
    printf(" > First population initialized!\n\n");
#endif
    
#ifdef DEBUG2
    // print population
    print_individuals(nsga2Params, parent_pop);
#endif

#ifdef DEBUG1
    printf(" > Decoding population...\n");
#endif

    decode_pop(nsga2Params, parent_pop); // in my case generate the spiking neural network

#ifdef DEBUG1
    printf(" > Population decoded!\n\n");
#endif

/*#ifdef DEBUG2
    // print decoded networks
    print_networks(nsga2Params, parent_pop);
#endif*/
    
        /*#ifdef DEBUG3
            // print networks information
            for(j = 0; j<nsga2Params->popsize; j++){
                if(j == 2){
                    printf(" > > Printing individual %d...\n", j);
                    
                    printf(" > > > Number of motifs = %d: ", parent_pop->ind[j].n_motifs);
                    new_motif_t *motif_node = parent_pop->ind[j].motifs_new;
                    while(motif_node != NULL){
                        printf("%d ", motif_node->motif_type);
                        motif_node = motif_node->next_motif;
                    }
                    printf("\n");

                    printf(" > > > Number of neurons = %d\n", parent_pop->ind[j].n_neurons);
                    printf(" > > > Number of input neurons = %d\n", parent_pop->ind[j].n_input_neurons);
                    printf(" > > > Number of synapses = %d\n", parent_pop->ind[j].n_synapses);
                    printf(" > > > Number of input synapses = %d\n", parent_pop->ind[j].n_input_synapses);

                    // print connectivity matrix
                    int *result_matrix = (int *)calloc(parent_pop->ind[j].n_neurons * parent_pop->ind[j].n_neurons, sizeof(int));
                    get_complete_matrix_from_dynamic_list(result_matrix, parent_pop->ind[j].connectivity_matrix, parent_pop->ind[j].n_neurons);

                    print_synapses_dynamic_list(parent_pop->ind[j].connectivity_matrix);
                    print_connectivity_matrix(result_matrix, parent_pop->ind[j].n_neurons);

                    free(result_matrix);

                    fflush(stdout);
                }
            }
        #endif*/

    // initialize variables to store input spike trains
#ifdef DEBUG1
    printf(" > Loading input spike trains...\n");
#endif
    // TODO: this MUST be a function and more general, as not always images are loaded
    int pT = 100;
    int pDT = 1;
    int bins = (int)(pT/pDT);

    image_dataset.bins = nsga2Params->bins;
    image_dataset.image_size = nsga2Params->image_size;
    image_dataset.n_images = nsga2Params->n_train_samples;
    image_dataset.n_classes = nsga2Params->n_classes;

    image_dataset.images = malloc(image_dataset.n_images * sizeof(image_dataset_t)); // alloc memory for images
    image_dataset.labels = malloc(image_dataset.n_images * sizeof(int)); // alloc memory for images
    for(i = 0; i<image_dataset.n_images; i++){
        image_dataset.images[i].image = malloc(image_dataset.image_size * sizeof(int *));
        for(j=0; j<image_dataset.image_size; j++)
            image_dataset.images[i].image[j] = malloc(image_dataset.bins * sizeof(int));
    }

    // load input spike trains
    FILE *f = fopen(nsga2Params->train_dataset_dir, "r");
    if(f == NULL){ 
        printf(" > Error opening the file\n\n");
        exit(1);
    }

    FILE *f_labels = fopen(nsga2Params->train_labels_dir, "r");
    if(f_labels == NULL){
        printf(" > Error opening the labels file\n");
        exit(1);
    }

#ifdef DEBUG1
    printf(" > Reading input spike trains and labels...\n");
#endif
    for(i=0; i<image_dataset.n_images; i++){
        fscanf(f_labels, "%d", &(image_dataset.labels[i]));
        for(j=0; j<image_dataset.image_size; j++){
            fscanf(f, "%d", &(image_dataset.images[i].image[j][0]));
            for(l = 1; l<image_dataset.images[i].image[j][0]; l++){
                fscanf(f, "%d", &(image_dataset.images[i].image[j][l]));
            }
        }
    }

#ifdef DEBUG1
    printf(" > Input spikes and labels loaded!\n\n");
#endif    

#ifdef DEBUG1
    printf(" > Evaluating parent population...\n");
#endif 

    evaluate_pop (nsga2Params, parent_pop, inp, out);

#ifdef DEBUG1
    printf(" > Parent population evaluated!\n\n");

    printf(" > Deallocating SNN structure...\n");
#endif

    deallocate_memory_pop_snn_only(nsga2Params, parent_pop, nsga2Params->popsize);

#ifdef DEBUG1
    printf(" > SNN structure deallocated!\n");
    
    printf(" > Computing rank and crowding distance...\n");
#endif

    assign_rank_and_crowding_distance (nsga2Params, parent_pop);

#ifdef DEBUG1
    printf(" > Rank and crowding distance assigned!\n\n");
#endif

    //report_pop (nsga2Params, parent_pop, fpt1);
    //printf(" > Report population\n");
    
    fprintf(fpt4,"# gen = 1\n");
    //report_pop(nsga2Params, parent_pop,fpt4);
    //printf("\n -- Generation 1 --");
    
    //if (nsga2Params->choice!=0)    
    //    onthefly_display (nsga2Params, parent_pop,gp,1);

    fflush(fpt1);
    fflush(fpt2);
    fflush(fpt3);
    fflush(fpt4);
    fflush(fpt5);

    exit(0);

    return;
}




int NSGA2(NSGA2Type *nsga2Params, void *inp, void *out)
{
    int i, j, s;
    
    // NSGA2 simulation: loop over ngen generations
    for (i=2; i<=nsga2Params->ngen; i++)
    {
        printf("\n= == === ===== === == = \n Computing Generation %d \n = == === ===== === == = \n", i);

    #ifdef DEBUG1

        printf(" > Selecting individuals from parent population for child population...\n");

        fflush(stdout);
    #endif 

        selection (nsga2Params,  parent_pop, child_pop);

    #ifdef DEBUG1
        printf(" > New population selected!\n\n");

        /*#ifdef DEBUG3
            // print networks information
            for(j = 0; j<nsga2Params->popsize; j++){
                printf(" > > Printing individual %d...\n", j);
                
                printf(" > > > Number of motifs = %d: ", child_pop->ind[j].n_motifs);
                new_motif_t *motif_node = child_pop->ind[j].motifs_new;
                while(motif_node != NULL){
                    printf("%d ", motif_node->motif_type);
                    motif_node = motif_node->next_motif;
                }
                printf("\n");

                printf(" > > > Number of neurons = %d\n", child_pop->ind[j].n_neurons);
                printf(" > > > Number of input neurons = %d\n", child_pop->ind[j].n_input_neurons);
                printf(" > > > Number of synapses = %d\n", child_pop->ind[j].n_synapses);
                printf(" > > > Number of input synapses = %d\n", child_pop->ind[j].n_input_synapses);

                // print connectivity matrix
                int *result_matrix = (int *)calloc(child_pop->ind[j].n_neurons * child_pop->ind[j].n_neurons, sizeof(int));
                get_complete_matrix_from_dynamic_list(result_matrix, child_pop->ind[j].connectivity_matrix, child_pop->ind[j].n_neurons);

                print_synapses_dynamic_list(child_pop->ind[j].connectivity_matrix);
                print_connectivity_matrix(result_matrix, child_pop->ind[j].n_neurons);

                free(result_matrix);

                fflush(stdout);
            }
        #endif*/

        printf(" > Mutating population...\n");

        printf(" > Motif types: ");



        fflush(stdout);
    #endif

        mutation_pop (nsga2Params,  child_pop);

    #ifdef DEBUG1
        printf(" > Population mutated!\n\n");

        #ifdef DEBUG3
            // print networks information
            for(j = 0; j<nsga2Params->popsize; j++){
                printf(" > > Printing individual %d...\n", j);

                printf(" > > > Number of motifs = %d: ", child_pop->ind[j].n_motifs);
                new_motif_t *motif_node = child_pop->ind[j].motifs_new;
                while(motif_node != NULL){
                    printf("%d ", motif_node->motif_type);
                    motif_node = motif_node->next_motif;
                }
                printf("\n");

                printf(" > > > Number of neurons = %d\n", child_pop->ind[j].n_neurons);
                printf(" > > > Number of input neurons = %d\n", child_pop->ind[j].n_input_neurons);
                printf(" > > > Number of synapses = %d\n", child_pop->ind[j].n_synapses);
                printf(" > > > Number of input synapses = %d\n", child_pop->ind[j].n_input_synapses);

                // print connectivity matrix
                int *result_matrix = (int *)calloc(child_pop->ind[j].n_neurons * child_pop->ind[j].n_neurons, sizeof(int));
                get_complete_matrix_from_dynamic_list(result_matrix, child_pop->ind[j].connectivity_matrix, child_pop->ind[j].n_neurons);

                //print_synapses_dynamic_list(child_pop->ind[j].connectivity_matrix);
                print_connectivity_matrix(result_matrix, child_pop->ind[j].n_neurons);

                free(result_matrix);

                fflush(stdout);
            }
        #endif

        printf(" > Decoding child population...\n");

        fflush(stdout);
    #endif

        decode_pop(nsga2Params,  child_pop);

    #ifdef DEBUG1
        printf(" > Child population decoded!\n\n");        

        printf(" > Evaluating population...\n");

        fflush(stdout);
    #endif

        evaluate_pop(nsga2Params,  child_pop, inp, out);

   #ifdef DEBUG1
        printf(" > Population evaluated!\n\n");
    
        printf(" > Deallocating SNN structure...\n");
    #endif

        deallocate_memory_pop_snn_only(nsga2Params, child_pop, nsga2Params->popsize);

    #ifdef DEBUG1
        printf(" > Merging parent population and child population...\n");

        fflush(stdout);
    #endif

        merge (nsga2Params,  parent_pop, child_pop, mixed_pop);

    #ifdef DEBUG1
        printf(" > Populations merged!\n\n");

        printf(" > Deallocating parent population...\n");

        fflush(stdout);
    #endif

        deallocate_memory_pop(nsga2Params, parent_pop, nsga2Params->popsize);

    #ifdef DEBUG1
        printf(" > Parent population deallocated!\n\n");

        printf(" > Allocating parent population again...\n");

        fflush(stdout);
    #endif 

        allocate_memory_pop(nsga2Params, parent_pop, nsga2Params->popsize);

    #ifdef DEBUG1
        printf(" > Parent population allocated!\n\n");

        printf(" > Filling parent population...\n");

        fflush(stdout);
    #endif

        fill_nondominated_sort (nsga2Params,  mixed_pop, parent_pop);

        // TODO: this is not optimal
    #ifdef DEBUG1
        printf(" > Parent population filled!\n\n");

        printf(" > Decoding parent population...\n");

        fflush(stdout);
    #endif
        //decode_pop(nsga2Params, parent_pop);

    #ifdef DEBUG1
        printf(" > Parent population decoded!\n\n");

        fflush(stdout);
    #endif
        /* Comment following four lines if information for all
         generations is not desired, it will speed up the execution */
        //fprintf(fpt4,"# gen = %d\n",i);
        //report_pop(nsga2Params, parent_pop,fpt4);
        //fflush(fpt4);
        //if (nsga2Params->choice!=0)    
        //    onthefly_display (nsga2Params, parent_pop,gp,i);
        //printf("\n -- Generation %d --", i);
    }
    printf("\n Generations finished, now reporting solutions");
    fflush( stdout );

    //report_pop(nsga2Params,  parent_pop,fpt2);
    //report_feasible(nsga2Params,  parent_pop,fpt3);
    
    if (nsga2Params->nreal!=0)
    {
        fprintf(fpt5,"\n Number of crossover of real variable = %d",nsga2Params->nrealcross);
        fprintf(fpt5,"\n Number of mutation of real variable = %d",nsga2Params->nrealmut);
    }
    if (nsga2Params->nbin!=0)
    {
        fprintf(fpt5,"\n Number of crossover of binary variable = %d",nsga2Params->nbincross);
        fprintf(fpt5,"\n Number of mutation of binary variable = %d",nsga2Params->nbinmut);
    }
    
    // Closing the files and freeing up memories...
    fflush(stdout);
    fflush(fpt1);
    fflush(fpt2);
    fflush(fpt3);
    fflush(fpt4);
    fflush(fpt5);
    fclose(fpt1);
    fclose(fpt2);
    fclose(fpt3);
    fclose(fpt4);
    fclose(fpt5);
    if (nsga2Params->choice!=0)
    {
        pclose(gp);
    }
    if (nsga2Params->nreal!=0)
    {
        free (nsga2Params->min_realvar);
        free (nsga2Params->max_realvar);
    }
    if (nsga2Params->nbin!=0)
    {
        free (nsga2Params->min_binvar);
        free (nsga2Params->max_binvar);
        free (nsga2Params->nbits);
    }

    deallocate_memory_pop (nsga2Params,  parent_pop, nsga2Params->popsize);
    deallocate_memory_pop (nsga2Params,  child_pop, nsga2Params->popsize);
    deallocate_memory_pop (nsga2Params,  mixed_pop, 2*nsga2Params->popsize);
    free (parent_pop);
    free (child_pop);
    free (mixed_pop);


    printf("\n Routine successfully exited \n");
    return (0);
}


void print_nsga2Params(NSGA2Type *nsga2Params){
    int i;
    
    printf("NSGA2 Parameters:\n");
    printf("\n seed number is : %lf", nsga2Params->seed);
    printf("\n population size read is : %d",nsga2Params->popsize);
    printf("\n number of generations read is : %d",nsga2Params->ngen);
    printf("\n number of objectives entered is : %d",nsga2Params->nobj);
    printf("\n number of constraints entered is : %d",nsga2Params->ncon);
    printf("\n number of real variables entered is : %d",nsga2Params->nreal);
    printf("\n variables bounds: ");
    for (i=0; i<nsga2Params->nreal; i++){
        printf("[%lf", nsga2Params->min_realvar[i]);
        printf(" %lf], ", nsga2Params->max_realvar[i]);
    }
    printf("\n Probability of crossover entered is : %e",nsga2Params->pcross_real);
    printf("\n Probability of mutation entered is : %e",nsga2Params->pmut_real);
    printf("\n The eta_c entered is : %e",nsga2Params->eta_c);
    printf("\n The eta_m entered is : %e",nsga2Params->eta_m);
    if (nsga2Params->nbin != 0){
        printf ("\n number of binary variables entered is : %d",nsga2Params->nbin);
        printf("\n Probability of crossover entered is : %e",nsga2Params->pcross_bin);
        printf("\n Probability of mutation entered is : %e",nsga2Params->pmut_bin);
    }
}




