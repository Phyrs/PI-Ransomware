#ifndef ANALYZER_VIPASANA
#define ANALYZER_VIPASANA
#pragma once

#include "Analyzer.h"
#include <string>
#include <regex>

class AnalyzerVipasana : public Analyzer
{
	public:
	AnalyzerVipasana(char* file_path, long filesize);
	virtual bool analyzer_in_range(long cursor);
	virtual void analyzer_process(char* buffer, int readbytes, long cursor);
	virtual void analyzer_compute();
	virtual void analyzer_result();
	virtual bool is_ciphered_by_ransomware();
	virtual std::string get_ransomware_name();

	private:

	long file_size;
	char endbuffer[50000];
	char *startbuffer;
	bool filenamecheck, endsigncheck, firstbytescheck;

	void check_file_name();
	void check_end_signature();
	void check_first_bytes();
};

#endif