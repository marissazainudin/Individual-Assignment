// QOTD server code
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

//to handle error 
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)
//variable declaration
int day, port;
char *QOTD;
const char *gtQOTD;
pthread_mutex_t quoteLock=PTHREAD_MUTEX_INITIALIZER;
pthread_t checkForNewDayThread, connectionHandlerThread;

    
//link to quotes file
int line_no(const char* file)
{
    int count = 0 ;
    int currentChar ;
    FILE* fds = fopen(file, "r") ;
	
    if(fds==NULL)
	{
		perror("Error to open quotes file") ;
		exit(EXIT_FAILURE) ;
    }
    while(true)
      {
        currentChar = fgetc(fds) ;
        switch (currentChar)
	{
            case '\n':
	    {
                count++ ;
                break ;
            }
            case EOF:
	    {
                fclose(fds) ;
                return count ;
            }
            default:
                continue ;
        }
	}
}
// to search for quotes file
// to send quotes to buffer address
char* quote_read(const char* filePath)
{
	int quotes_no=line_no(filePath) ;
	int lineNumOfQOTD=rand()%quotes_no ;
	int lineCount=0 ;
	char* lineptr=NULL ; 
	size_t n=0;
	FILE* fds=fopen(filePath, "r") ;

    if(fds==NULL)
	{
        perror("Error to open quotes file") ;
        exit(EXIT_FAILURE) ;
    }
   while(lineCount<lineNumOfQOTD)
	{ 
		if(fgetc(fds)=='\n') lineCount++ ;
	}
    getline(&lineptr, &n, fds) ; 
    fclose(fds) ;
    return lineptr ;
}

//handle connection
void * connection(void* port_ptr)
{
    struct sockaddr_in address ;
    int server_fds, new_socket, opt = 1, addrlen = sizeof(address), port=*((int*) port_ptr) ;

    free(port_ptr) ;

//create socket file
    if ((server_fds = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed") ;
        exit(EXIT_FAILURE) ;
    }
//assign ip and port to 1717
    if (setsockopt(server_fds, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))==-1)
    {
        perror("setsockopt") ;
        exit(EXIT_FAILURE) ;
    }
    address.sin_family = AF_INET ;
    address.sin_addr.s_addr = INADDR_ANY ;
    address.sin_port = htons(port) ;

    if (bind(server_fds, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed") ;
        exit(EXIT_FAILURE) ;
    }
    if (listen(server_fds, 100) < 0)
    {
        perror("listen") ;
        exit(EXIT_FAILURE) ;
    }

    printf("Listening on port %i\n", port);

    while(1)
	{ 
        if ((new_socket = accept(server_fds, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0)
		{
            perror("accept") ;
            exit(EXIT_FAILURE) ;
        }
        pthread_mutex_lock(&quoteLock) ;
        send(new_socket, QOTD, strlen(QOTD), 0) ;
        pthread_mutex_unlock(&quoteLock) ;
        close(new_socket) ;
    }
}

int main(int argc, char const *argv[])
{
    int thread1, thread2, join ;
    int* port=malloc(sizeof(int)) ;

    if(port==NULL)
	{
		perror("Couldn't allocate memory to store listening port") ;
		exit(EXIT_FAILURE) ;
    }

    time_t time1=time(NULL) ; 
    struct tm tm = *localtime(&time1) ;
    day=tm.tm_mday ; 

    switch(argc)
	{ 
        case 1:
		{ 
            *port=1717 ;
            gtQOTD=strdup("quotes.txt") ;
            break ;
        }
        case 3:
		{ 
            *port=atoi(argv[2]) ;
            gtQOTD=argv[1] ;
            break ;
        }
        default:
		{
            fprintf(stderr,"Bad arguments\n") ;
            fprintf(stderr,"Usage:\n%s [path_to_quotes_file] [port]\nServer will read quotes from file named \"q.txt\" in the current directory and listens to the port 1717.\nNOTE: the standard port of QOTD protocol is 17.\nOnce an argument was specified, all arguments became mandatory.\nThe file containing the quotes must be a .txt file that contains one quote per line and ending with exactly one new line\n", argv[0]) ;
            exit(EXIT_FAILURE) ;
        }

    }

	printf("Username : %s", getlogin()) ;
	printf("\n") ;
	printf("Quotes file contains %i quotes\n", line_no(gtQOTD)) ;
    srand(time1) ; 
    QOTD = quote_read(gtQOTD) ; 
    connection(port) ;

    return 0 ;
}
