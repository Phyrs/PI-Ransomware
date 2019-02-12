#include "Analyzer_Chi_Squared.h"

/**
 * Intitialize analyzer variables
 */
AnalyzerChiSquared::AnalyzerChiSquared(char* file_path) : Analyzer(file_path) {
	a_bytes_sum= 0;
	a_bytes_square_sum= 0;
	a_bytes_count= 0;
	a_result= 0;
	a_p_value = 0.05;
}

/**
 * Tells if the buffer currently pointed by the parser is interresing for the signature's calculation
 */
bool AnalyzerChiSquared::analyzer_in_range(long cursor){
	return true;
}

/**
 * Process current file block
 */
void AnalyzerChiSquared::analyzer_process(char* buffer, int readbytes, long cursor){
	int i;
	for(i= 0; i<readbytes; i++){
		a_bytes_sum+= buffer[i] & 0xff;
		a_bytes_square_sum+= pow(((double)(buffer[i] & 0xff)), 2);
		a_bytes_count++;
	}
}

/**
 * Tells if given data sample can be related to specified multinomial distribution, within given error margin
 */
void AnalyzerChiSquared::analyzer_compute(){
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
void AnalyzerChiSquared::analyzer_result(){
	printf(" - All the file's bytes have been ciphered : %s\n", (a_result ? "yes" : "no"));
}

int AnalyzerChiSquared::get_result(){
	return a_result;
}


bool AnalyzerChiSquared::is_ciphered_by_ransomware(){
	return (a_result ? true : false);
}


std::string AnalyzerChiSquared::get_ransomware_name(){
	return std::string("unknown");
}