#include "Analyzer_Crypy.h"
#include "Analyzer_Chi_Squared.h"
#include "../tools/multithreading.h"

/**
 * Intitialize analyzer variables
 */
AnalyzerCrypy::AnalyzerCrypy(char* file_path, Analyzer *ana) : Analyzer(file_path) {
	analyzer = (AnalyzerChiSquared *) ana;
	filenamecheck = false;
}

/**
 * Hidden Anayzer requires : the filename and a Chi2Test on the whole file, we get all of that from constructor.
 */
bool AnalyzerCrypy::analyzer_in_range(long cursor){
	return false;
}

void AnalyzerCrypy::analyzer_process(char* buffer, int readbytes, long cursor){
	// Nothing to do here
}

void AnalyzerCrypy::analyzer_compute(){
	// Name checking
	check_file_name();
}


std::string AnalyzerCrypy::analyzer_result(){
	std::string res(" - Crypy analysis :\n");
	res.append("   - File's name corresponds to the Crpy pattern : ");
	res.append((filenamecheck ? "yes" : "no"));
	res.append("\n");
	return res;
}


void AnalyzerCrypy::check_file_name(){
	regex hidden_name(".*\\.sinta$");

	string name = getFileName(a_file_path);

	if(regex_search(name, hidden_name)){
		filenamecheck = true;
	}
}

bool AnalyzerCrypy::is_ciphered_by_ransomware(){
	return (filenamecheck && analyzer->get_result() == 1);
}


std::string AnalyzerCrypy::get_ransomware_name(){
	return std::string("CryPy");
}