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

#define SZ 32 * 1024 // buffer size

void sig_quit()
{
    printf("Good Bye\n");
    exit(0);
}

/*
 * this function is the core of the client part assighment 
 * 
 * and his objective is to read the file and send it to server
 * and recieves the reply and send the reply to the print function
 */
void connection(int sockfd)
{

    char *buf = malloc(SZ * sizeof(char)); // allocating memory
    bzero(buf, sizeof(buf));               //cleaning the buffer

    int n, idx = 0;
    // n = size of incoming stream         idx = index for the current length buffer in order to add the new line
    char tmp[1024]; //temporary string

    // fgets(tmp, sizeof(tmp), stdin);

    while (1)
    {
        fputs("enter your guess: ", stdout);
        fgets(tmp, sizeof(tmp), stdin);
        //sending request
        write(sockfd, tmp, strlen(tmp));
        //recieving request
        n = read(sockfd, buf, SZ);
        //print result

        printf("server respond:%s\n", buf);

        if (strstr(buf, "closing") != NULL || strstr(buf, "secret") != NULL)
        {
            break;
        }
        bzero(buf, sizeof(buf));
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(argv[2]));

    signal(SIGQUIT, sig_quit);
    printf("ip %s\n", argv[1]);
    printf("port %s\n", argv[2]);
    // printf("enter the values of sending and reciving buffers : ");
    // int nSnd, nRcv;
    // scanf("%d %d", &nSnd, &nRcv);
    // setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &nSnd, sizeof(nSnd));
    // setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &nRcv, sizeof(nRcv));

    int n = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (n < 0)
    {
        puts("ip or port is wrong client will close");
        exit(0);
    }
    connection(sockfd);
    puts("Good Bye");
    exit(0);
}