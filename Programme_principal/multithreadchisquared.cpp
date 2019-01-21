// Semaphore_test.cpp : définit le point d'entrée pour l'application console.
//

// !!! pas oublier de free structures apres malloc
// pas oublier de free les handle apres malloc

#include "multithreadchisquared.h"
#include "Analyzer_Chi_Squared.h"

HANDLE WRITE_MUTEX;             // to ensure that write operations on log output are safe
HANDLE THREAD_COUNT_SEMAPHORE;

// Represents the data allocated to a thread for file-analysis
struct THREAD_DATA_ {
	int thread_id;                                   // -2 if not related to a thread, -1 if related thread is waiting for CloseHandle
	HANDLE thread_handle; 
	char thread_buffer[BUFFER_SIZE];                     // buffer for file reading
	char file_path[MAX_PATH];
    int file_start;                                  // when only a part of the file must be analyzed
    int file_end;                                    // idem
} THREAD_DATA_DEFAULT= {-1, NULL, NULL, NULL, 0, 0};
typedef struct THREAD_DATA_ THREAD_DATA;



THREAD_DATA* td_[MAX_THREAD_COUNT];

/**
 * Returns the size (in bytes) of targetted file
 * @param filename {char*}
 * @return {int} file size
 */
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream selected_file(filename, std::ios::ate | std::ios::binary);
	long test = selected_file.tellg();
    return selected_file.tellg(); 
}

/**
 * Detects generic encryption on given file (i.e. the file's bytes values have an homogeneous repartition)
 * @param file_path {char*}
 * @param start {int}: starting index (optionnal)
 * @param end {int}: last index (optionnal)
 */
DWORD WINAPI file_analysis( LPVOID file_data ){
    THREAD_DATA* td= ((THREAD_DATA*)file_data);
    
	// TODO: init all analyzers
	analyzer_init(td->file_path);
	int process= 0, bytes_count= 0;

	if(td->file_end-td->file_start<=0){
		td->file_start= 0;
		td->file_end= (int)filesize(td->file_path);
	}
	// Open the file and process its bytes
	ifstream file(td->file_path, ios::binary);
	file.clear();
	do{
		process= analyzer_in_range(file.tellg()); // TODO: for all analyzers
		if(process){
			file.read(td->thread_buffer, sizeof(td->thread_buffer));
			if(analyzer_in_range(file.tellg()))
				analyzer_process(td->thread_buffer, file.gcount());
		}else{
			file.seekg(BUFFER_SIZE, file.cur);
		}
	}while(!file.eof());
	file.close();
	// TODO: for all analyzers
	analyzer_compute();

	// Wait for the write operation to be secure
	WaitForSingleObject( 
            WRITE_MUTEX,    // handle to mutex
            INFINITE);      // no time-out interval


	analyzer_result();  // TODO: for all analyzers

	ReleaseMutex(WRITE_MUTEX); 

	td->thread_id= -1;

	// Tells the main process that the current THREAD_DATA can be allocated to another thread
    ReleaseSemaphore(THREAD_COUNT_SEMAPHORE, // handle to semaphore
					 1,                      // increase count by one
                     NULL);                  // not interested in previous count
	return 0;
}

void init_threading(){
	WRITE_MUTEX = CreateMutex(NULL,          // default security attributes
						     FALSE,          // initially not owned
						     NULL);          // unnamed mutex

    THREAD_COUNT_SEMAPHORE = CreateSemaphore(NULL,              // default security attributes
								             MAX_THREAD_COUNT,  // initial count
								             MAX_THREAD_COUNT,  // maximum count
								             NULL);             // unnamed semaphore

	int i;
	// Initialisation of shared memory structure
	for(i= 0; i<MAX_THREAD_COUNT; i++){
		td_[i]= (THREAD_DATA*) malloc(sizeof(THREAD_DATA));
		*(td_[i])= THREAD_DATA_DEFAULT;
	}
}

void end_threading(){
	// Close handles
	int i= 1, j = 0;
	while(i!=0){
		i= 0;
		// i: count number of remaining threads
		for(j= 0; j<MAX_THREAD_COUNT; j++){
			if(td_[j]->thread_id>=0)
				i++;
		}
		// ii: consume 1 semaphore unit (include the case when we created more threads than files to read)
		WaitForSingleObject(THREAD_COUNT_SEMAPHORE,   // handle to semaphore
							INFINITE);                // zero-second time-out interval
		j= 0;
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
}

void launch_thread(const char *file_path){
	// Wait if MAX_THREAD_COUNT are already running
		WaitForSingleObject(THREAD_COUNT_SEMAPHORE,   // handle to semaphore
							INFINITE);                // zero-second time-out interval
		int j= 0;
		while(td_[j]->thread_id!=-1)
			j++;
		if(td_[j]->thread_handle!=NULL){
			CloseHandle(td_[j]->thread_handle);
		}
		*(td_[j])= THREAD_DATA_DEFAULT;
		td_[j]->thread_id= j;

		strcpy_s(td_[j]->file_path, file_path);

        td_[j]->thread_handle= CreateThread(NULL,                                              // default security attributes
                     0,                                                 // default stack size
                     (LPTHREAD_START_ROUTINE) file_analysis, // start routine
                     td_[j],                                            // no thread function arguments
                     0,                                                 // default creation flags
                     NULL);  
}