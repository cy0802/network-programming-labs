#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

const char* host = "140.113.213.213";
const int port = 10302;

const int width = 79;
const int height = 21;

char map[22][80];
string ans;
// right, left, up, down
int dr[4] = {0, 0, -1, 1}; 
int dc[4] = {1, -1, 0, 0};

bool found = false;
void dfs(int row, int col, string route);
void print();

int main(){
    // socket connection
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1)   cout << "Fail to create a socket.\n";
    else                cout << "sockfd: " << sockfd << endl;
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
    
    // read messages
    char buffer[20000];
    int n;
    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, sizeof(buffer));
    cout << buffer << "\n\n\n";
    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, 170);
    cout << buffer << "\n\n\n";
    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, sizeof(buffer));
    if(n < 0) cout << "error while receiving messages from server\n";
    cout << buffer << "\n";

    // generate map
    int cnt = 0;
    for(int i = 0; i < sizeof(buffer) / sizeof(char); i++){
        if(buffer[i] == '#'){
            strncpy(map[cnt], buffer+i, width);
            i += width;
            cnt++;
        }
        if(cnt >= 21) break;
    }
    
    // search for E
    for(int r = 0; r < height; r++){
        for(int c = 0; c < width; c++){
            if(map[r][c] == '*'){
                cout << "start" << r << " " << c << endl;
                dfs(r, c, "");
                break;
            }
        }
    }

    // send answer
    cout << "answer: " << ans << endl;
    ans += '\n';
    bzero(&buffer, sizeof(buffer));
    strncpy(buffer, ans.c_str(), ans.length());
    n = write(sockfd, buffer, sizeof(char) * ans.length());
    if(n < 0) cout << "error while sending answer\n";
    
    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, sizeof(buffer));
    cout << buffer << "\n\n\n";

    bzero(&buffer, sizeof(buffer));
    n = read(sockfd, buffer, sizeof(buffer));
    cout << buffer << "\n\n\n";
}

void dfs(int row, int col, string route){
    // cout << "<" << row << " " << col << "\t" << route << ">" << endl;
    // print();
    if(found) return;
    if(map[row][col] == 'E'){
        ans = route;
        found = true;
        return;
    } else if(map[row][col] == '#' || map[row][col] == '$'){
        return;
    }
    map[row][col] = '$'; // visited
    // right, left, up, down
    dfs(row + dr[0], col + dc[0], route + 'D');
    dfs(row + dr[1], col + dc[1], route + 'A');
    dfs(row + dr[2], col + dc[2], route + 'W');
    dfs(row + dr[3], col + dc[3], route + 'S');
    
}

void print(){
    for(int r = 0; r < 21; r++){
        for(int c = 0; c < 79; c++){
            cout << map[r][c];
        }
        cout << endl;
    }
    cout << endl;
}