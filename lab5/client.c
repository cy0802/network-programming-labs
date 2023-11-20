#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#define MAX 30000
#define MAXLINE 60000
const int port = 10495;
int main(int argc, char *argv[]) {
	int sockfd; 
    struct sockaddr_in servaddr; 
   
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
   
    memset(&servaddr, 0, sizeof(servaddr)); 
       
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    socklen_t len = sizeof(servaddr);

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("connection"); 
        exit(EXIT_FAILURE); 
    } else {
		// printf("connect to server successfully\n");
    }
       
    int n;
    
	// send packet for measuring latency
	struct timeval _time;
	struct timezone _zone;
    int cnt = MAX;
    char sendData[MAXLINE];

    // packet content
    // sec
    // usec
    // aaaaaaaaaaaaaaa......
    while(cnt--){
        int err = gettimeofday(&_time, &_zone);
        bzero(sendData, sizeof(sendData));
        sprintf(sendData, "%lld\n%lld\n", (long long int)_time.tv_sec, (long long int)_time.tv_usec);
        int l = strlen(sendData);
        
        if(cnt == MAX-1) sendData[l] = 's'; // start
        else if(cnt == 0) sendData[l] = 'e'; // end
        else sendData[l] = 'm';
        
        for(int i = l+1; i < MAXLINE-1 && cnt != MAX-1; i++) sendData[i] = 'a';
        if(cnt == MAX - 1) sendData[strlen(sendData)] = '\0';
        else sendData[MAXLINE-1] = '\0';
        if(sendto(sockfd, sendData, sizeof(sendData), 0, (struct sockaddr *) &servaddr, len) == -1){
            perror("sending");
            exit(EXIT_FAILURE);
        } else {
            // if(cnt % 1000 == 0) printf("%d\n", cnt);
            // printf("%d\n", cnt);
            // printf("%s\nsent\n", sendData);
        }
    }

    bzero(sendData, sizeof(sendData));
    n = recvfrom(sockfd, sendData, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
    sendData[n] = '\0'; 
    printf("%s", sendData); 

    close(sockfd);
	return 0;
}
