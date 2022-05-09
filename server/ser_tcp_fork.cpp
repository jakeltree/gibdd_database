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
// Определимся с номером порта и другими константами.
#define PORT    5559
#define BUFLEN  512

// функции для чтения/записи (см. ниже)
bool  readFromClient(int fd);
int  writeToClient (int fd);
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
			  ProcessClientRequest(new_sock);

              // очистим временные файлы
              char part[BUFLEN];
              sprintf(part, "%d", getpid());
              remove(strcat(part, "_selection.csv"));
              sprintf(part, "%d", getpid());
              remove(strcat(part, "_hello.txt"));
			  return 0;
		  } else {
			  // родительский процесс
			  while ( waitpid(-1, nullptr, WNOHANG) > 0 );
		  }
	   }
	}
}
namespace server_cpp {
  void CreateSelection() { 
      std::ifstream file_request(std::to_string(getpid()) + "_hello.txt");
	    std::string request;
      getline(file_request, request);
      file_request.close();

	    database::SelectionTree tr(request);
      std::string path1 = "database.csv";
	    database::Seabase b(path1); 
      std::string path2 = std::to_string(getpid()) + "_selection.csv";
      b.SelectionToFile(b.ParseTreeToSelection(tr), path2);
  }
}

bool  ProcessClientRequest (int cli_socket)
{
	if ( !readFromClient (cli_socket) ) {
		printf("read error: connection is closed\n");
		close(cli_socket);
		return false;
	}
	server_cpp::CreateSelection();
	if ( !writeToClient (cli_socket) ) {
		printf("read error: connection is closed\n");
		close(cli_socket);
		return false;
	}
	return true;
}
bool  readFromClient (int fd)
{
	int  nbytes;
    char part[BUFLEN];
    sprintf(part, "%d", getpid());
	FILE* fp = fopen(strcat(part, "_hello.txt"), "w");
	while (1) {
		char buf[BUFLEN];
		nbytes = read(fd,buf,BUFLEN);
		buf[BUFLEN] = '\0';
		if (nbytes<0 ) {
			perror ("Server: read failure");
			fclose(fp);
			return false;
		}
		if (nbytes == 0 || buf[0] == '#') {
			fclose(fp);
			return true;
		}
		printf("nbytes: %d message: %s\n", nbytes, buf);
		fprintf(fp, "%s", buf);
	}
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

int  writeToClient (int fd)
{
	int   nbytes;
	FILE* fp;
    char part[BUFLEN];
    sprintf(part, "%d", getpid());
	fp = fopen(strcat(part, "_selection.csv"), "r");
	while (1) {
		char buf[BUFLEN];
		if (fgetss(buf, BUFLEN, fp)==1) {
			write (fd, buf, BUFLEN);
			write(fd, "#", 2);
			fclose(fp);
			return 1;
		}
		nbytes = write (fd, buf, BUFLEN);
		if (nbytes<=0) { perror("write"); fclose(fp); return -1; }
	}
}

