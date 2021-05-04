
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

#define BACKLOG 5

int running = 1;

// the argument we will pass to the connection-handler threads
struct connection {
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

//the struct to store key/value pair
typedef struct linkedlist 
{
    char* value;
    char* key;
    struct linkedlist* next;
}list;

list* head = NULL;
int server(char *port);
void *echo(void *arg);
void *response(void *arg);

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    (void) server(argv[1]);
    return EXIT_SUCCESS;
}

void handler(int signal)
{
    running = 0;
}

int server(char *port)
{
    struct addrinfo hint, *info_list, *info;
    struct connection *con;
    int error, sfd;
    pthread_t tid;

    // initialize hints
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    	// setting AI_PASSIVE means that we want to create a listening socket

    // get socket and address info for listening port
    // - for a listening socket, give NULL as the host name (because the socket is on
    //   the local host)
    error = getaddrinfo(NULL, port, &hint, &info_list);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (info = info_list; info != NULL; info = info->ai_next) {
        sfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        
        // if we couldn't create the socket, try the next method
        if (sfd == -1) {
            continue;
        }

        // if we were able to create the socket, try to set it up for
        // incoming connections;
        // 
        // note that this requires two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, info->ai_addr, info->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0)) {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (info == NULL) {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(info_list);

    struct sigaction act;
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);
	
	sigset_t mask;
	
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);

    //datalist = malloc(sizeof(list));
    // at this point sfd is bound and listening
    printf("Waiting for connection\n");
   while(running) {
    	// create argument struct for child thread
		con = malloc(sizeof(struct connection));
        con->addr_len = sizeof(struct sockaddr_storage);
        	// addr_len is a read/write parameter to accept
        	// we set the initial value, saying how much space is available
        	// after the call to accept, this field will contain the actual address length
        
        // wait for an incoming connection
        con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
        	// we provide
        	// sfd - the listening socket
        	// &con->addr - a location to write the address of the remote host
        	// &con->addr_len - a location to write the length of the address
        	//
        	// accept will block until a remote host tries to connect
        	// it returns a new socket that can be used to communicate with the remote
        	// host, and writes the address of the remote hist into the provided location
        
        // if we got back -1, it means something went wrong
        if (con->fd == -1) {
            perror("accept");
            continue;
        }

        // temporarily block SIGINT (child will inherit mask)
        error = pthread_sigmask(SIG_BLOCK, &mask, NULL);
        if (error != 0) {
        	fprintf(stderr, "sigmask: %s\n", strerror(error));
        	abort();
        }

        // spin off a worker thread to handle the remote connection
        error = pthread_create(&tid, NULL, response, con);

		// if we couldn't spin off the thread, clean up and wait for another connection
        if (error != 0) {
            fprintf(stderr, "Unable to create thread: %d\n", error);
            close(con->fd);
            free(con);
            continue;
        }

		// otherwise, detach the thread and wait for the next connection request
        pthread_detach(tid);
        
        // unblock SIGINT
        error = pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
        if (error != 0) {
        	fprintf(stderr, "sigmask: %s\n", strerror(error));
        	abort();
        }
    }
    list* cur = head;
    while(cur != NULL)
    {
        list* temp = cur;
        cur = cur->next;
        free(temp);
    }
    puts("No longer listening.");
	pthread_detach(pthread_self());
	pthread_exit(NULL);

    // never reach here
    return 0;
}

#define BUFSIZE 8

// void *echo(void *arg)
// {
//     char host[100], port[10], buf[BUFSIZE + 1];
//     struct connection *c = (struct connection *) arg;
//     int error, nread;

// 	// find out the name and port of the remote host
//     error = getnameinfo((struct sockaddr *) &c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
//     	// we provide:
//     	// the address and its length
//     	// a buffer to write the host name, and its length
//     	// a buffer to write the port (as a string), and its length
//     	// flags, in this case saying that we want the port as a number, not a service name
//     if (error != 0) {
//         fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
//         close(c->fd);
//         return NULL;
//     }

//     printf("[%s:%s] connection\n", host, port);

//     while ((nread = read(c->fd, buf, BUFSIZE)) > 0) {
//         buf[nread] = '\0';
//         printf("[%s:%s] read %d bytes |%s|\n", host, port, nread, buf);
//     }

//     printf("[%s:%s] got EOF\n", host, port);

//     close(c->fd);
//     free(c);
//     return NULL;
// }

void *response(void *arg)
{
    struct connection *c = (struct connection *) arg;
    // int error;
    int d;
    char* read = malloc(sizeof(char));
    int num_read = 0;
    int numToRead = 0;
    int numOfNewLine = 0;
    char* command = malloc(sizeof(char)*4);
    int count = 0;

    //set up read buffer fin and write buffer fout
    int sock1 = c->fd;
    int sock2 = dup(c->fd);
    if(sock2 == -1)
    {
        //perror(errno);
        fprintf(stderr, "dup: %s\n", strerror(sock2));
        abort();
    }
    FILE *fin = fdopen(sock1, "r");
    FILE *fout = fdopen(sock2, "w");

    //read
    d = getc(fin);
    while(d != EOF)
    {
        //printf("%d", d);
        num_read++;
        read = realloc(read, (num_read+1)*sizeof(char));
        read[num_read-1] = d;
        read[num_read] = '\0';
        if(num_read == 3)
        {
            // for(int i=0; i<num_read; i++)
            // {
            //     printf("%c", read[i]);
            // }
            // printf("\n");
            // strncpy(command, read, 3);
            // command[3] = '\0';
            // if(strcmp(read, "SET") == 0)
            // {
            //     printf("yes!!!!!!\n");
            // }
            // printf("%s\n", read);
            if(strcmp(read, "SET") != 0 && strcmp(read, "GET") !=0 && strcmp(read, "DEL") !=0)
            {
                //printf("here it is\n");
                fprintf(fout, "ERR\nBAD\n");
                break;
            }
        }
        else if(num_read == 4 && d != '\n')
        {
                fprintf(fout, "ERR\nBAD\n");
                break;
        }
        else if(num_read > 4 && d == '\n' && numToRead ==0)
        {
            //printf("count is %d\n", num_read);
            char temp[num_read-5];
            strncpy(temp, read+4, num_read-5);
            //printf("%s\n", temp);
            numToRead = atoi(temp);
            // printf("num to read is %d\n", numToRead);
            if(numToRead <= 0)
            {
                fprintf(fout, "ERR\nBAD\n");
                break;
            }
            count = num_read;
        }
        // else if(num_read == 5)
        // {
        //     char temp = d;
        //     numToRead = (int) temp;
        //     printf("%d\n", numToRead);
        // }
        else if(num_read > count && num_read < (count+numToRead )&& d == '\n')
        {
            numOfNewLine++;
            if(numOfNewLine > 1)
            {
                fprintf(fout, "ERR\nLEN\n");
                break;                
            }
        }
        else if(num_read == (count+numToRead) && d == '\n')
        {
            //printf("read is %s\n", read);
            command=strncpy(command, read, 3);
            command[3] = '\0';
            if(strcmp(command, "GET") ==0 && numOfNewLine == 0)
            {
                //printf("1really?\n");
                char* key = malloc(sizeof(char)*numToRead);
                key = strncpy(key, read+count, numToRead-1);
                key[numToRead-1] = '\0';
                //printf("key is %s\n", key);
                if(head == NULL)
                {
                    //printf("2really?\n");
                    fprintf(fout, "KNF\n");
                    fflush(fout);
                    // free(read);
                    read = realloc(read, sizeof(char));
                    num_read = 0;
                    numToRead = 0;
                    numOfNewLine = 0;
                    count = 0;
                }
                else if(strcmp(key, head->key) == 0)
                {
                    //printf("3really?\n");
                    fprintf(fout, "OKG\n%lu\n%s\n", strlen(head->value)+1, head->value);
                    fflush(fout);
                    read = realloc(read, sizeof(char));
                    num_read = 0;
                    numToRead = 0;
                    numOfNewLine = 0;
                    count = 0;
                }
                else
                {
                    //printf("4really?\n");
                    list* node = head;
                    while(node->next != NULL)
                    {
                        node = node->next;
                        if(strcmp(key, node->key) == 0)
                        {
                            fprintf(fout, "OKG\n%lu\n%s\n", strlen(node->value)+1, node->value);
                            fflush(fout);
                            break;
                        }
                    }
                    if(node->next == NULL)
                    {
                        fprintf(fout, "KNF\n");
                        fflush(fout);
                    }
                    read = realloc(read, sizeof(char));
                    num_read = 0;
                    numToRead = 0;
                    numOfNewLine = 0;     
                                        count = 0;   
                }
            }
            else if(strcmp(command, "SET") ==0 && numOfNewLine ==1)
            {
                //printf("%s\n", read);
                list* node = malloc(sizeof(list));
                int i = count+1;
                for(; read[i] != '\n'; i++){}
                //printf("i is %d\n", i);
                node->key = malloc(sizeof(char)*(i-count+1));
                node->key = strncpy(node->key, read+count, i-count);
                node->key[i-count] = '\0';
                //printf("key is %s\n", node->key);
                node->value = malloc(sizeof(char)*(numToRead-i+count-1));
                node->value = strncpy(node->value, read+i+1, numToRead-i+count-2);
                node->value[numToRead-i+count-2] = '\0';
                //printf("value is %s\n", node->value);
                node->next = NULL;


                if(head == NULL)
                {
                    head = node;
                }
                else
                {
                    list* temp = head;
                    while(temp != NULL)
                    {
                        if(strcmp(node->key, temp->key) == 0)
                        {
                            temp->value = node->value;
                            break;
                        }
                        temp = temp->next;
                    }
                    if(temp == NULL)
                    {
                        node->next = head;
                        head = node;           
                    }

                }
                fprintf(fout, "OKS\n");
                fflush(fout);
                    read = realloc(read, sizeof(char));
                    num_read = 0;
                    numToRead = 0;
                    numOfNewLine = 0;
                    count = 0;
            }
            else if(strcmp(command, "DEL") ==0 && numOfNewLine == 0)
            {
                int exist = 0;
                char* key = malloc(sizeof(char)*(numToRead)); 
                key = strncpy(key, read+count, numToRead-1);
                key[numToRead-1] = '\0';
                list* cur = head;
                list* pre = NULL;
                while(cur != NULL)
                {
                    if(strcmp(cur->key, key) == 0)
                    {
                        exist = 1;
                        if(pre == NULL)
                        {
                            list* temp = head;
                            head = head->next;
                            fprintf(fout, "OKD\n%lu\n%s\n", strlen(cur->value)+1, cur->value);
                            fflush(fout);
                            free(temp);
                            break;
                        }
                        else
                        {
                            list* temp = cur;
                            pre->next = cur->next;
                            fprintf(fout, "OKD\n%lu\n%s\n", strlen(cur->value)+1, cur->value);
                            fflush(fout);
                            free(temp);
                            break;
                        }

                    }
                    pre = cur;
                    cur = cur->next;
                }
                if(exist == 0)
                {
                    fprintf(fout, "KNF\n");
                    fflush(fout);
                }
                    read = realloc(read, sizeof(char));
                    num_read = 0;
                    numToRead = 0;
                    numOfNewLine = 0;
                    count = 0;
            }
            else
            {
                fprintf(fout, "ERR\nLEN\n");
                break;
            }
        }
        else if(num_read == count+numToRead && d != '\n')
        {
            fprintf(fout, "ERR\nLEN\n");
            break;
        }
        d = getc(fin);
    }

    free(command);
    free(read);
    fclose(fin);
    fclose(fout);
    free(c);
    return NULL;
}





