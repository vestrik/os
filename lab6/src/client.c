#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>
#include "multmodulo.h"

uint64_t result = 1;
uint64_t fact = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct Server {
  char ip[255];
  int port;
};

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10); //str to unsigned long integer
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

struct TaskToServer
{
	struct Server server;
	uint64_t begin;
	uint64_t end;
	uint64_t mod;
};


void sendTaskToServer(void * args)
{
		    struct TaskToServer * task = (struct TaskToServer *)args;
		    printf("Client: sending data to server begin = %lu end = %lu mod = %lu\n", task->begin, task->end, task->mod);
			struct hostent *hostname = gethostbyname((task->server).ip); //получаем имя машины
			if (hostname == NULL)
			{
				fprintf(stderr, "gethostbyname failed with %s\n", task->server.ip);
				exit(1);
			}
            //Заполняем структуру адреса, на котором будет работать сервер
			struct sockaddr_in server;
			server.sin_family = AF_INET; //ip
			server.sin_port = htons((task->server).port); //port
			server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr); //сетевой интерфейс

			int sck = socket(AF_INET, SOCK_STREAM, 0); //создаем сокет, AF_INET для сетевого протокола IPv4
			if (sck < 0)
			{
				fprintf(stderr, "Client: Socket creation failed!\n");
				exit(1);
			}
            //printf("Client: socket created\n");
			if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) //Устанавливаем соединение с сервером
			{
				fprintf(stderr, "Client: Connection failed\n");
				exit(1);
			}
            //printf("connected to server\n");
			char charTask[sizeof(uint64_t) * 3];
			memcpy(charTask, &(task->begin), sizeof(uint64_t));
			memcpy(charTask + sizeof(uint64_t), &(task->end), sizeof(uint64_t));
			memcpy(charTask + 2 * sizeof(uint64_t), &(task->mod), sizeof(uint64_t));

			if (send(sck, charTask, sizeof(charTask), 0) < 0)
			{
				fprintf(stderr, "Client: Send failed\n");
				exit(1);
			}

			char response[sizeof(uint64_t)];
			if (recv(sck, response, sizeof(response), 0) < 0)
			{
				fprintf(stderr, "Client: Recieve failed\n");
				exit(1);
			}   
			uint64_t answer1 = 0;
			memcpy(&answer1, response, sizeof(uint64_t));

            if (recv(sck, response, sizeof(response), 0) < 0)
			{
				fprintf(stderr, "Client: Recieve failed\n");
				exit(1);
			}        
			uint64_t answer2 = 0;
			memcpy(&answer2, response, sizeof(uint64_t));
            
			printf("Client thread: Answer: mod %lu fac %lu\n", answer1,answer2);
			
			close(sck);
			pthread_mutex_lock(&mut);   
            fact*=answer2;
            result=MultModulo(result, answer1,task->mod);            
			pthread_mutex_unlock(&mut);           
			return;
}

struct Server* parseFile(char* fileName)
{
	char fullName[PATH_MAX] = { '\0' };
	strcpy(fullName, get_current_dir_name());
	strcat(fullName, "/");
	strcat(fullName, fileName);
	FILE * file = fopen(fullName, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Client: File open error\n");
		return NULL;
	}
	char ipPort[255];
	int serverCounter = 0;
	while (fgets(ipPort, sizeof(ipPort), file) != NULL)
	{
		serverCounter++;
	}
	fseek(file, 0, SEEK_SET); //указатель в начало файла
	struct Server *to = malloc(sizeof(struct Server) * (serverCounter+1));
	int j = 0;
	while (fgets(ipPort, sizeof(ipPort), file) != NULL)
	{
		int i = 0;
		while ((ipPort[i] != ':')&&(ipPort[i] != '\0'))
		{
			to[j].ip[i] = ipPort[i];
			i++;
		}
		to[j].port = atoi(&ipPort[i + 1]);
		printf(to[j].ip);
		printf(":");
		printf("%d", to[j].port);
		printf("\n");
		j++;
	}
	to[serverCounter].ip[0] = '\0';
	to[serverCounter].port = 0;
	fclose(file);
	return to;
}

int getServerNum(struct Server* s)
{
	int i = 0;
	while (strlen(s[i].ip))
	{
		i++;
	}
	return i;
}

int main(int argc, char **argv)
{
	uint64_t k = -1;
	uint64_t mod = -1;
	char servers[255] = { '\0' }; ////

	while (true)
	{
		int current_optind = optind ? optind : 1;

		static struct option options[] = { { "k", required_argument, 0, 0 },
		{ "mod", required_argument, 0, 0 },
		{ "servers", required_argument, 0, 0 },
		{ 0, 0, 0, 0 } };

		int option_index = 0;
		int c = getopt_long(argc, argv, "", options, &option_index);

		if (c == -1)
			break;

		switch (c)
		{
		case 0:
		{
			switch (option_index)
			{
			case 0:
				if (!ConvertStringToUI64(optarg, &k))
				{
					fprintf(stderr, "Invalid argument k\n");
					exit(1);
				}
				printf("k = %lu  ", k);
				break;
			case 1:
				if (!ConvertStringToUI64(optarg, &mod))
				{
					fprintf(stderr, "Invalid argument mod\n");
					exit(1);
				}
				printf("mod = %lu\n", mod);
				break;
			case 2:
				memcpy(servers, optarg, strlen(optarg) + 1);//XXX
				printf("servers = ");
				printf(servers);
				printf("\n");
				break;
			default:
				printf("Index %d is out of options\n", option_index);
			}
		} break;

		case '?':
			printf("Arguments error\n");
			break;
		default:
			fprintf(stderr, "getopt returned character code 0%o?\n", c);
		}
	}

	if (k == -1 || mod == -1 || !strlen(servers))
	{
		fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
			argv[0]);
		return 1;
	}

	struct Server *to = parseFile(servers);
	unsigned int servers_num = getServerNum(to);     	
	printf("Client: Number of servers = %d\n", servers_num);    
	struct TaskToServer * task = malloc(sizeof(struct TaskToServer)*servers_num);	
    pthread_t thread[servers_num];
    int part =k/servers_num;
    int l=0;
    if(k%2!=0)
    {
        l=1;
    }
	for (int i = 0; i < servers_num; i++)
	{
	    if(i==0)
		{
			task[i].server = to[i];
			task[i].begin = 1;
			task[i].end = 1+(k-1)/servers_num;
			task[i].mod = mod;
		}
		else
		{
			task[i].server = to[i];
			task[i].begin = task[i-1].end+1;
			task[i].end = task[i].begin + (k-1)/servers_num;
			task[i].mod = mod;
			if(task[i].end > (k))
			{
				(task[i]).end = k;
			}
		}
		if (pthread_create(&(thread[i]), NULL, (void *)sendTaskToServer, (void *)&(task[i])) != 0) 
        {
            perror("pthread_create");
            exit(1);
        }
	}
	
	for (int i = 0; i < servers_num; i++)
	{
	    if (pthread_join(thread[i], NULL) != 0) 
      {
            perror("pthread_join");
            exit(1);
      }
	}
	
	//printf("Client: factorial = %lu, %lu \n", result,result%mod);    
    //result = MultModulo(1, result, task->mod);
    printf("Client: factorial = %lu, mod = %lu \n", fact,result);  
	
	free(to);
    result = 1;

	return 0;
}