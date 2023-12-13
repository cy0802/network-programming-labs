#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <utility>
#define errquit(m) { perror(m); exit(-1); }
#define N 30
#define MAXLINE 5000
using namespace std;

void nQueen(int n);
void printVisitedStatus();
void printMap();
char socketPath[] = "/queen.sock";

char buffer[MAXLINE];
char map_[N][N];
char result[N][N];
bool endFlag = false;
bool visitedRow[N] = {0};
bool visitedCol[N] = {0};
bool visitedDiagonal[2*N-1] = {0}; // map[row][col] => diagonal = col - row + 29
bool visitedDiagonal2[2*N-1] = {0}; // map[row][col] => diagonal2 = row + col
vector<pair<int, int>> queens, resultQueens;

int sockfd;
struct sockaddr_un cliaddr, servaddr;

int main(){
    if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {errquit("socket");}
    else cout << "create socket sucessfully\n";
    bzero(&cliaddr, sizeof(cliaddr));
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strncpy(servaddr.sun_path, socketPath, sizeof(socketPath) - 1);

    if((connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {errquit("connect");}
    else cout << "connect to server\n";

    bzero(&buffer, sizeof(buffer));
    sprintf(buffer, "S");
    if((send(sockfd, buffer, strlen(buffer), 0)) < 0) errquit("send");
    bzero(&buffer, sizeof(buffer));
    if((recv(sockfd, buffer, MAXLINE, 0)) < 0) errquit("recv");
    memcpy(map_, buffer + 4, 30*30);

    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            if(map_[row][col] == 'Q'){
                visitedCol[col] = true;
                visitedRow[row] = true;
                visitedDiagonal[col - row + N - 1] = true;
                visitedDiagonal2[row + col] = true;
            }
        }
    }

    nQueen(27);
    cout << "===================================================================\n";
    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            cout << result[row][col] << " ";
        }
        cout << endl;
    }
    cout << "===================================================================\n";
    
    for(auto it = resultQueens.begin(); it < resultQueens.end(); it++){
        bzero(&buffer, sizeof(buffer));
        sprintf(buffer, "M %d %d", it->first, it->second);
        if(send(sockfd, buffer, sizeof(buffer), 0) < 0) errquit("send");
        bzero(&buffer, sizeof(buffer));
        if(recv(sockfd, buffer, sizeof(buffer), 0) < 0) errquit("recv");
    }
    bzero(&buffer, sizeof(buffer));
    sprintf(buffer, "C");
    if(send(sockfd, buffer, sizeof(buffer), 0) < 0) errquit("send");
    bzero(&buffer, sizeof(buffer));
    if(recv(sockfd, buffer, sizeof(buffer), 0) < 0) errquit("recv");
    cout << buffer << endl;
    
    close(sockfd);
    return 0;
}

void nQueen(int n){
    // printMap();
    if(n == 0){
        memcpy(result, map_, sizeof(map_));
        resultQueens = queens;
        // printMap();
        endFlag = true;
        return;
    }
    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            if(endFlag) return;
            if(visitedRow[row]) break;
            if(visitedCol[col]) continue;
            if(visitedDiagonal[col - row + N - 1]) continue;
            if(visitedDiagonal2[row + col]) continue;
            
            map_[row][col] = 'Q';
            queens.push_back({row, col});
            visitedRow[row] = true; visitedCol[col] = true;
            visitedDiagonal[col - row + N - 1] = true;
            visitedDiagonal2[row + col] = true;
            nQueen(n-1);
            map_[row][col] = '.';
            queens.pop_back();
            visitedRow[row] = false; visitedCol[col] = false;
            visitedDiagonal[col - row + N - 1] = false;
            visitedDiagonal2[row + col] = false;
        }
        if(!visitedRow[row]) return;
    }
}
void printVisitedStatus(){
    cout << "===========================================\n";
    cout << "row: ";
    for(int i = 0; i < N; i++) cout << visitedRow[i] << " ";
    cout << "\ncol: ";
    for(int i = 0; i < N; i++) cout << visitedCol[i] << " ";
    cout << "\ndiagonal 1: ";
    for(int i = 0; i < 2*N-1; i++) cout << visitedDiagonal[i] << " ";
    cout << "\ndiagonal 2: ";
    for(int i = 0; i < 2*N-1; i++) cout << visitedDiagonal2[i] << " ";
    cout << "\n===========================================";
}
void printMap(){
    cout << "===========================================\n";
    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            cout << map_[row][col] << " ";
        }
        cout << "\n";
    }
    cout << "===========================================\n";
}
