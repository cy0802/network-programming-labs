#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
using namespace std;
// unsigned char -> uint8_t -> 1byte
// unsigned short -> uint16_t -> 2byte
// unsigned int -> uint32_t -> 4byte
// unsigned long long -> uint64_t -> 8byte
unsigned short big_endian(unsigned short&);
unsigned int big_endian(unsigned int&);
bool checksum(unsigned char[], uint16_t, uint16_t);
int main() {
    ifstream file;
    string dict = "";
    vector<pair<uint32_t, string>> data;
    file.open("demo.bin", std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }
    
    // read header info
    char magic[8];
    unsigned int datasize;
    unsigned short n_blocks;
    unsigned short zeros;
    file.read((char *)&magic, sizeof(unsigned long long));
    file.read((char *)&datasize, sizeof(unsigned int));
    file.read((char *)&n_blocks, sizeof(unsigned short));
    file.read((char *)&zeros, sizeof(unsigned short));
    big_endian(datasize);
    big_endian(n_blocks);
    // cout << "header info\n";
    // cout << "magic: " << magic << endl;
    // cout << "datasize: " << big_endian(datasize) << endl;
    // cout << "n_blocks: " << big_endian(n_blocks) << endl;
    // cout << "===========================================================\n\n";

    // read each block
    // cout << "block info\n\n";
    for(int i = 0; i < n_blocks; i++){
        uint32_t offset;        
        uint16_t cksum;         
        uint16_t length;
        unsigned char buffer[5000];
        file.read((char *)&offset, sizeof(uint32_t));
        file.read((char *)&cksum, sizeof(uint16_t));
        file.read((char *)&length, sizeof(uint16_t));
        big_endian(offset);
        big_endian(length);
        // cout << "offset: " << big_endian(offset) << endl;
        // cout << "cksum: " << cksum << endl;
        // cout << "length: " << big_endian(length) << endl;
        bzero(&buffer, sizeof(buffer));
        file.read((char *)&buffer, length * sizeof(unsigned char));
        //cout << "buffer address: " << (void*)buffer << endl;
        if(checksum(buffer, cksum, length)){
            string tmp((char*)buffer, length);
            data.push_back({offset, tmp});
            // cout << "string length: " << tmp.length() << endl;
            // cout << "good block" << endl;
        } else {
            // cout << "corrupt block\n";
        }
        // cout << "current data:\n";
        // for(auto it = data.begin(); it < data.end(); it++) cout << it->first << " " << it->second << "\n";
        // cout << "---------------------------------------------------------\n\n";
    }
    // sort
    sort(data.begin(), data.end());
    for(auto it = data.begin(); it < data.end(); it++){
        dict = dict + it->second;
        // cout << "dict length: " << dict.length() << endl;
        // cout << it->first << " ";
    }
    // cout << "max size of string: " << dict.max_size() << endl;  
    // cout << "==========================================================\n\n";

    // read flag info
    uint16_t offset_array_length;
    string ans = "", tmp;
    stringstream ss;
    // cout << "flag info\n\n";
    file.read((char*)&offset_array_length, sizeof(uint16_t));
    big_endian(offset_array_length);
    // cout << "n_offset: " << big_endian(offset_array_length) << "\n\n";
    // cout << "dict length: " << dict.length() << "\n\n";
    for(int i = 0; i < offset_array_length; i++){
        uint32_t offset;
        file.read((char*)&offset, sizeof(uint32_t));
        big_endian(offset);
        // cout << "offset: " << offset << "\t\t";
        // cout << hex << setw(2) << setfill('0') << (int)(unsigned char)dict[offset] << "\t";
        ss.clear(); tmp.clear();
        ss << hex << setw(2) << setfill('0') << (int)(unsigned char)dict[offset]; ss >> tmp;
        ans = ans + tmp;
        // cout << hex << setw(2) << setfill('0') << (int)(unsigned char)dict[offset + 1] << "\n";
        ss.clear(); tmp.clear();
        ss << hex << setw(2) << setfill('0') << (int)(unsigned char)dict[offset + 1]; ss >> tmp;
        ans = ans + tmp;
        // cout << ans << endl;
    }
    cout << ans;
    // cout << "hex format flag: " << ans << endl << endl;
    file.close();
    return 0;
}

unsigned short big_endian(unsigned short &value){
    value = (value >> 8) | (value << 8);
    return value;
}
unsigned int big_endian(unsigned int &value){
    unsigned int tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    value = (tmp << 16) | (tmp >> 16);
    return value;
}
bool checksum(unsigned char buffer[], uint16_t cksum, uint16_t length){
    // cout << "checksum()\n";
    // cout << "buffer address: " << (void*)&buffer[0] << endl;
    unsigned short *ptr = (unsigned short*)buffer;
    uint16_t result = ptr[0];
    // cout << (void*)&ptr[0] << "\t" << ptr[0] << "\t" << result << "\n";
    int round = length / 2;
    for(int i = 1; i < round; i++){
        result = result ^ ptr[i];
        // cout << (void*)&ptr[i] << "\t" << ptr[i]  << "\t" << result << "\n";
    }
    // cout << "checksum result: " << result << endl;
    if(result == cksum) return true;
    else return false;
}