// PI_Universal_Detector.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <fstream>
#include <boost/math/distributions/chi_squared.hpp>

using namespace std;

double p_value= 0.05;
char stream_buffer[1000000];

// Returns the size (in bytes) of targetted file
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

	double model_sd= sqrt( pow((double)256, 2)/12); // model's standard deviation

	double t_stat = (sample_size - 1) * (sample_sd / model_sd) * (sample_sd / model_sd);

	chi_squared dist(sample_size - 1); // chi-squared distribution
	double ucv2 = quantile(complement(dist, p_value/2)); // upper quantile
	double lcv2 = quantile(dist, p_value/2); // lower quantile
   
	// Null-Hypothesis: (are the file's bytes following a multinomial distribution with a standard deviation of 127.5 ?) 
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
void chi_square_byte_analysis(char* file_path, int start= 0, int end= 0){
	int i, bytes_count= 0;
	double bytes_sum= 0, bytes_square_sum= 0;

	if(end-start<=0){
		start= 0;
		end= (int)filesize(file_path);
	}
	// i - Open the file and count its bytes by value
	ifstream file(file_path, ios::binary);
	do{
		file.read(stream_buffer, sizeof(stream_buffer));
		for(i= 0; i<file.gcount(); i++){
			if(bytes_count>= start && bytes_count<end){
				bytes_sum+= stream_buffer[i] & 0xff;
				bytes_square_sum+= pow(((double)(stream_buffer[i] & 0xff)), 2);
			}
			bytes_count++;
			if(bytes_count>=end)
				break;
		}
		if(bytes_count>=end)
			break;
	}while(!file.eof());
	file.close();
	// ii - Chi-Square test
	double bytes_sd= sqrt( bytes_square_sum/(bytes_count-start) - pow(((double)bytes_sum/(bytes_count-start)), 2) ); // standard deviation of file's bytes
	chi_squared_test(bytes_sd, bytes_count-start, p_value);
}

int _tmain(int argc, _TCHAR* argv[])
{
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/cyphered_msg.txt");
	chi_square_byte_analysis("C:/WINDOWS/system32/MRT.exe");
	chi_square_byte_analysis("C:/WinDDK/7600.16385.1/Debuggers/debugger.chm");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/test.txt.locked");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/email-Johnmen.24@aol.com.ver-CL 1.2.0.0.id-QMCRJYODTJYNDSIXNCRGWLBQGVLZQEUJZOET-16@01@2019 14@15@037221765.randomname-BIEZZTNJEYTNIDYSNICXSMHBWRLGCW.SNI.cbf");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/email-JohnmenII");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/test.txt.lockedbis");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/vipa_big.txt");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/Copie de vipa_big.txt");
	chi_square_byte_analysis("C:/Documents and Settings/Administrateur/Bureau/Copie de vipa_big.txt", 0, 768);
	while(1);
	return 0;
}

