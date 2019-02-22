#ifndef ANALYZER_HIDDEN
#define ANALYZER_HIDDEN
#pragma once

#include "Analyzer.h"
#include "Analyzer_Chi_Squared.h"
#include <string>
#include <regex>

class AnalyzerHidden : public Analyzer
{
	public:
	AnalyzerHidden(char* file_path, Analyzer *ana);
	virtual bool analyzer_in_range(long cursor);
	virtual void analyzer_process(char* buffer, int readbytes, long cursor);
	virtual void analyzer_compute();
	virtual std::string analyzer_result();
	virtual bool is_ciphered_by_ransomware();
	virtual std::string get_ransomware_name();

	private:

	bool filenamecheck;
	AnalyzerChiSquared* analyzer;

	void check_file_name();
};

#endif