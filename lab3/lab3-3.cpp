#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define inf 1000
using namespace std;

const char* host = "140.113.213.213";
const int port = 10303;

char map[110][110];
char buffer[60000];
int sockfd = 0;
string ans;
// right, left, up, down
int dr[4] = {0, 0, -1, 1}; 
int dc[4] = {1, -1, 0, 0};

bool found = false;
void dfs(int row, int col, string route);
void query(string q);
void rcv(bool discard);
int _match(char*);

int main(){
    // socket connection
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

    rcv(false); rcv(false);
    // move view port to top
    int space = 0;
    string q;
    for(int i = 0; i < sizeof(buffer) / sizeof(char); i++){
        int row;
        if((row = _match(&buffer[i])) != -inf){
            // cout << row << endl;
            // int tmp = 2;
            while(buffer[i + 3 + space] == ' '){
                space++;
            }
            if(row >= 0) q += string(row, 'I');
            else         q += string(-row, 'K');
            if(space)    q += string(space, 'L');
            query(q); rcv(false); rcv(true);
            break;
        }
    }
    // move view port to left
    for(int i = 0; i < sizeof(buffer) / sizeof(char) && !space; i++){
        if(_match(&buffer[i]) != -inf){
            if(buffer[i+3] != ' '){
                q = "J";
                query(q); rcv(false); rcv(true);
                i = 0; // data in the buffer is renewed
            }
        }
    }
    if(!space){
        q = "L";
        query(q); rcv(false); rcv(true);
    }

    // map:      101 * 101
    // viewport: 7 * 11
    // scan map
    for(int row = 0; row < 101; row += 7){
        for(int col = 0; col < 101; col += 11){
            int cnt = 0; 
            // cout << "buffer\n" << buffer << "\n";
            for(int i = 0; i < sizeof(buffer) / sizeof(char); i++){
                // cout << i << ' ' << sizeof(buffer) / sizeof(char) << "\n";
                if(_match(&buffer[i]) != -inf){
                    strncpy(&map[row+cnt][col], &buffer[i+3], 11);
                    cnt++; i+=11;
                    // cout << "\nmap: =========================================================================\n";
                    // for(int r = 0; r < 101; r++){
                    //     for(int c = 0; c < 101 + 11; c++ ){
                    //         cout << map[r][c];
                    //     }
                    //     cout << endl;
                    // }
                    // cout << "==============================================================================\n\n";
                    // char tmp; cin >> tmp;
                }
                if(cnt >= 7)    break;
            }
            // move right
            q = string(11, 'L');
            query(q); rcv(false); rcv(true); 
        }
        // move down and left
        q = string(7, 'K') + string(99+11, 'J');
        query(q); rcv(false); rcv(true);
    }
    // pair<int, int> start;
    // cout << "complete map ====================================================================\n";
    for(int row = 0; row < 101; row++){
        for(int col = 0; col < 101; col++){
            // cout << map[row][col];
            if(map[row][col] == '*'){
                dfs(row, col, "");
                query(ans); rcv(false); rcv(false);
            } 
        }
        // cout << endl;
    }
    // cout << "=================================================================================\n\n";

}

void query(string q){
    q += '\n';
    bzero(&buffer, sizeof(buffer));
    strncpy(buffer, q.c_str(), q.length());
    cout << "send: =========================\n" << buffer;
    int n = write(sockfd, buffer, q.length());
    cout << n << "\n================================\n\n";
    if(n < 0) cout << "error while sending data\n";
}
void rcv(bool discard){
    if(discard){
        char tmp[1000];
        bzero(&tmp, sizeof(tmp));
        int n = read(sockfd, tmp, sizeof(tmp));
        cout << "received: ======================\n" << "discarded\n" << tmp << "\n================================\n\n";
        return;
    }
    bzero(&buffer, sizeof(buffer));
    int n = read(sockfd, buffer, sizeof(buffer));
    cout << "received: ======================\n" << buffer << "\n================================\n\n";
}
int _match(char* ch){
    if(*ch >= '0' && *ch <= '9' && *(ch+1) == ':'){
        if(*(ch-1) == 'e') return -inf;
        int result = 0;
        result += (*ch) - '0';
        if(*(ch-1) == '-') result = -result;
        if(*(ch-1) >= '0' && *(ch-1) <= '9') result += 10 * ((*(ch-1) - '0'));
        if(*(ch-2) >= '0' && *(ch-2) <= '9') result += 100 * ((*(ch-2) - '0'));
        return result;
    } else {
        return -inf;
    }
}

// 'W' walk upward, 'S' walk downward, 'A' walk left, 'D' walk right.
// Move view port:
// 'I' move upward, 'K' move downward, 'J' move left, 'L' move right.
// 'R' reset view port to current position.


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
