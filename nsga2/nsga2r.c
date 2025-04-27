# include "rand.h"
# include "nsga2.h"

int main(int argc, char **argv)
{
	/* code */
	NSGA2Type nsga2Params;
	void *inp = NULL;
	void *out = NULL;

	// read input parameters
	nsga2Params = ReadParameters(argc, argv);
	printf("\nInput parameters readed\n");

	// Init population and individuals
	InitNSGA2(&nsga2Params, inp, out);
	printf("\nNSGAII initialized\n");

	// loop over generations
	NSGA2(&nsga2Params, inp, out);
	printf("\nLoop finished\n");

	return 0;
}