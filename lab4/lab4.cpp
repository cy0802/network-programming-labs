#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

// public server
// const char* host = "140.113.213.213";
// const int port = 10314;

// submit
const char* host = "172.21.0.4";
const int port = 10001;
char buffer[20000];
int sockfd = 0;
struct sockaddr_in  server;

// const char* host = 


bool _connect();
void snd(string q);
void rcv();
string encode(string);

int main(){    
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if(!_connect()) return 0;

    string request = "GET /otp?name=111550107 HTTP/1.1\r\n\r\n";
    snd(request);
    rcv(); rcv();
    string otp = buffer;

    if(!_connect()) return 0;

    // request = "GET /verify?otp=" + encode(otp) + " HTTP/1.1\r\n\r\n";
    // snd(request);
    // rcv();rcv();

    // if(!_connect()) return 0;

    request = "POST /upload HTTP/1.1\r\n";
    request += "Host: ";
    request += host;
    request += ":";
    request += to_string(port);
    request += "\r\nConnection: keep-alive\r\n";
    // request += "\r\nContent-Type: application/octet-stream\r\n";
    request += "Content-Length: " + std::to_string(otp.length() + 182) + "\r\n";
    request += "Cache-Control: max-age=0\r\n";
    request += "Upgrade-Insecure-Requests: 1\r\n";
    // request += "Origin: http://inp.zoolab.org:10314\r\n";
    request += "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryNlOjgzJB8fFMOkhq\r\n";
    request += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/118.0.0.0 Safari/537.36\r\n";
    request += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n";
    // request += "Referer: http://inp.zoolab.org:10314/upload\r\n";
    request += "Accept-Encoding: gzip, deflate\r\n";
    request += "Accept-Language: en-US,en;q=0.9,zh-TW;q=0.8,zh;q=0.7,ja;q=0.6\r\n\r\n";
    request += "------WebKitFormBoundaryNlOjgzJB8fFMOkhq\r\n";
    request += "Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n";
    request += "Content-Type: text/plain\r\n\r\n";
    request += otp;
    // request += "\r\n\r\n";
    request += "\r\n------WebKitFormBoundaryNlOjgzJB8fFMOkhq--\r\n";
    snd(request);
    rcv(); rcv();

    return 0;
}

bool _connect(){
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1){
        cout << "Fail to create a socket.\n";
        return false;
    } else {
        cout << "sockfd: " << sockfd << endl;
    }
    if(connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        cout << "fail to connect" << endl;
        return false;
    } else {
        cout << "connect to server successfully" << endl;
    }
    return true;
}

string encode(string raw){
    string encoded = "";
    for(int i = 0; i < raw.length(); i++){
        int tmp = (int)raw[i];
        string ch;
        stringstream ss;
        ss.clear();
        ss << hex << tmp; ss >> ch;
        // cout << ch << endl;
        encoded = encoded + "%" + ch;
    }
    return encoded;
}

void snd(string q){
    // q += '\n';
    bzero(&buffer, sizeof(buffer));
    strncpy(buffer, q.c_str(), q.length());
    cout << "send: =========================\n" << buffer;
    int n = write(sockfd, buffer, q.length());
    cout << n << "\n================================\n\n";
    if(n < 0) cout << "error while sending data\n";
}
void rcv(){
    bzero(&buffer, sizeof(buffer));
    int n = read(sockfd, buffer, sizeof(buffer));
    if(n < 0) cout << "error whild receiving data ==============================================\n\n" << endl;
    else cout << "received: ======================\n" << buffer << "\n================================\n\n";
}