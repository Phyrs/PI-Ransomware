#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
 
using namespace std;
typedef std::vector<std::string> stringvec;

void read_directory(const std::string& name, stringvec& v){
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    stringvec v1;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
            
            if (v[ v.size()-1]!= ".." && v[ v.size()-1]!= ".")
            {
            	cout<<v[ v.size()-1]<<endl;
            	read_directory(name + "\\" + v[v.size()-1],v1);
            }
            
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    } 

}

int main(){
    stringvec v;
    string name_rep;
    printf("Please enter the path to analyse :\n");
    cin >> name_rep;
    read_directory(name_rep, v);
}