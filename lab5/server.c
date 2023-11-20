#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#define MAX 30000
#define MAXLINE 60000
const int port = 10495;

int main(int argc, char *argv[]) {
	int sockfd; 
    char buffer[MAXLINE]; 
    // const char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 
    struct timeval timeout = {2, 0}; //set timeout for 2 seconds

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
       
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
       
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port); 
       
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    socklen_t len;

    while(1){
        int n; 
        int flag = 1;
        long long int startSec = 0;
        long long int endSec = 0;
        double delay = 0;
        long long int totalSize = 0;
        long long int servSec, servUsec, cliSec, cliUsec;
        int cnt = MAX;

        while(cnt--){
            len = sizeof(cliaddr);
            bzero(buffer, sizeof(buffer));
            n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&cliaddr, &len);
            if(n < 0) break;
            if(flag) printf("%s\n", buffer);
            totalSize += n;
            buffer[n] = '\0';
            // printf("Receive %d char from client\n", n, buffer);
            // printf("%s\n", p);
            
            char* _cliSec = strtok(buffer, "\n");
            char* _cliUsec = strtok(NULL, "\n");
            char* payload = strtok(NULL, "\n");
            char *eptr;
            cliSec = strtoll(_cliSec, &eptr, 10);
            cliUsec = strtoll(_cliUsec, &eptr, 10);
            // printf("client time: %lld %lld\n", cliSec, cliUsec);
            
            struct timeval _time;
            struct timezone _zone;
            int err = gettimeofday(&_time, &_zone);
            servSec = (long long int)_time.tv_sec;
            servUsec = (long long int)_time.tv_usec;
            if(servUsec < cliUsec){
                servSec--; servUsec += 1e6;
            }
            if(flag){
                startSec = cliSec;
                delay = (servSec - cliSec + (servUsec - cliUsec) / 1000000.0) * 1000.0;
                flag = 0;
            }
            // if(cnt % 1000 == 0) printf("%d ", cnt);
            // printf("%c", payload[0]);
            if(payload[0] == 'e') break;
            
        }
        endSec = servSec;
        double bandwidth = ((8.0 * totalSize / 1.0 / (endSec - startSec)) / 1000000.0);
        printf("\n# RESULTS: delay = %.4lf ms, bandwidth = %.4lf Mbps\n", delay, bandwidth);
        
        // send result back
        if((delay - 0) > 0.00001){
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, "# RESULTS: delay = %.4lf ms, bandwidth = %.4lf Mbps\n", delay, bandwidth);
            if(sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) == -1){
                perror("sending");
                exit(EXIT_FAILURE);
            } else {
                printf("send result back to client\n");
                // if(cnt % 1000 == 0) printf("%d\n", cnt);
                // printf("%d\n", cnt);
                // printf("%s\nsent\n", sendData);
            }
        }
    }

  	
   
	// while(cnt--){
    //     // printf("%d ", cnt);
		
    //     // if(cnt % 1000 == 0){
    //     //     printf("\nserver time: %lld %lld\n", (long long int)_time.tv_sec, (long long int)_time.tv_usec);
    //     //     printf("latency: %llds %lldµs\n", servSec - cliSec, servUsec - cliUsec);
    //     //     printf("============================================================\n");
    //     // }
    //     if(cnt == 0){
    //         endSec = servSec;
    //         printf("totalSize: %lld\tstartSec: %lld\tendSec: %lld\n", totalSize, startSec, endSec);
    //         printf("throughput: %lf bits/sec\n", 8.0 * totalSize / 1.0 / (endSec - startSec));
    //         fflush(stdout);
    //         totalSize = 0;
    //         endSec = startSec = 0;
    //         flag = 1;
    //         cnt = MAX;
    //     }
	// }
    

    // len = sizeof(cliaddr);  //len is value/result 
   
    // n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
    //             MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
    //             &len); 
    // buffer[n] = '\0'; 
    // printf("Client : %s\n", buffer); 
    // sendto(sockfd, (const char *)hello, strlen(hello),  
    //     MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
    //         len); }
	return 0;
}
