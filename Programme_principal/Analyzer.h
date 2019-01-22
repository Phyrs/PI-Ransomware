#ifndef ANALYZER
#define ANALYZER

#include <string>

class Analyzer
{
	public:

	// Constructor
	Analyzer(char* file_path);

	/**
	 * Tells if the buffer currently pointed by the parser is interesting for the signature's calculation
	 */
	virtual bool analyzer_in_range(long cursor)=0;

	/**
	 * Process current file block
	 */
	virtual void analyzer_process(char* buffer, int readbytes, long cursor)=0;

	/**
	 * Tells if given data sample can be related to the analyzer's purpose.
	 */
	virtual void analyzer_compute()=0;

	/**
	 * Print / save in a log file the result of the scan
	 */
	virtual void analyzer_result() =0;



	protected:
	char* a_file_path;
	std::string getFileName(char *path_char);

};

#endif