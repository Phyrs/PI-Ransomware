#include "Analyzer_Chi_Squared.h"

double a_bytes_sum, a_bytes_square_sum, a_bytes_count;
char* a_file_path;
int a_result;
double a_p_value= 0.05;

/**
 * Intitialize analyzer variables
 */
void analyzer_init(char* file_path){
	a_bytes_sum= 0;
	a_bytes_square_sum= 0;
	a_bytes_count= 0;
	a_result= 0;
	a_file_path= file_path;
}

/**
 * Tells if the buffer currently pointed by the parser is interresing for the signature's calculation
 */
int analyzer_in_range(int cursor){
	return 1;
}

/**
 * Process current file block
 */
void analyzer_process(char* buffer, int file_count){
	int i;
	for(i= 0; i<file_count; i++){
		a_bytes_sum+= buffer[i] & 0xff;
		a_bytes_square_sum+= pow(((double)(buffer[i] & 0xff)), 2);
		a_bytes_count++;
	}
}

/**
 * Tells if given data sample can be related to specified multinomial distribution, within given error margin
 */
void analyzer_compute(){
	using boost::math::chi_squared;
	using boost::math::quantile;
	using boost::math::complement;
	using boost::math::cdf;

	double sample_sd= sqrt( a_bytes_square_sum/a_bytes_count - pow(((double)a_bytes_sum/a_bytes_count), 2) ); // standard deviation of file's bytes

	// Experimental model: each byte's possible value has the same probability of apparition (1/256)
	double model_sd= sqrt( pow((double)256, 2)/12); // model's standard deviation

	double t_stat = (a_bytes_count - 1) * (sample_sd / model_sd) * (sample_sd / model_sd);  // test statistic

	chi_squared dist(a_bytes_count - 1); // chi-squared distribution
	double ucv2 = quantile(complement(dist, a_p_value/2)); // upper quantile
	double lcv2 = quantile(dist, a_p_value/2); // lower quantile
   
	// Null-Hypothesis: (are the file's bytes following a multinomial distribution with a mean of 127.5 ?) 
	if(t_stat < lcv2 || t_stat > ucv2){
		a_result= 0;
	}else{
		a_result= 1;
	}
}

/**
 * Print / save in a log file the a_result of the scan
 */
void analyzer_result(){
	printf("%s: %d\n", a_file_path, a_result);
}