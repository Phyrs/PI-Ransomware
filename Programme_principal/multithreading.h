#ifndef MULTITHREAD_CHISQUARED
#define MULTITHREAD_CHISQUARED


// Semaphore_test.cpp : définit le point d'entrée pour l'application console.
//

// !!! pas oublier de free structures apres malloc
// pas oublier de free les handle apres malloc

#include "stdafx.h"
#include <windows.h>
#include <fstream>

#define MAX_THREAD_COUNT 1
#define BUFFER_SIZE 1000000
#define ANALYZERS_NUMBER 2

using namespace std;

/**
 * Returns the size (in bytes) of targetted file
 * @param filename {char*}
 * @return {int} file size
 */
std::ifstream::pos_type filesize(const char* filename);

/**
 * Tells if given data sample can be related to specified multinomial distribution, within given error margin
 * @param sample_sd {double}: standard deviation of sample
 * @param sample_size {uint}: size of sample
 * @param p_value {double}: error margin
 * @return Boolean: true if there is concordance
 */
int chi_squared_test(double sample_sd, unsigned sample_size, double p_value);

/**
 * Detects generic encryption on given file (i.e. the file's bytes values have an homogeneous repartition)
 * @param file_path {char*}
 * @param start {int}: starting index (optionnal)
 * @param end {int}: last index (optionnal)
 */
DWORD WINAPI chi_square_byte_analysis( LPVOID file_data );

void init_threading();

void end_threading();

void launch_thread(const char *file_path);


#endif