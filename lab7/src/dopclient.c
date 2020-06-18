#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>

//#define SERV_PORT 20001
//#define BUFSIZE 1024
#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)
#define SIZE sizeof(struct sockaddr_in)

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;


typedef struct servArgs {
  int tport;
  int tbuffsize;  
  char* taddr;
} thread_data;

void *servtcp(void *arg)
{
    printf("tcp client started\n");
    thread_data *tdata=(thread_data *)arg; 
    int buff=tdata->tbuffsize;   
    int port=tdata->tport;   
    char* addr = tdata->taddr;
  int fd;
  int nread;
    
  int sockfd, n;
  char sendline[buff], recvline[buff + 1];  
  struct sockaddr_in cliaddr;
  char buf[buff];
  struct sockaddr_in servaddr;


  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, addr, &servaddr.sin_addr) <= 0) {
    perror("bad address");    
    exit(1);
  }

  servaddr.sin_port = htons(port);

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "before open\n", 13);

  FILE* fp;
  fp = fopen ("client.txt", "r");
  write(1, "after open\n", 13);
  	char str[buff];
	int serverCounter = 0;
    while(!feof(fp))
    {
        if (fgets(str, sizeof(str), fp))
        {          
                       
            if (write(fd, str, sizeof(str)) < 0) {
                perror("write");
                exit(1);
            }
        }


    }
    close(fp);
    write(1, "after close file tcp\n", 13);

  close(fd);
  exit(0);

}

void *servudp(void *arg)
{
  printf("udp client started\n");
    thread_data *tdata=(thread_data *)arg; 
    int buff=tdata->tbuffsize;   
    int port=tdata->tport;   
    char* addr = tdata->taddr;
    
  int sockfd, n;
  char sendline[buff], recvline[buff + 1];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if (inet_pton(AF_INET, addr, &servaddr.sin_addr) < 0) {
    perror("inet_pton problem");
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  write(1, "Enter string\n", 13);

  FILE* cfp;
  pthread_mutex_lock(&mut);
  cfp = fopen("client.txt", "w+");
  fprintf(cfp, "");     

  for (int i=1;i<=5;i++)
  {
      
      fprintf(cfp, "%d\n", i); 
	  sprintf(sendline,"%d",i);
      if (sendto(sockfd, sendline, buff, 0, (SADDR *)&servaddr, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }     
      
  }
  fclose(cfp);
  pthread_mutex_unlock(&mut);
   pthread_t pthr;
   struct servArgs argss[1];
    argss[0].tport = port;
    argss[0].tbuffsize = buff;
    argss[0].taddr = addr;

  close(sockfd);

}




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



  int threads_num=2;
  pthread_t pthread1, pthread2;
  struct servArgs args[threads_num];
  for (int i=0;i<threads_num;i++)
  {
      args[i].tport = SERV_PORT;
      args[i].tbuffsize = BUFSIZE;
      args[i].taddr = ADDR;
    }
 
  //if (!strcmp(serv,"udp"))
  {if(pthread_create(&pthread1, NULL, (void*)servudp, (void *)&(args[0])))
      {
          printf("Server: Error: pthread_create failed!\n");
          return 1;
          }
  }
  //if (!strcmp(serv,"tcp"))
  {if(pthread_create(&pthread2, NULL, (void*)servtcp, (void *)&(args[0])))
      {
          printf("Server: Error: pthread_create failed!\n");
          return 1;
          }
  }

  //if (!strcmp(serv,"udp"))
    pthread_join(pthread1, NULL);
   // if (!strcmp(serv,"tcp"))
    pthread_join(pthread2, NULL);
 


}