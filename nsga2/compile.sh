#gcc -Wall -g  -c allocate.c
#gcc -Wall -g  -c auxiliary.c
#gcc -Wall -g  -c crossover.c
#gcc -Wall -g  -c crowddist.c
#gcc -Wall -g  -c decode.c
#gcc -Wall -g  -c display.c
#gcc -Wall -g  -c dominance.c
#gcc -Wall -g  -c eval.c
#gcc -Wall -g  -c fillnds.c
#gcc -Wall -g  -c initialize.c
#gcc -Wall -g  -c list.c
'''gcc -Wall -g  -c merge.c
gcc -Wall -g  -c mutation.c
gcc -Wall -g  -c nsga2.c
gcc -Wall -g  -c nsga2r.c
gcc -Wall -g  -c problemdef.c
gcc -Wall -g  -c rand.c
gcc -Wall -g  -c rank.c
gcc -Wall -g  -c report.c
gcc -Wall -g  -c sort.c
gcc -Wall -g  -c tourselect.c
gcc  allocate.o auxiliary.o crossover.o crowddist.o decode.o display.o dominance.o eval.o fillnds.o initialize.o list.o merge.o mutation.o nsga2.o nsga2r.o problemdef.o rand.o rank.o report.o sort.o tourselect.o -o nsga2r -lm
'''
gcc -o nsga2r allocate.c auxiliary.c crossover.c crowddist.c decode.c display.c dominance.c eval.c fillnds.c initialize.c list.c merge.c mutation.c nsga2.c nsga2r.c problemdef.c rand.c rank.c report.c sort.c tourselect.c ../src/snn_library.c -lm
gcc -I. -I../include -o nsga2r allocate.c auxiliary.c crossover.c crowddist.c decode.c display.c dominance.c eval.c fillnds.c initialize.c list.c merge.c mutation.c nsga2.c nsga2r.c problemdef.c rand.c rank.c report.c sort.c tourselect.c ../src/snn_library.c ../src/load_data.c ../src/helpers.c ../src/neuron_models/lif_neuron.c ../src/training_rules/stdp.c ../src/simulations/simulations.c -lm