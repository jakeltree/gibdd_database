
/**
*  В.Д.Валединский
*  Простейший пример TCP клиента и сервера.
*
*  Файлы
*  cli_tcp.cpp  - пример TCP клиента
*  ser_tcp_select.cpp - пример TCP сервера c использованием select()
*
*  Клиент в цикле получает с клавиатуры текстовую строку и отсылает
*  ее на сервер, читает и выводит на экран ответ сервера.
*  Сервер ждет соединений от клиентов. При установленом
*  соединении получает строку от клиента, переводит ее в верхний
*  регистр и отсылает обратно клиенту. Если клиент посылает строку
*  содержащую слово stop, то сервер закрывает соединение с этим клиентом,
*  а клиент заканчивает свою работу.
*  Клиент и сервер печатают на экран протокол своей работы т.е.
*  разные данные, которые они получают или отсылают.
*
*  Для ясности примеры составлены максимально просто и не анализируют
*  некорректные ответы и запросы клиента и сервера, возможность переполнения
*  буферов ввода вывода, неожиданное 'падение' сервера и т.п.
*
*/


#include "stdafx.h"
#pragma warning(suppress : 4996)
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>


// Определимся с номером порта и другими константами.
#define PORT    5555
#define BUFLEN  4096

// Две вспомогательные функции для чтения/записи (см. ниже)
int   readFromClient(int fd, char *buf);
void  writeToClient(int fd, char *buf);


int  main(void)
{
    int     i, err, opt = 1;
    int     sock, new_sock;
    fd_set  active_set, read_set;
    struct  sockaddr_in  addr;
    struct  sockaddr_in  client;
    char    buf[BUFLEN];
    socklen_t  size;

    printf("http SERVER\n");

    // Инициализация windows sockets
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Создаем TCP сокет для приема запросов на соединение
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock<0) {
        perror("Server: cannot create socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Заполняем адресную структуру и
    // связываем сокет с любым адресом
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (err<0) {
        perror("Server: cannot bind socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Создаем очередь на 3 входящих запроса соединения
    err = listen(sock, 3);
    if (err<0) {
        perror("Server: listen queue failure");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Подготавливаем множества дескрипторов каналов ввода-вывода.
    // Для простоты не вычисляем максимальное значение дескриптора,
    // а далее будем проверять все дескрипторы вплоть до максимально
    // возможного значения FD_SETSIZE.
    FD_ZERO(&active_set);
    FD_SET(sock, &active_set);

    // Основной бесконечный цикл проверки состояния сокетов
    while (1) {
        // Проверим, не появились ли данные в каком-либо сокете.
        // В нашем варианте ждем до фактического появления данных.
        read_set = active_set;
        if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
            perror("Server: select  failure");
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        // fd_set в windows устроена немного иначе - как массив дескрипторов
        // Данные появились. Проверим в каком сокете.
        for (int j = 0; j<read_set.fd_count; j++) {
            i = read_set.fd_array[j];
            if (FD_ISSET(i, &read_set)) {
                if (i == sock) {
                    // пришел запрос на новое соединение
                    size = sizeof(client);
                    new_sock = accept(sock, (struct sockaddr*)&client, &size);
                    if (new_sock<0) {
                        perror("accept");
                        WSACleanup();
                        exit(EXIT_FAILURE);
                    }
                    fprintf(stdout, "Server: connect from host %s, port %hu.\n",
                        inet_ntoa(client.sin_addr),
                        ntohs(client.sin_port));
                    FD_SET(new_sock, &active_set);
                    fprintf(stdout, "new socket = %d\n", new_sock);
                }
                else {
                    fprintf(stdout, "socket = %d\n", i);
                    // пришли данные в уже существующем соединени
                    err = readFromClient(i, buf);
                    if (err<0) {
                        // ошибка или конец данных
                        closesocket(i);
                        FD_CLR(i, &active_set);
                        fprintf(stdout, "close socket = %d\n", i);
                    }
                    else {
                        // а если это команда закончить работу?
                        if (strstr(buf, "stop")) {
                            closesocket(i);
                            FD_CLR(i, &active_set);
                            fprintf(stdout, "close socket = %d\n", i);
                        }
                        else {
                            // данные прочитаны нормально
                            writeToClient(i, buf);
                            //closesocket(i);
                            //FD_CLR(i, &active_set);
                        }
                    }
                }
            }
        }
    }
    WSACleanup();
}

int  readFromClient(int fd, char *buf)
{
    int  nbytes;

    nbytes = recv(fd, buf, BUFLEN, 0);
    fprintf(stdout, "reading %d bytes from socket %d\n", nbytes, fd);
    if (nbytes<0) {
        // ошибка чтения
        perror("Server: read failure");
        return -1;
    }
    else {
        // есть данные
        fprintf(stdout, "Server got message: %s\n", buf);
        return 0;
    }
}


void  writeToClient(int fd, char *buf)
{
    int  nbytes;
    unsigned char *s;

    for (s = (unsigned char*)buf; *s; s++) *s = toupper(*s);
    nbytes = send(fd, buf, strlen(buf) + 1, 0);
    fprintf(stdout, "Write back: %s\nnbytes=%d\n", buf, nbytes);

    if (nbytes<0) {
        perror("Server: write failure");
    }
}





