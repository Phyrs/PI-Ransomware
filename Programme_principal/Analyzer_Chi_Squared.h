#ifndef ANALYZER_CHI_SQUARED
#define ANALYZER_CHI_SQUARED
#pragma once

#include <boost/math/distributions/chi_squared.hpp>
#include "Analyzer.h"

class AnalyzerChiSquared : public Analyzer
{
	public:
	AnalyzerChiSquared(char* file_path);
	virtual bool analyzer_in_range(long cursor);
	virtual void analyzer_process(char* buffer, int readbytes, long cursor);
	virtual void analyzer_compute();
	virtual void analyzer_result();
	int get_result();

	private:

	double a_bytes_sum, a_bytes_square_sum, a_bytes_count;
	int a_result;
	double a_p_value;

};

#endif