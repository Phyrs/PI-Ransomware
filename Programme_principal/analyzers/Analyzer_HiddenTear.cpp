#include "Analyzer_HiddenTear.h"
#include "Analyzer_Chi_Squared.h"
#include "../tools/multithreading.h"

/**
 * Intitialize analyzer variables
 */
AnalyzerHidden::AnalyzerHidden(char* file_path, Analyzer *ana) : Analyzer(file_path) {
	analyzer = (AnalyzerChiSquared *) ana;
	filenamecheck = false;
}

/**
 * Hidden Anayzer requires : the filename and a Chi2Test on the whole file, we get all of that from constructor.
 */
bool AnalyzerHidden::analyzer_in_range(long cursor){
	return false;
}

void AnalyzerHidden::analyzer_process(char* buffer, int readbytes, long cursor){
	// Nothing to do here
}

void AnalyzerHidden::analyzer_compute(){
	// Name checking
	check_file_name();
}


std::string AnalyzerHidden::analyzer_result(){
	std::string res(" - HiddenTear analysis :\n");
	res.append("   - File's name corresponds to the HiddenTear pattern : ");
	res.append((filenamecheck ? "yes" : "no"));
	res.append("\n");
	return res;
}


void AnalyzerHidden::check_file_name(){
	regex hidden_name(".*\\.locked$");

	string name = getFileName(a_file_path);

	if(regex_search(name, hidden_name)){
		filenamecheck = true;
	}
}

bool AnalyzerHidden::is_ciphered_by_ransomware(){
	return (filenamecheck && analyzer->get_result() == 1);
}


std::string AnalyzerHidden::get_ransomware_name(){
	return std::string("HiddenTear");
}