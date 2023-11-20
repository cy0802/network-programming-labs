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
const int port = 10304;

char map[110][110];
char buffer[80000];
int sockfd = 0;
string ans;
bool found = false;
// right, left, up, down
int dr[4] = {0, 0, -1, 1}; 
int dc[4] = {1, -1, 0, 0};

void query(string q);
void rcv(bool discard);
int _match(char*);
void dfs(int _from, string route);
void updateMap();

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
    updateMap();
    dfs(-1, "");
    cout << ans << endl;

    
}

void updateMap(){
    int cnt = 0;
    for(int i = 0; i < sizeof(buffer) / sizeof(char); i++){
        // cout << i << ' ' << sizeof(buffer) / sizeof(char) << "\n";
        if(_match(&buffer[i]) != -inf){
            strncpy(&map[cnt][0], &buffer[i+3], 11);
            cnt++; i+=11;
        }
        if(cnt >= 7)    break;
    }
    // for(int i = 0; i < 7; i++) cout << map[i] << endl;
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
    if(n < 0) cout << "error when receiving data ==============================================\n\n" << endl;
    else cout << "received: ======================\n" << buffer << "\n================================\n\n";
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

// 'W' walk upward, 'S' walk downward, 
// 'A' walk left, 'D' walk right.

void dfs(int _from, string route){ // right, left, up, down
    if(found) return;
    cout << route << "\n\n";
    int fromDirec[4] = {1, 0, 3, 2};
    int direction[4][2] = {{3, 6}, {3, 4}, {2, 5}, {4, 5}};
    string go[4] = {"D", "A", "W", "S"};
    string back[4] = {"A", "D", "S", "W"};
    for(int i = 0; i < 4; i++){
        if(i == _from) continue;
        if(map[direction[i][0]][direction[i][1]] == 'E'){
            found = true;
            query(go[i]); rcv(false); rcv(false); rcv(false); rcv(false); rcv(false);
            int tmp; cin >> tmp;
            return;
        }
        if(map[direction[i][0]][direction[i][1]] != '#'){
            query(go[i]); rcv(false); rcv(true);
            updateMap();
            dfs(fromDirec[i], route + go[i]);
            query(back[i]); rcv(false); rcv(true);
            updateMap();
        }
    }
    
} // current position {3, 5}

