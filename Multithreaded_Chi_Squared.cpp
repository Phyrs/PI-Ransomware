#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <boost/math/distributions/chi_squared.hpp>

#define MAX_THREAD_COUNT 4

using namespace std;

double p_value= 0.05;

HANDLE WRITE_MUTEX;             // to ensure that write operations on log output are safe
HANDLE THREAD_COUNT_SEMAPHORE;

// Represents the data allocated to a thread for file-analysis
struct THREAD_DATA_ {
	int thread_id;                                   // -2 if not related to a thread, -1 if related thread is waiting for CloseHandle
	HANDLE thread_handle; 
	char thread_buffer[1000000];                     // buffer for file reading
	char file_path[MAX_PATH];
    int file_start;                                  // when only a part of the file must be analyzed
    int file_end;                                    // idem
} THREAD_DATA_DEFAULT= {-1, NULL, 0, NULL, NULL, 0, 0};
typedef struct THREAD_DATA_ THREAD_DATA;

/**
 * Returns the size (in bytes) of targetted file
 * @param filename {char*}
 * @return {int} file size
 */
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream selected_file(filename, std::ios::ate | std::ios::binary);
    return selected_file.tellg(); 
}

/**
 * Tells if given data sample can be related to specified multinomial distribution, within given error margin
 * @param sample_sd {double}: standard deviation of sample
 * @param sample_size {uint}: size of sample
 * @param p_value {double}: error margin
 * @return Boolean: true if there is concordance
 */
int chi_squared_test(double sample_sd, unsigned sample_size, double p_value){
	using boost::math::chi_squared;
	using boost::math::quantile;
	using boost::math::complement;
	using boost::math::cdf;

	// Experimental model: each byte's possible value has the same probability of apparition (1/256)
	double model_sd= sqrt( pow((double)256, 2)/12); // model's standard deviation

	double t_stat = (sample_size - 1) * (sample_sd / model_sd) * (sample_sd / model_sd);  // test statistic

	chi_squared dist(sample_size - 1); // chi-squared distribution
	double ucv2 = quantile(complement(dist, p_value/2)); // upper quantile
	double lcv2 = quantile(dist, p_value/2); // lower quantile
   
	// Null-Hypothesis: (are the file's bytes following a multinomial distribution with a mean of 127.5 ?) 
	if(t_stat < lcv2 || t_stat > ucv2)
		return 0;
	return 1;
}

/**
 * Detects generic encryption on given file (i.e. the file's bytes values have an homogeneous repartition)
 * @param file_path {char*}
 * @param start {int}: starting index (optionnal)
 * @param end {int}: last index (optionnal)
 */
DWORD WINAPI chi_square_byte_analysis( LPVOID file_data ){
    THREAD_DATA* td= ((THREAD_DATA*)file_data);
    
	int i, bytes_count= 0;
	double bytes_sum= 0, bytes_square_sum= 0;

	if(td->file_end-td->file_start<=0){
		td->file_start= 0;
		td->file_end= (int)filesize(td->file_path);
	}
	// i - Open the file and process its bytes
	ifstream file(td->file_path, ios::binary);
	do{
		file.read(td->thread_buffer, sizeof(td->thread_buffer));
		for(i= 0; i<file.gcount(); i++){
			if(bytes_count>= td->file_start && bytes_count<td->file_end){
				bytes_sum+= td->thread_buffer[i] & 0xff;
				bytes_square_sum+= pow(((double)(td->thread_buffer[i] & 0xff)), 2);
			}
			bytes_count++;
			if(bytes_count>=td->file_end)
				break;
		}
		if(bytes_count>=td->file_end)
			break;
	}while(!file.eof());
	file.close();
	// ii - Chi-Square test
	double bytes_sd= sqrt( bytes_square_sum/(bytes_count-td->file_start) - pow(((double)bytes_sum/(bytes_count-td->file_start)), 2) ); // standard deviation of file's bytes
	int file_status= chi_squared_test(bytes_sd, bytes_count-td->file_start, p_value);

	// Wait for the write operation to be secure
	WaitForSingleObject( 
            WRITE_MUTEX,    // handle to mutex
            INFINITE);      // no time-out interval

	// TODO: replace by write operation in log file
	printf("%s: %d\n", td->file_path, file_status);

	ReleaseMutex(WRITE_MUTEX);

	td->thread_id= -1;

	// Tells the main process that the current THREAD_DATA can be allocated to another thread
    ReleaseSemaphore(THREAD_COUNT_SEMAPHORE, // handle to semaphore
					 1,                      // increase count by one
                     NULL);                  // not interested in previous count
	return 0;
}

int main( void )
{
	WRITE_MUTEX = CreateMutex(NULL,          // default security attributes
						     FALSE,          // initially not owned
						     NULL);          // unnamed mutex

    THREAD_COUNT_SEMAPHORE = CreateSemaphore(NULL,              // default security attributes
								             MAX_THREAD_COUNT,  // initial count
								             MAX_THREAD_COUNT,  // maximum count
								             NULL);             // unnamed semaphore

	int i,j;
	// Initialisation of shared memory structure
    THREAD_DATA* td_[MAX_THREAD_COUNT];
	for(i= 0; i<MAX_THREAD_COUNT; i++){
		td_[i]= (THREAD_DATA*) malloc(sizeof(THREAD_DATA));
		*(td_[i])= THREAD_DATA_DEFAULT;
	}
	
    for( i=0; i < 12; i++ ){
		// Wait if MAX_THREAD_COUNT are already running
		WaitForSingleObject(THREAD_COUNT_SEMAPHORE,   // handle to semaphore
							INFINITE);                // zero-second time-out interval
		j= 0;
		while(td_[j]->thread_id!=-1)
			j++;
		if(td_[j]->thread_handle!=NULL){
			CloseHandle(td_[j]->thread_handle);
		}
		*(td_[j])= THREAD_DATA_DEFAULT;
		td_[j]->thread_id= j;

		// The following switch (along the above for-loop) simulates the hard-disk browsing function; cases 0-4: must not be recognized, cases 5-9: must be recognized
		switch(i){
			case 0:
				strcpy_s(td_[j]->file_path, "C:/WINDOWS/system32/MRT.exe");
				break;
			case 1:
				strcpy_s(td_[j]->file_path, "C:/WinDDK/7600.16385.1/Debuggers/debugger.chm");
				break;
			case 2:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/vipa_big.txt");
				break;
			case 3:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/Copie de vipa_big.txt");
				break;
			case 4:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/test.txt.locked");
				break;
			case 5:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/email-Johnmen.24@aol.com.ver-CL 1.2.0.0.id-QMCRJYODTJYNDSIXNCRGWLBQGVLZQEUJZOET-16@01@2019 14@15@037221765.randomname-BIEZZTNJEYTNIDYSNICXSMHBWRLGCW.SNI.cbf");
				break;
			case 6:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/email-JohnmenII");
				break;
			case 7:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/test.txt.lockedbis");
				break;
			case 8:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/Copie de vipa_big.txt");
				td_[j]->file_start= 0;
				td_[j]->file_end= 768;
				break;
			case 9:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/test.txt.lockedbis");
				break;
			case 10:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/test.txt.lockedbis");
				break;
			case 11:
				strcpy_s(td_[j]->file_path, "C:/Documents and Settings/Administrateur/Bureau/test.txt.lockedbis");
				break;
		}

        td_[j]->thread_handle= CreateThread(NULL,                                              // default security attributes
                     0,                                                 // default stack size
                     (LPTHREAD_START_ROUTINE) chi_square_byte_analysis, // start routine
                     td_[j],                                            // no thread function arguments
                     0,                                                 // default creation flags
                     NULL);                             // receive thread identifier
    }
	// Close handles
	i= 1;
	while(i!=0){
		i= 0;
		// i: count number of remaining threads
		for(j= 0; j<MAX_THREAD_COUNT; j++){
			if(td_[j]->thread_id>=0)
				i++;
		}
		// ii: consume 1 semaphore unit (include the case when more threads were created compared to the number of targetted files)
		WaitForSingleObject(THREAD_COUNT_SEMAPHORE,   // handle to semaphore
							INFINITE);                // zero-second time-out interval
		j= 0;
		// iii: if the consumed semaphore unit was related to a terminated thread, find it and close the handle
		while(j<MAX_THREAD_COUNT && (td_[j]->thread_id!=-1 || td_[j]->thread_handle==NULL))
			j++;
		if(j<MAX_THREAD_COUNT && td_[j]->thread_handle!=NULL){
			CloseHandle(td_[j]->thread_handle);
			td_[j]->thread_handle= NULL;
			td_[j]->thread_id= -2;
			i--;
		}
	}
	// Free shared memory
	for(j= 0; j<MAX_THREAD_COUNT; j++)
		delete td_[j];
	CloseHandle(WRITE_MUTEX);
    CloseHandle(THREAD_COUNT_SEMAPHORE);
	while(1);
    return 0;
}