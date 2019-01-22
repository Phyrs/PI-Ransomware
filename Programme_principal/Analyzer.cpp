#include "Analyzer.h"

using namespace std;

// constructor
Analyzer::Analyzer(char* file_path){
	a_file_path= file_path;
}

string Analyzer::getFileName(char *path_char){
	string path(path_char);

	char sep = '/';
	size_t i = path.rfind(sep, path.length());
	if(i != string::npos)
		return path.substr(i+1, path.length() - i);

	return string("");
}