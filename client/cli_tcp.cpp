/**
 *  В.Д.Валединский
 *  Простейший пример TCP клиента и сервера.
 * 
 *
 *  Файлы    
 *  cli_tcp.cpp  - пример TCP клиента
 *  ser_tcp_select.cpp - пример TCP сервера c использованием select()
 *  ser_tcp_poll.cpp   - пример TCP сервера c использованием poll()
 *  ser_tcp_fork.cpp   - пример TCP сервера c использованием fork()
 *  ser_tcp_prefork.cpp   - пример TCP сервера c использованием fork()
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



#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Определимся с портом, адресом сервера и другими константами.
// В данном случае берем произвольный порт и адрес обратной связи
// (тестируем на одной машине).
#define  SERVER_PORT     5555
#define  SERVER_NAME    "127.0.0.1"
#define  BUFLEN          512 

// Две вспомогательные функции для чтения/записи (см. ниже)
int  writeToServer  (int fd);
int  readFromServer (int fd);


int  main (void)
{
//    int i;
    int err;
    int sock;
    struct sockaddr_in server_addr;
    struct hostent    *hostinfo;

    // Получаем информацию о сервере по его DNS имени
    // или точечной нотации IP адреса.
    hostinfo = gethostbyname(SERVER_NAME);
    if ( hostinfo==NULL ) {
        fprintf (stderr, "Unknown host %s.\n",SERVER_NAME);
        exit (EXIT_FAILURE);
    }

    // Заполняем адресную структуру для последующего
    // использования при установлении соединения
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr = *(struct in_addr*) hostinfo->h_addr;

    // Создаем TCP сокет.
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if ( sock<0 ) {
        perror ("Client: socket was not created");
        exit (EXIT_FAILURE);
    }

    // Устанавливаем соединение с сервером
    err = connect (sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if ( err<0 ) {
        perror ("Client:  connect failure");
        exit (EXIT_FAILURE);
    }
    fprintf (stdout,"Connection is ready\n");

    writeToServer(sock);
    readFromServer(sock);
    fprintf (stdout,"The end\n");


    // Закрываем socket
    close (sock);
    exit (EXIT_SUCCESS);
}

int fgetss(char* buf, int LEN, FILE *f)
{
    // прочитать до LEN следующих символов
    for (int i = 0; i < LEN; i++)
    {
        char c = fgetc(f);
        if (c == EOF || c == '\0') {
            buf[i] = '\0';
            return 1;
        }
        buf[i] = c;
    }
    return 0;
}

int  writeToServer (int fd)
{
    int   nbytes;
    FILE* fp;
    fp = fopen("requests.txt", "r");

    while (1) {
        char buf[BUFLEN];
        if (fgetss(buf, BUFLEN, fp)==1) {
            write (fd, buf, BUFLEN);
            write(fd, "#", 2);
            fclose(fp);
            return 0;
        }
        buf[BUFLEN] = 0;
        nbytes = write (fd, buf, BUFLEN);
        printf("%s\n", buf);
        if (nbytes<=0) { perror("write"); fclose(fp);return -1; }
    }
}




int  readFromServer (int fd)
{
    int  nbytes;
    FILE* fp = fopen("selection_client.csv", "w");
    while (1) {
        char buf[BUFLEN];
        nbytes = read(fd,buf,BUFLEN);
        buf[BUFLEN] = '\0';
        if ( nbytes<0 ) {
            perror ("Server: read failure");
            fclose(fp);
            return 0;
        } 
        if (nbytes == 0 || buf[0] == '#') {
            fclose(fp);
            return 1;
        }
        fprintf(fp, "%s", buf);
    }
}