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

int f=0;



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


    int mass[100]={-1};
    int pipefd[2];  
    pipe(pipefd);

    pid_t child_pid = fork(); 
    if (child_pid >= 0) {
        active_child_processes += 1;
        if (child_pid == 0)
        {
            int lfd, cfd;
            int nread;
            const size_t kSize = sizeof(struct sockaddr_in);
            char buf1[BUFSIZE],buf2[BUFSIZE];
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

            int k,l;
            int mass[100];
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
                

               // while ((nread = read(cfd, buf, BUFSIZE)) > 0) {  
                
                /*int i=atoi(buf);    
                fp = fopen("server.txt", "a+");
                fprintf(fp, "%d\n", i); 
                printf(" %4d ",i);
                if ((i%10==0) && (i!=0))
                {printf("\n");}
                
                fclose(fp);*/
                read(cfd, buf1, BUFSIZE);
                printf("tcp count:%s\n",buf1);                
                write(pipefd[1],buf1,sizeof(buf1));
                l=atoi(buf1);    
                sleep(1);         
    
                read(pipefd[0],buf2,sizeof(buf2));
                k=atoi(buf2);
                printf("tcp from udp count:%s\n",buf2);                
                printf("total packet:%d,udp packet:%d\n",l,k);
                int mass[k];
                for (int i=0;i<k;i++)
                {
                    read(pipefd[0],buf2,sizeof(buf2));                    
                    mass[i]=atoi(buf2);

                }
                printf("tcp mass:\n");

                for (int i=0;i<k;i++)
                {
                    printf(" %4d ",mass[i]);
                    if ((i%10==0) && i!=0)
                        printf("\n");                        
                }
                printf("\n");
                printf("tcp send numbers of lost packets\n");
                int p=l-k+1;                
                sprintf(buf1,"%d",p);
                write(cfd,buf1,sizeof(buf1));             

                for (int i=0;i<k;i++)
                {
                    if (mass[i]!=i)
                    {
                        sprintf(buf1,"%d",i);
                        //printf("dont receive:%s\n",buf1);
                        write(cfd,buf1,sizeof(buf1));
                    }

                }
                printf("\n");                




                close(pipefd[1]);               

                if (nread == -1) {
                perror("read");
                exit(1);
                }
                printf("tcp closed");
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
            
            read(pipefd[0],mesg,sizeof(mesg));
            printf("udp from pipe %s\n",mesg);
            k=atoi(mesg);
            int h;

            while (1) {
                unsigned int len = SLEN;    

                if ((n = recvfrom(sockfd, mesg, BUFSIZE, 0, (SADDR *)&cliaddr, &len)) < 0) {
                perror("recvfrom");
                exit(1);
                }
                mesg[n] = '\0';         
                mass[l]=atoi(mesg);                 
                                
                l++;
                if(l==70)
                {
                                        
                    sprintf(mesg,"%d",l);
                    write(pipefd[1],mesg,sizeof(mesg));
                    printf("udp try to pipe %s\n",mesg);
                    
                    for (int i=0;i<l;i++)
                    {
                        printf(" %4d ",mass[i]);
                        if ((i%10==0) && i!=0)
                            printf("\n");                        
                    }
                    if (k!=l)
                        {for (int i=0;i<l;i++)   
                            {                        
                                sprintf(mesg,"%d",mass[i]);
                                write(pipefd[1],mesg,sizeof(mesg)); 
                            }                       

                        }  

                    printf("\n");                        

                    int b=k-l+1;        
                    int e[b];             
                    for (int i=0;i<b;i++)
                    {                        
                        if ((n = recvfrom(sockfd, mesg, BUFSIZE, 0, (SADDR *)&cliaddr, &len)) < 0) {
                        perror("recvfrom");
                        exit(1);
                        }
                        mesg[n] = '\0';         
                        e[i]=atoi(mesg);
                        printf("udp losted packet is:%d\n",e[i]);                       

                        
                    }                   


                    printf("\n"); 


                }                                


            
            }            
        } 
    }

    while (active_child_processes > 0) {
    
    int status = -1;
    waitpid(-1, &status, 0);

    active_child_processes -= 1;
  }


}