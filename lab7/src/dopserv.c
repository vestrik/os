#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <getopt.h>
//#define SERV_PORT 20001
//#define BUFSIZE 1024
#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)


typedef struct servArgs {
  int tport;
  int tbuffsize;  
} thread_data;

void *servtcp(void *arg)
{
      int lfd, cfd;
  int nread;
  const size_t kSize = sizeof(struct sockaddr_in);
      
    thread_data *tdata=(thread_data *)arg; 
    int buff=tdata->tbuffsize;   
    int port=tdata->tport;   


 char buf[buff];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, kSize);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(lfd, 5) < 0) {
    perror("listen");
    exit(1);
  }
  printf("tcp serv started\n");
  while (1) {
    unsigned int clilen = kSize;

    if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
      perror("accept");
      exit(1);
    }
    printf("connection established\n");
    FILE* fp;      
    fp = fopen("server.txt", "w+");
    fprintf(fp, ""); 

    while ((nread = read(cfd, buf, buff)) > 0) {
      write(1, &buf, nread);

      int i=atoi(buf);    
      fp = fopen("server.txt", "a+");
      fprintf(fp, "%d\n", i); 
      fclose(fp);
    }

    if (nread == -1) {
      perror("read");
      exit(1);
    }
    close(cfd);
  }

}

void *servudp(void *arg)
{
    
    thread_data *tdata=(thread_data *)arg; 
    int buff=tdata->tbuffsize;   
    int port=tdata->tport;   
    int sockfd, n;
    char mesg[buff], ipadr[16];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        exit(1);
        }    

    memset(&servaddr, 0, SLEN);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(sockfd, (SADDR *)&servaddr, SLEN) < 0) {
        perror("bind problem");
        exit(1);
        }
   printf("udp serv started\n");

  while (1) {
    unsigned int len = SLEN;    

    if ((n = recvfrom(sockfd, mesg, buff, 0, (SADDR *)&cliaddr, &len)) < 0) {
      perror("recvfrom");
      exit(1);
    }
    mesg[n] = '\0';  
    

    printf("REQUEST %s      FROM %s : %d\n", mesg,
           inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
           ntohs(cliaddr.sin_port));    

  }
  

}

int main(int argc, char **argv)  {

int SERV_PORT = -1;
int BUFSIZE = -1;
char serv[3];
      while (1) {
        int current_optind = optind ? optind : 1;
    
        static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                          {"serv_port", required_argument, 0, 0},
                                          {"serv_type", required_argument, 0, 0},
                                          {0, 0, 0, 0}};
    
        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index); 
        
        if (c == -1) break;
    
        switch (c) {
          case 0:
            switch (option_index) {
              case 0:
                BUFSIZE = atoi(optarg);
                if (BUFSIZE < 1) { 
                    printf("bufsize must be a positive number\n");
                    return 1;
                }
                break;
              case 1:
                SERV_PORT = atoi(optarg);
                if (SERV_PORT < 0) { 
                    printf("serv_port must be a positive number\n");
                    return 1;
                }
                break;
                case 2:                
                strcpy(serv,optarg);
                if (strlen(serv) != 3) {
	                printf("serv_type must be an type of server - udp or tcp)\n");
	                return 1;
	              }
                
	            break;
              defalut:
                printf("Index %d is out of options\n", option_index);
            }
            break;
          case '?':
            break; 
            
          default:
            printf("getopt returned character code 0%o?\n", c);
        }
      }
    
      if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
      }
    
      if (BUFSIZE == -1 || SERV_PORT == -1) {
        printf("Usage: %s --bufsize 100 --serv_port 10050 \n", argv[0]);
        return 1;
      }  

  int threads_num=2;
  pthread_t pthread1, pthread2;
  struct servArgs args[threads_num];
  for (int i=0;i<threads_num;i++)
  {
      args[i].tport = SERV_PORT;
      args[i].tbuffsize = BUFSIZE;
    }
    
  if (!strcmp(serv,"udp"))
  {if(pthread_create(&pthread1, NULL, (void*)servudp, (void *)&(args[0])))
      {
          printf("Server: Error: pthread_create failed!\n");
          return 1;
          } 
  }
  if (!strcmp(serv,"tcp"))
  {if(pthread_create(&pthread2, NULL, (void*)servtcp, (void *)&(args[1])))
      {
          printf("Server: Error: pthread_create failed!\n");
          return 1;
          }  
  }  
   
     if (!strcmp(serv,"udp")){pthread_join(pthread1, NULL);}
     if (!strcmp(serv,"tcp")){pthread_join(pthread2, NULL); }
     printf("thread ended\n"); 

  






}