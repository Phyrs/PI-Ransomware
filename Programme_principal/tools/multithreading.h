#ifndef MULTITHREAD
#define MULTITHREAD


// Semaphore_test.cpp�: d�finit le point d'entr�e pour l'application console.
//

// !!! pas oublier de free structures apres malloc
// pas oublier de free les handle apres malloc

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>

#define MAX_THREAD_COUNT 1
#define BUFFER_SIZE 1000000
#define ANALYZERS_NUMBER 4

using namespace std;

extern vector<std::string> ciphered_files_path;

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

/**
 * @param reportPath path to the general report in which the pgm will write the differents analyzers results.
 */
void init_threading(string reportPath);

void end_threading();

void launch_thread(const char *file_path);


#endif