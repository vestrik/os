#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>

//#define SERV_PORT 20001
//#define BUFSIZE 1024
#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)
#define SIZE sizeof(struct sockaddr_in)

int active_child_processes=0;


int main(int argc, char **argv) {
 int BUFSIZE = -1;
 int SERV_PORT = -1;	
 char* ADDR = "1";
 while (true) {
	int current_optind = optind ? optind : 1;
	
	static struct option options[] = {{"bufsize", required_argument, 0, 0},
	                                      {"serv_port", required_argument, 0, 0},
	                                      {"addr", required_argument, 0, 0},
	                                      {0, 0, 0, 0}};
	int option_index = 0;
	int c = getopt_long(argc, argv, "",options, &option_index);
	
	if (c == -1) break;	
	    switch (c) {
	      case 0:
	        switch (option_index) {
	          case 0:
	            BUFSIZE = atoi(optarg);
	            if (BUFSIZE <= 0) {
	                printf("bufsize must be a positive number\n");
	                return 1;
	              }
	            break;
	
	            case 1:
	            SERV_PORT = atoi(optarg);
	            if (SERV_PORT <= 0) {
	                printf("serv_port must be a positive number\n");
	                return 1;
	              }
	            break;
	
	            case 2:
	            ADDR = optarg;
                if (ADDR <= 0) {
	                printf("addr must be an adress\n");
	                return 1;
	              }
                
	            break;
	        }
	
	      case '?':
	        break;
	
	      default:
	        printf("getopt returned character code 0%o?\n", c);
	    }
	
	  }
	  if (BUFSIZE == -1 || SERV_PORT == -1 || ADDR == "1") {
	    printf("Usage: %s --bufsize \"buffer_size\" --serv_port \"serv_port\" --addr \"adress\"\n",
	           argv[0]);
	    return 1;
	}  


    int mass[100];
    for (int i=0;i<=100;i++)
    {
        mass[i]=i;
    }     
    int pipefd[2];  
    pipe(pipefd);

    pid_t child_pid = fork(); 
    if (child_pid >= 0) {
        active_child_processes += 1;
        if (child_pid == 0)
        {
            printf("tcp client started\n");
            int fd;
            int nread;                
            int sockfd, n;
            char sendline[BUFSIZE], recvline[BUFSIZE + 1];  
            struct sockaddr_in cliaddr;
            char buf[BUFSIZE];
            struct sockaddr_in servaddr;


            if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket creating");
                exit(1);
            }

            memset(&servaddr, 0, SIZE);
            servaddr.sin_family = AF_INET;

            if (inet_pton(AF_INET, ADDR, &servaddr.sin_addr) <= 0) {
                perror("bad address");    
                exit(1);
            }

            servaddr.sin_port = htons(SERV_PORT);

            if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
                perror("connect");
                exit(1);
            } 

            
            read(pipefd[0],sendline,sizeof(sendline));


            //printf("%d\n",mass[99]);
            //sprintf(buf,"%d",mass[99]);
            printf("tcp from pipe %s\n",sendline);
            write(fd,sendline,sizeof(sendline)); 
            //strcpy(sendline,"123");
            //sleep(2);         

            read(fd,sendline,sizeof(sendline));
            printf("tcp count of not received packets:%s\n",sendline);
            int q=atoi(sendline);
            write(pipefd[1],sendline,sizeof(sendline)); 
            for (int i=0;i<q;i++)
                {
                    read(fd,sendline,sizeof(sendline));
                    printf("tcp not received packet:%s\n",sendline);
                    write(pipefd[1],sendline,sizeof(sendline)); 

                }                



            close(pipefd[0]);             

            close(fd);
            exit(0);     
            
        }
        if (child_pid > 0)
        {
            printf("udp client started\n");    
            int sockfd, n;
            char sendline[BUFSIZE], recvline[BUFSIZE + 1];
            struct sockaddr_in servaddr;
            struct sockaddr_in cliaddr;

            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(SERV_PORT);

            if (inet_pton(AF_INET, ADDR, &servaddr.sin_addr) < 0) {
                perror("inet_pton problem");
                exit(1);
            }
            if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket problem");
                exit(1);
            } 

            int k=0;
            for (int i=0;i<100;i++)
            {
                if (mass[i]>0)
                {k++;}

            }
            sprintf(sendline,"%d",k);
            
            write(pipefd[1],sendline,sizeof(sendline));

           
            //sprintf(sendline,"%d",mass[98]);
            
            for (int i=0;i<20;i++)
            {
                sprintf(sendline,"%d",mass[i]);
                sendto(sockfd, sendline, BUFSIZE, 0, (SADDR *)&servaddr, SLEN);
            }
            for (int i=50;i<100;i++)
            {
                sprintf(sendline,"%d",mass[i]);
                sendto(sockfd, sendline, BUFSIZE, 0, (SADDR *)&servaddr, SLEN);
            }
            
            read(pipefd[0],sendline,sizeof(sendline));
            int z=atoi(sendline);
            
            for (int i=0;i<z;i++)
            {
                read(pipefd[0],sendline,sizeof(sendline));
                int y=atoi(sendline);
                printf("\tudp not reseived packet:%d\n",y);
                sprintf(sendline,"%d",mass[y]);
                sendto(sockfd, sendline, BUFSIZE, 0, (SADDR *)&servaddr, SLEN);

            }


            close(pipefd[0]);
            close(pipefd[1]);
            close(sockfd);       

        } 
    }

    while (active_child_processes > 0) {
    
    int status = -1;
    waitpid(-1, &status, 0);

    active_child_processes -= 1;
  }



}