#ifndef ANALYZER_CHI_SQUARED
#define ANALYZER_CHI_SQUARED
#pragma once

#include <boost/math/distributions/chi_squared.hpp>

/**
 * Intitialize analyzer variables
 */
void analyzer_init(char* file_path);

/**
 * Tells if the buffer currently pointed by the parser is interresing for the signature's calculation
 */
int analyzer_in_range(int cursor);

/**
 * Process current file block
 */
void analyzer_process(char* buffer, int file_count);

/**
 * Tells if given data sample can be related to specified multinomial distribution, within given error margin
 */
void analyzer_compute();

/**
 * Print / save in a log file the result of the scan
 */
void analyzer_result();

#endif