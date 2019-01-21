// That program tries to determine if a file has been ciphered by Vipasana
//

#include "Vipasana_Signature.h"


string getFileName(char *path_char){
	string path(path_char);

	char sep = '/';

	size_t i = path.rfind(sep, path.length());
	if(i != string::npos)
		return path.substr(i+1, path.length() - i);

	return "";
}

ifstream loadFile(char *path){
	ifstream file;
	file.open(path, ios::binary);

	if (!file) {
		cerr << "Error while opening file, please check the name...";
		cin >> path;
		exit(1);   // call system to stop
	} else {
		return file;
	}
}

long getFileLength(ifstream &file){
	file.seekg(0, file.end);
	long length = file.tellg();
	file.clear();
	file.seekg(0, file.beg);
	return length;
}


long get_beginvipasana_signblock_position(ifstream &file, bool hugefile, long length){
	long count = 0;
	if(hugefile){
		// Starting at 50.000 from the end if file is > 100 Ko (optimization)
		file.clear();
		file.seekg(length - 50000, file.beg);
		count = length - 50000;
	} else {
		file.clear();
		file.seekg(0, file.beg);
	}
	string line = "";
	string search = "{ENCRYPTSTART}";
	size_t pos;
	bool find = false;
	while(file.good()){
		count += line.length();
		getline(file, line);
		pos = line.find(search);
		if(pos != string::npos){
			find = true;
			count += pos;
			break;
		}
	}
	if(find)
		return count;
	else
		return -1;
}


bool check_sign_end_of_file(ifstream &file){
	
	// Searching for the beginning position of the Vipasana signature : {ENCRYPTSTART}
	long file_length = getFileLength(file);

	long sign_block_position;
	if(file_length > 100000)
		sign_block_position = get_beginvipasana_signblock_position(file, true, file_length);
	else
		sign_block_position = get_beginvipasana_signblock_position(file, false, file_length);

	if(sign_block_position == -1){
		return false;
	}

	// Checking if the end of the file corresponds to the Vipasana Signature with a regex.
	//regex vipasana_sign("(.|\\n|\\r)*\\{ENCRYPTSTART\\}\\{([0-9]*:?)*\\}(\\{(.|\\n|\\r)*\\})\\{BLOCKSSTART\\}(.|\\n|\\r)*\\{BLOCKSEND\\}\\{(.|\\n|\\r)*\\}\\{ENCRYPTENDED\\}");
    regex vipasana_sign("\\{ENCRYPTSTART\\}(.|\\n|\\r)*\\{BLOCKSSTART\\}(.|\\n|\\r)*\\{BLOCKSEND\\}(.|\\n|\\r)*\\{ENCRYPTENDED\\}");

	char *buffer;
	long buffer_length = file_length - sign_block_position + 1 ;

	buffer = (char *) malloc(buffer_length * sizeof(char));

	if(buffer == NULL){
		printf("Malloc error\n");
		exit(1);
	}
	file.clear();
	file.seekg(sign_block_position, file.beg);

	char c[1];
	int i = 0;
	while(file.good()){
		file.read(c, 1);
		if(c[0] == 0)
			c[0] = 1;
		buffer[i] = c[0];
		i++;
	}

	string end_file(buffer);

	bool res = false;
	if(regex_search(end_file, vipasana_sign)){
		res = true;
	}

	free(buffer);
	return res;
}

bool check_filename_correspondance(string filename){
	  regex vipasana_name("email-Johnmen\\.24@aol\\.com\\.ver-CL.*([0-9]*@)* ([0-9]*@*)*\\.randomname.*\\.cbf");

	bool res = false;
	if(regex_search(filename, vipasana_name)){
		res = true;
	}

	return res;
}

void print_report(string filename, bool eof_correspondance, bool filename_correspondance){
	printf("------------------------------------------\n");
	printf("         REPORT of %s analysis            \n", filename.c_str());
	printf("------------------------------------------\n");
	printf("\n");
	printf(" - End of file corresponds to the Vipasana's typical EndOfFile : %s\n", (eof_correspondance ? "yes" : "no"));
	printf(" - Filename corresponds to the Vipasana's typical filename : %s\n", (filename_correspondance ? "yes" : "no"));
}


/**
* The argument is a path to a file that has been supposedly encrypted with vipasana.
*/
void scan_vipasana(char* file_path)
{
	string filename = getFileName(file_path);

	ifstream file = loadFile(file_path);

	// Checking for the end of the file.
	bool end_of_file_correspondance = check_sign_end_of_file(file);

	// Checking for the filename structure..
	bool filename_correspondance = check_filename_correspondance(filename);

	file.close();

	print_report(filename, end_of_file_correspondance, filename_correspondance);

	//cin >> argc;
	//return 0;
}

