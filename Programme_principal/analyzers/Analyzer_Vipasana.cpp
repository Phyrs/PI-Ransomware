#include "Analyzer_Vipasana.h"
#include "Analyzer_Chi_Squared.h"
#include "../tools/multithreading.h"

/**
 * Intitialize analyzer variables
 */
AnalyzerVipasana::AnalyzerVipasana(char* file_path, long filesize) : Analyzer(file_path) {
	file_size = filesize;
	filenamecheck = false;
	endsigncheck = false;
	firstbytescheck = false;
}

/**
 * Vipasana Anayzer requires : the filename, the last 50.000 bytes (checking the signature) and the first 768 bytes.
 */
bool AnalyzerVipasana::analyzer_in_range(long cursor){
	// If cursor at begenning (768 first bytes)
	if(cursor == 0)
		return true;

	// Require the last 50.000 bytes
	if(file_size > BUFFER_SIZE){
		if(cursor > file_size - 50000 - BUFFER_SIZE)
			return true;
	}

	return false;
}

void AnalyzerVipasana::analyzer_process(char* buffer, int readbytes, long cursor){
	// It's the 768 first bytes
	if(cursor == 0){

		int length = (readbytes < 768) ? readbytes : 768 ;
		startbuffer = (char *) malloc(length * sizeof(char));

		if(startbuffer == NULL){
			printf("Malloc error");
			exit(1);
		}

		memcpy(startbuffer, buffer, length);
	}
	// if 50.000 last bytes.
	int number_of_bytes_to_copy = 0;
	if(file_size <= 50000){
		number_of_bytes_to_copy = readbytes;
		memcpy(endbuffer, buffer, number_of_bytes_to_copy);
	}
	else if(cursor < file_size - 50000 && cursor > file_size - BUFFER_SIZE - 50000){
		number_of_bytes_to_copy = (cursor + readbytes - (file_size - 50000));
		memcpy(endbuffer, buffer+(readbytes-number_of_bytes_to_copy), number_of_bytes_to_copy);
	}
	else if(cursor >= file_size - 50000){
		number_of_bytes_to_copy = readbytes;
		memcpy(endbuffer+(50000-number_of_bytes_to_copy), buffer, number_of_bytes_to_copy);
	}
}

void AnalyzerVipasana::analyzer_compute(){
	// Name checking
	check_file_name();

	// End Signature checking
	check_end_signature();

	// First 768 bytes checking
	check_first_bytes();
}


std::string AnalyzerVipasana::analyzer_result(){
	std::string res(" - Vipasana analysis :\n");
	res.append("   - File's name corresponds to the Vipasana pattern : ");
	res.append((filenamecheck ? "yes" : "no"));
	res.append("\n");
	res.append("   - File's end contains the Vipasana's signature  : ");
	res.append((endsigncheck ? "yes" : "no"));
	res.append("\n");
	res.append("   - The 768 first bytes have been ciphered : ");
	res.append((firstbytescheck ? "yes" : "no"));
	res.append("\n");
	return res;
}


void AnalyzerVipasana::check_file_name(){
	regex vipasana_name("email-Johnmen\\.24@aol\\.com\\.ver-CL.*([0-9]*@)* ([0-9]*@*)*\\.randomname.*\\.cbf");

	string name = getFileName(a_file_path);

	if(regex_search(name, vipasana_name)){
		filenamecheck = true;
	}
}

void AnalyzerVipasana::check_end_signature(){

	// Checking if the end of the file corresponds to the Vipasana Signature with a regex.
	//regex vipasana_sign("(.|\\n|\\r)*\\{ENCRYPTSTART\\}\\{([0-9]*:?)*\\}(\\{(.|\\n|\\r)*\\})\\{BLOCKSSTART\\}(.|\\n|\\r)*\\{BLOCKSEND\\}\\{(.|\\n|\\r)*\\}\\{ENCRYPTENDED\\}");
    regex vipasana_sign("\\{ENCRYPTSTART\\}(.|\\n|\\r)*\\{BLOCKSSTART\\}(.|\\n|\\r)*\\{BLOCKSEND\\}(.|\\n|\\r)*\\{ENCRYPTENDED\\}");

	for(int i = 0; i < 50000; i++){
		if(endbuffer[i]==0)
			endbuffer[i] = 1;
	}

	string end_file(endbuffer);

	if(regex_search(end_file, vipasana_sign)){
		endsigncheck = true;
	}
}

void AnalyzerVipasana::check_first_bytes(){
	AnalyzerChiSquared *analyzer = new AnalyzerChiSquared(a_file_path);

	analyzer->analyzer_process(startbuffer, sizeof(startbuffer), 0);
	analyzer->analyzer_compute();

	if(analyzer->get_result())
		firstbytescheck = true;

	free(startbuffer);
}


bool AnalyzerVipasana::is_ciphered_by_ransomware(){
	return (endsigncheck && firstbytescheck);
}


std::string AnalyzerVipasana::get_ransomware_name(){
	return std::string("Vipasana");
}