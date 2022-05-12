/**
 *  В.Д.Валединский
 *  Простейший пример TCP клиента и сервера.
 *
 *  Файлы    
 *  cli_tcp.cpp  - пример TCP клиента
 *  ser_tcp_select.cpp - пример TCP сервера c использованием select()
 *  ser_tcp_poll.cpp   - пример TCP сервера c использованием poll()
 *  ser_tcp_fork.cpp   - пример TCP сервера c использованием fork()
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

#include "database.cc"

using namespace std;
#include <iostream>
#include <sstream>
// Определимся с номером порта и другими константами.
#define PORT    5555
#define BUFLEN  512

// функции для чтения/записи (см. ниже)
int  readFromClient(int fd, char *buf);
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
    writeToClient (cli_socket, buf);
    
    return true;
}

int min(int a, int b) {
    return  (a <= b) ? a : b;
}

int  readFromClient (int fd, char *buf)
{
    int  nbytes;

    nbytes = read(fd,buf,BUFLEN);
    if ( nbytes<0 ) {
        // ошибка чтения
        perror ("Server: read failure");
        return -1;
    } else {
        // есть данные
        printf("\n\nServer gets %d bytes:\n", nbytes);
        // поставим 0 в конце прочитанных данных
        buf[min(nbytes, BUFLEN - 1)] = 0;
        fprintf(stdout,"%s\n",buf);
        return nbytes;
    }
}
char HexSymToAnsiSym(string str) 
{
    unsigned int x;   
    std::stringstream ss;
    ss << std::hex << str;
    ss >> x;
    char c = (char) x; 
    return c;
}
string ProcessRequest(string request)
{
    // получим переменную req с сообщением в нормальной кодировке
    size_t pos1 = request.find("=") + 1;
    size_t len = request.find("&") - pos1;
    string str = request.substr(pos1, len);
    string req = "";
    size_t i = 0;
    while (i < len) {
        switch (str[i]) {
            case '+':
                req.push_back('_');
                i++;
                break;
            case '%':
                req.push_back(HexSymToAnsiSym(str.substr(i+1, 2)));
                i+=3;
                break;
            default:
                req.push_back(str[i]);
                i++;
        }
    }

    unsigned int check = request[request.find("check") + 5] - '0';
    if (check == 1) {
        //FIO case
        return "select FIO=" + req + " end";
    }
    if (check == 2) {
        //sign case
        return "select SIGN=" + req + " end";
    }
}
void ProcessHTML(stringstream &html, string & request)
{
    #define TBUFLEN 64    
    char tbuf[TBUFLEN] = {0};
    html.clear();
    html << "<!DOCTYPE html>";
    html << "<html>";
    html << "<head>";
    html << "<meta charset =\"UTF-8\">";
    html << "<title> Проверка штрафов ГИБДД </title>";
    html << "</head>";
    html << "<body>";
    html << "  <h2> Проверка штрафов ГИБДД </h2>";
    html << "<form name=\"myform\" action=\"http://localhost:5555\" method=\"get\">";
    html << "<input name=\"request\" type=\"text\" width=\"20\"> <br>";
    html << "ФИО <input name=\"rb1\" type=\"radio\" value=\"check1\">";
    html << "Гос. номер <input name=\"rb1\" type=\"radio\" value=\"check2\" checked> ";
    html << "<br>";
    html << "<input name=\"sub\" type=\"submit\" value=\"Send\">";
    html << "</form>";
   html << "<p>Штрафы</p>";
    html << "<table>";
    html << "  <tr>";
    html << "    <th>ФИО</th>";
    html << "    <th>Марка автомобиля</th>";
    html << "  <th>Гос. номер</th>";
    html << "    <th>Дата истечения штрафа</th>";
    html << "    <th>Дата выдачи штрафа</th>";
    html << "    <th>Размер штрафа</th>";
    html << "  </tr>";
    string processed_request = ProcessRequest(request);
    cout << processed_request << endl;
    string path = "database.csv";
    database::SelectionTree tr(processed_request);
    database::Seabase b(path);
    b.AddHTML(html, b.ParseTreeToSelection(tr));
    html << "</table>";

    html << "</body>";
    html << "</html>";


}


bool  writeToClient (int fd, char *buf)
{
    int  nbytes;
	stringstream head; 
	stringstream html;
	string request = "";

    if (strstr(buf, "GET")) {
        char *p1 = strchr(buf, '/');
        char *p2 = strchr(p1, ' ');
        *p2 = 0;
        request = p1;
        printf("get request: %s\n", request.c_str());
        *p2 = ' ';
    }
    if (strstr(buf, "POST")) {
        char *p1 = strstr(buf, "\r\n\r\n");
        request = (p1 + 4);
        printf("post request: %s\n", request.c_str());
    }

    // ответ на нераспознанные запросы    
    if (request != "/" && request.find("request") == string::npos)  {
        head << "HTTP/1.1 404 Not found\r\n";
        head << "Connection: close\r\n";
        head << "Content-length: 0\r\n";
        head << "\r\n";
        nbytes = write(fd, head.str().c_str(), head.str().length() + 1);
        return 0;
    }

    // ответ на распознанные запросы
    // формирование HTML
	ProcessHTML(html, request);
    int html_length = html.str().length();

    // формирование HTTP заголовка
	head << "HTTP/1.1 200 OK\r\n";
	head << "Connection: keep-alive\r\n";
	head << "Content-type: text/html\r\n";
	head << "Content-length: " << html_length << "\r\n";
	head << "\r\n";

    // вывод заголовка и html
	nbytes = write(fd, head.str().c_str(), head.str().length() + 1);
	printf("http nb = %d\n", nbytes);
	nbytes = write(fd, html.str().c_str(), html.str().length() + 1);

	//cout << "Write back\n";
	//cout << head.str();
	//cout << html.str();

	printf("html nb = %d\n", nbytes);
    if ( nbytes<0 ) {
        perror ("Server: write failure");
        return 0;
    }
    return 1;
}


