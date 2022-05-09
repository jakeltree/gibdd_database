#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <chrono>
#include <thread>
// Определимся с номером порта и другими константами.
#define PORT    5555
#define BUFLEN  512
const int t = 1;

// функции для чтения/записи (см. ниже)
bool  readFromClient(int fd, char *buf);
bool  writeToClient (int fd, char *buf);
bool  ProcessClientRequest (int cli_socket);


int  main (void)
{
    int     err, opt=1;
    int     sock, new_sock;
    struct  sockaddr_in  addr;
    struct  sockaddr_in  client;
    socklen_t  size;

    // Создаем TCP сокет для приема запросов на соединение
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if ( sock<0 ) {
        perror ("Server: cannot create socket");
        exit (EXIT_FAILURE);
    }
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt));

    // Заполняем адресную структуру и
    // связываем сокет с любым адресом
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock,(struct sockaddr*)&addr,sizeof(addr));
    if ( err<0 ) {
        perror ("Server: cannot bind socket");
        exit (EXIT_FAILURE);
    }

    // Создаем очередь на 3 входящих запроса соединения
    err = listen(sock,3);
    if ( err<0 ) {
        perror ("Server: listen queue failure");
        exit(EXIT_FAILURE);
    }

    // разветвление - отдельный процесс для каждого клиента
    pid_t  pid;
    while (1) {
       size = sizeof(sockaddr_in);
       new_sock = accept(sock, (struct sockaddr*)&client,&size);
       if ( new_sock<0 ) {
          perror("accept");
          exit (EXIT_FAILURE);
       } else {
          pid = fork();
          if (pid == 0) {
              // порожденный процесс
              close(sock);
              while ( ProcessClientRequest(new_sock) );
              return 0;
          } else {
              // родительский процесс
              while ( waitpid(-1, nullptr, WNOHANG) > 0 );
          }
       }
    }
}


bool  ProcessClientRequest (int cli_socket)
{
    char buf[BUFLEN];
    if ( !readFromClient (cli_socket, buf) ) {
        printf("read error: connection is closed\n");
        close(cli_socket);
        return false;
    }
    sleep(atoi(buf)*t);
    if (strstr(buf, "stop")) {
        printf("client asks to close connection\n");
        close(cli_socket);
        return false;
    }
    if ( !writeToClient (cli_socket, buf) ) {
        // возвращаем то же значение, что было на входе
        printf("write error: connection is closed\n");
        close(cli_socket);
        return false;
    }
    return true;
}

bool  readFromClient (int fd, char *buf)
{
    int  nbytes;
    nbytes = read(fd,buf,BUFLEN);
    if ( nbytes<0 ) {
        perror ("Server: read failure");
        return false;
    } else if ( nbytes==0 ) {
        return false;
    } else {
        fprintf(stdout,"Server gets message: %s\n",buf);
        return true;
    }
}

bool  writeToClient (int fd, char *buf)
{
    int nbytes;
    printf("EVERYTHING IS ALRIGHT");
    // отправляем клиенту
    nbytes = write(fd,buf,strlen(buf)+1);
    // возвращаем то же значение, что было на входе
    fprintf(stdout,"Write back: %s\nnbytes=%d\n", buf, nbytes);
    if ( nbytes<0 ) {
        perror ("Server: write failure");
        return false;
    }
    return true;
}