#ifndef PCAP_INVESTIGATOR
#define PCAP_INVESTIGATOR

#include <fstream>
#include <pcap.h>
#include <Windows.h>

// WinPcap requirements:
// Libray: https://www.winpcap.org/devel.htm
// Doc: https://www.winpcap.org/docs/docs_411/html/group__wpcapsamps.html
void switch_bytes(char* target, int target_size);
int is_big_endian();
void packet_analyzer_hidden_tear(const u_char* data);
void pcap_process_hidden_tear(pcap_t* pcap);
void pcap_parse_file(std::string file_path);

#endif