#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
  #include <sys/types.h>
  #include <sys/wait.h>

#include <pthread.h>
#include <getopt.h>
//#define SERV_PORT 20001
//#define BUFSIZE 1024
#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)
int active_child_processes=0;



int main(int argc, char **argv)  {

int SERV_PORT = -1;
int BUFSIZE = -1;

      while (1) {
        int current_optind = optind ? optind : 1;
    
        static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                          {"serv_port", required_argument, 0, 0},
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



    pid_t child_pid = fork(); 
    if (child_pid >= 0) {
        active_child_processes += 1;
        if (child_pid == 0)
        {
            int lfd, cfd;
            int nread;
            const size_t kSize = sizeof(struct sockaddr_in);
            char buf[BUFSIZE];
            struct sockaddr_in servaddr;
            struct sockaddr_in cliaddr;

            if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket");
                exit(1);
            }

            memset(&servaddr, 0, kSize);
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(SERV_PORT);

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
                /*FILE* fp;      
                fp = fopen("server.txt", "w+");
                fprintf(fp, ""); 
                fclose(fp);*/
                

                while ((nread = read(cfd, buf, BUFSIZE)) > 0) {  
                
                /*int i=atoi(buf);    
                fp = fopen("server.txt", "a+");
                fprintf(fp, "%d\n", i); 
                printf(" %4d ",i);
                if ((i%10==0) && (i!=0))
                {printf("\n");}
                
                fclose(fp);*/
                printf("tcp mess:%s\n",buf);
                }
                

                if (nread == -1) {
                perror("read");
                exit(1);
                }
                close(cfd);
            }                   

        }
        if (child_pid > 0)
        {

            int sockfd, n;
            char mesg[BUFSIZE], ipadr[16];
            struct sockaddr_in servaddr;
            struct sockaddr_in cliaddr;
            if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket problem");
                exit(1);
                }    

            memset(&servaddr, 0, SLEN);
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(SERV_PORT);
            if (bind(sockfd, (SADDR *)&servaddr, SLEN) < 0) {
                perror("bind problem");
                exit(1);
                }
        printf("udp serv started\n");

        int k;   
        
            int l=0;
            char str[BUFSIZE];

        while (1) {
            unsigned int len = SLEN;    

            if ((n = recvfrom(sockfd, mesg, BUFSIZE, 0, (SADDR *)&cliaddr, &len)) < 0) {
            perror("recvfrom");
            exit(1);
            }
            mesg[n] = '\0';  
            printf("udp mess:%s\n",mesg);
            

            /*k=atoi(mesg);    
            if (k<25||k>30)
            {  mass[l]=k;    }  
        l++;
        if(l==50)
            {       
                FILE* fp;
                fp = fopen ("server.txt", "r");
                int i=0;
                printf("tcp file\n"); 
                while(!feof(fp))
                {
                    if (fgets(str, sizeof(str), fp))
                    {                
                        {    massf[i]=atoi(str);
                                printf(" %4d ",massf[i]);
                                if ((i%10==0) && i!=0)
                                    printf("\n"); 
                        }

                    }
                    i++;
                } 
                close(fp);
                
                printf("\n udp socket\n");       

            for (int l=0;l<i;l++)
                {
                printf(" %4d ",mass[l]);
                if ((l%10==0) && l!=0)
                        printf("\n"); 
                }
                printf("\n");
                for (int l=0;l<i;l++)
                {             
                    if(mass[l]!=massf[l])
                    {
                        printf("packet %d lost! \n",massf[l]);
                        mass[l]=massf[l];
                    }       


                }
                printf("\n restore lost packets from tcp socket. Udp packets now:\n");       

            for (int l=0;l<i;l++)
                {
                printf(" %4d ",mass[l]);
                if ((l%10==0) && l!=0)
                        printf("\n"); 
                }
                printf("\n");
            }*/
        
            }            
        } 
    }

    while (active_child_processes > 0) {
    
    int status = -1;
    waitpid(-1, &status, 0);

    active_child_processes -= 1;
  }


}