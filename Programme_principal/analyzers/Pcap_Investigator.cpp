#include "Pcap_Investigator.h"
#include <fstream>
#include <pcap.h>
#include <Windows.h>

using namespace std;

int endianness;

void switch_bytes(char* target, int target_size){
	char* buffer= new char[target_size];
	int i;
	for(i= 0; i<target_size; i++)
		buffer[i]= target[i];
	for(i= 0; i<target_size; i++)
		target[target_size-1-i]= buffer[i];
}

/**
 * Checks endianness of current host
 * @return {int}: 1 if big endian, 0 if not
 */
int is_big_endian(){
	int num = 1;
	if(*(char *)&num == 1)
		return 0;
	return 1;
}

/**
 * Analyze given packet and extracts possible encryption password if there is a match
 * @param data {string}: packet to be parsed
 * @param endianness {string}: 1 if byte-order of current host and byte-order of Pcap file match, orhterwise 0
 *                             currently not used
 */
int packet_analyzer_hidden_tear(const u_char* data){
	u_short buff;
	// Network byte-order is big-endian (foe Ethernet, IP and TCp/UDP protocols), i.e. conversion is needed if host is little-endian
	int big_endian= is_big_endian();

	// ----- i: Ethernet Header
	buff= ((u_short*)data)[6];
	if(!big_endian)
		switch_bytes((char*)(&buff), 2);
	if(buff==0x8100){ // 0x8100: 802.1Q Optionnal Header
		data+= 4;
	}
	buff= ((u_short*)data)[6];
	if(!big_endian)
		switch_bytes((char*)(&buff), 2);
	u_short ip_vers= buff;
	switch(buff){
		case 0x800: // ipv4
		case 0x86DD:// ipv6
			data+= 14;
			buff= ((data[0] >> 4) & 0xf);
			switch(buff){
				case 0x4:
					if(ip_vers==0x800 && data[9]==0x6){ // if protocol is TCP
						data+= (data[0] & 0xf) * 4; // Ptr is on TCP Header
					}else{
						return 0;
					}
					break;
				case 0x6:
					if(ip_vers==0x86DD && data[6]==0x6){ // if protocol is TCP
						data+= 40; // Ptr is on TCP Header (ipv6 header has a fixed length of 40 bytes) -> for now, additional headers are not taken in account
					}else{
						return 0;
					}
					break;
				default:
					return 0;
			}
			// ----- iii: TCP Header
			buff= ((u_short*)data)[1]; // TCP dest port
			if(!big_endian)
				switch_bytes((char*)(&buff), 2);
			if(buff== 80 ||
				buff== 3128 ||
				buff== 3132 ||
				buff== 5985 ||
				buff== 8080 ||
				buff== 8088 ||
				buff== 11371 ||
				buff== 1900 ||
				buff== 2869 ||
				buff== 2710){ // List of TCP ports associated by Wireshark to the HTTP protocol (but not always true)
				data+= ((data[12] >> 4) & 0xf) * 4; // Ptr is on (seemingly) HTTP Header
				// ----- iii: HTTP Body
				// The HTTP request of the Hidden-Tear message to the distant server has the following structure:
				// GET [server_url]?files=[computer_name]-[user_name]%20[PASSWORD] HTTP/1.1
				// Aborted: piece of code that compare targetted message with current Machine and User name (cf problem: only work on infected computer)
				if(strncmp((char*)data, "GET ", 4)==0){
					data+= 4;
					data= (u_char*)strstr((char*)data, "?files=");
					if(data!=NULL){
						// i: get length of message
						data+= 7;
						u_char* data_= (u_char*)strstr((char*)data, " HTTP/1.1");
						int urlencoded_len= data_ - data;
						// ii: is there a candidat for an Hidden-Tear password (i.e. string ends with %20 followed by 15 characters, with some potentially url-encoded)
						char* urlencoded= new char[urlencoded_len];
						strncpy(urlencoded, (char*)data, urlencoded_len);
						int i, len_enc= 0, len_dec= 0;
						char previous_char= 0, pprevious_char= 0;
						for(i= urlencoded_len-1; i>=0; i--){
							pprevious_char= previous_char;
							previous_char= data[i+1];
							len_dec++;
							len_enc++;
							if(data[i]==0x25){ // % character
								len_dec-=2;
								if(previous_char==0x32 && pprevious_char==0x30)
									break;
							}
						}
						len_dec-= 1; // last % (of %20) must not be counted
						len_enc-= 3;
						if(len_dec==15){ // 15: password size in Hidden-Tear: candidat found
							char urldecoded[15];
							int cursor= urlencoded_len-len_enc;
							for(i= 0; i<15; i++){
								// Only the special characters !, =, &, ? and / are used in the Hidden-Tear password (* and ! are not a special character)
								if(      urlencoded[cursor]==0x25 && urlencoded[cursor+1]==0x33 && urlencoded[cursor+2]==0x44){ // = (0x3D)
									cursor+=3;
									urldecoded[i]= 0x3D;
								}else if(urlencoded[cursor]==0x25 && urlencoded[cursor+1]==0x32 && urlencoded[cursor+2]==0x36){ // & (0x26)
									cursor+=3;
									urldecoded[i]= 0x26;
								}else if(urlencoded[cursor]==0x25 && urlencoded[cursor+1]==0x33 && urlencoded[cursor+2]==0x46){ // ? (0x3F)
									cursor+=3;
									urldecoded[i]= 0x3F;
								}else if(urlencoded[cursor]==0x25 && urlencoded[cursor+1]==0x32 && urlencoded[cursor+2]==0x46){ // / (0x2F)
									cursor+=3;
									urldecoded[i]= 0x2F;
								}else{
									urldecoded[i]= urlencoded[cursor];
									cursor+=1;
								}
							}
							printf("Candidat found: %.*s\n", 15, urldecoded);
						}
					}
				}
			}
			break;
		default:
			return 0;
	}
}

/**
 * Parse Pcap file, looking for information left by Hidden-Tear ransomware
 * @param pcap {pcap_t}: Pcap file
 * @param endianness {int}: 0 if match between current host and Pcap file's initial host
 */
void pcap_process_hidden_tear(pcap_t* pcap){
	printf("Searching for Hidden Tear password...\n");
	struct bpf_program filter_code;
	// Create filter according to current usage (only keep tcp packets)
	pcap_compile(pcap, &filter_code, "ip and tcp", 1, 0);
	pcap_setfilter(pcap, &filter_code);
 
	// PARSE PACKETS -------------------------------------------------------------------------

    struct pcap_pkthdr* header;
    const u_char* data;
 
    // Parse packets
    while (int returnValue = pcap_next_ex(pcap, &header, &data) >= 0){
		packet_analyzer_hidden_tear(data);
    }
	printf("End of search for Hidden Tear password...\n");
}

/**
 * Parse Pcap file, looking for information
 * @param file_path {string}
 */
void pcap_parse_file(string file_path){
	char errbuff[PCAP_ERRBUF_SIZE];
	// Open Pcap file
    pcap_t* pcap = pcap_open_offline(file_path.c_str(), errbuff);
	// Magic: every Pcap file has the DWORD 0xa1b2c3d4 at its beginning; Pcap files are written by using the endianness of the host computer
	// i.e. if the magic field is not recoginzed as 0xa1b2c3d4: problem!
	char magic[4];
	std::ifstream pcap_file(file_path, std::ios::in | std::ios::binary);
	pcap_file.read(magic, 4);
	pcap_file.close();
	endianness= 1;
	if(*((int*)magic)==0xa1b2c3d4)
		endianness= 0;

	// List here the different pcap parsers
	pcap_process_hidden_tear(pcap);
}