#ifndef TEST_H
#define TEST_H

# include "nsga2.h"
# include "snn_library.h"

# define KRED "\x1B[31m"
# define KGRN "\x1B[32m"

void print_result(int result, int test_id);
void print_results(int *results, int n);
int compare_target_and_obtained_matrix(int *result_matrix, int *target_matrix, int n_neurons);

#endif