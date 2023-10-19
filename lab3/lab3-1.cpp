#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define MAXLINE 100000
using namespace std;

const char* host = "140.113.213.213";
const int port = 10301;
// int dx = {0, 1, 0, -1};
// int dy = {1, 0, -1, 0};

int main(){
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1){
        cout << "Fail to create a socket.\n";
    } else {
        cout << "sockfd: " << sockfd << endl;
    }

    struct sockaddr_in  server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if(connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        cout << "fail to connect" << endl;
        return 0;
    } else {
        cout << "connect to server successfully" << endl;
    }
    // struct tcp_info info;
    // int len = sizeof(info);
    // getsocketopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);

    char buffer[5000000];
    bzero(&buffer, sizeof(buffer));
    
    int n;
    n = read(sockfd, buffer, MAXLINE);
    // cout << buffer << "\n";
    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, MAXLINE);
    cout << buffer << "\n";

    char map[8][12];

    int cnt = 0;

    for(int i = 0;; i++){
        if(buffer[i] == '#'){
            strncpy(&map[cnt][0], buffer + i, 11);
            // cout << map[cnt] << endl;
            cnt++; i += 11;
        }
        if(cnt >= 7) break;
    }
    pair<int, int> start, dest;
    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 11; j++){
            // cout << map[i][j];
            if(map[i][j] == 'E') dest = {i, j};
            else if(map[i][j] == '*') start = {i, j};
        }
        // cout << endl;
    }
    cout << "start: " << start.first << " " << start.second << "\n";
    cout << "dest: " << dest.first << " " << dest.second << "\n";
    string operation;
    if(start.first < dest.first){ // downward S
        int num = dest.first - start.first;
        while(num--) operation += 'S';
    }
    else if(start.first > dest.first){ // upward W
        int num = start.first - dest.first;
        while(num--) operation += 'W';
    }
    if(start.second > dest.second){ // left A
        int num = start.second - dest.second;
        while(num--) operation += 'A';
    }
    else if(start.second < dest.second){ // right D
        int num = dest.second - start.second;
        while(num--) operation += 'D';
    }
    char buffer2[10];
    bzero(buffer2, sizeof(buffer2));
    operation += '\n';
    // cout << operation << endl;
    // bzero(buffer, sizeof(buffer));
    strncpy(buffer2, operation.c_str(), operation.length());
    cout << buffer2 << endl;
    
    n = write(sockfd, buffer2, sizeof(char) * operation.length());
    // cout << n << endl;
    // cout << buffer2 << endl;

    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, MAXLINE);
    cout << buffer << "\n";

    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, MAXLINE);
    cout << buffer << "\n";
}

