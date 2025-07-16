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

	// init population and individuals
	InitNSGA2(&nsga2Params, inp, out);

	// loop over generations
	NSGA2(&nsga2Params, inp, out);

	return 0;
}