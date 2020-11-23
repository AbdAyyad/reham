#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <ctype.h>
#include <time.h>

#define SZ 32 * 1024 //buffer size
#define MAX_TRAILS 25
#define UPPER 9999
#define LOWER 1

void sig_child(int signo)
{

    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("Child %d terminated.\n\n", pid);
}

//this function recives the request and process it and send it
void str_echo(int sockfd, char *ip, int port)
{
    /* all needed vars */
    ssize_t n;
    char buf[SZ];
    bzero(buf, SZ);
    char *out = malloc(SZ * sizeof(char));
    bzero(out, sizeof(out));
    int solved = 0;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    // printf("time %d\n", start.tv_sec);
    srand(start.tv_sec);
    int randomNumber = (rand() % (UPPER - LOWER + 1)) + LOWER;
    //this loop reads the requests one by one and process them
    for (int i = 0; i < MAX_TRAILS && !solved; ++i)
    {
        n = read(sockfd, buf, SZ);
        //printf("%s\n", buf); //print the request

        int clientNum = atoi(buf);
        printf("recieved \"%d\" from client ip %s port %d\n", clientNum, ip, port);

        if (clientNum == 0)
        {
            sprintf(buf, "you gave up after %d trials, %d is the secret", i, randomNumber);
            solved = 1;
        }
        else if (clientNum == randomNumber)
        {
            strcpy(buf, "ok");
            solved = 1;
        }
        else if (i == MAX_TRAILS - 1)
        {
             sprintf(buf, "Sorry, %d is an incorrect guess. and you reached max attemps closing connection", clientNum);
        }
        else
        {
            sprintf(buf, "Sorry, %d is an incorrect guess. Try again", clientNum);
        }

        printf("sending %s\n", buf);
        write(sockfd, buf, strlen(buf));
    }

    read(sockfd, buf, SZ);
    // bzero(&buf,sizeof(buf));
    // strcpy(buf, "stop");
    // write(sockfd, buf, strlen(buf));
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("port %s\n", argv[1]);
    servaddr.sin_port = htons(atoi(argv[1]));

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 1024);
    fputs("server is listening\n", stdout);

    signal(SIGCHLD, sig_child);
    for (;;)
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if ((childpid = fork()) == 0)
        {
            close(listenfd);
            // printf("ip of client %s\nport %d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
            str_echo(connfd, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
            exit(0);
        }
        close(connfd);
    }
}
