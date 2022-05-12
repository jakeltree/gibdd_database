
/**
*  �.�.�����������
*  ���������� ������ TCP ������� � �������.
*
*  �����
*  cli_tcp.cpp  - ������ TCP �������
*  ser_tcp_select.cpp - ������ TCP ������� c �������������� select()
*
*  ������ � ����� �������� � ���������� ��������� ������ � ��������
*  �� �� ������, ������ � ������� �� ����� ����� �������.
*  ������ ���� ���������� �� ��������. ��� ������������
*  ���������� �������� ������ �� �������, ��������� �� � �������
*  ������� � �������� ������� �������. ���� ������ �������� ������
*  ���������� ����� stop, �� ������ ��������� ���������� � ���� ��������,
*  � ������ ����������� ���� ������.
*  ������ � ������ �������� �� ����� �������� ����� ������ �.�.
*  ������ ������, ������� ��� �������� ��� ��������.
*
*  ��� ������� ������� ���������� ����������� ������ � �� �����������
*  ������������ ������ � ������� ������� � �������, ����������� ������������
*  ������� ����� ������, ����������� '�������' ������� � �.�.
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

using namespace std;
#include <iostream>
#include <sstream>

// ����������� � ������� ����� � ������� �����������.
#define PORT    5555
#define BUFLEN  4096

// ��� ��������������� ������� ��� ������/������ (��. ����)
int   readFromClientHTTP(int fd, char *buf);
void  writeToClientHTTP(int fd, char *buf);


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

	// ������������� windows sockets
	WSADATA wsaData;
	if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed\n");
		return 1;
	}

	// ������� TCP ����� ��� ������ �������� �� ����������
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock<0) {
		perror("Server: cannot create socket");
		exit(EXIT_FAILURE);
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	// ��������� �������� ��������� �
	// ��������� ����� � ����� �������
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	err = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	if (err<0) {
		perror("Server: cannot bind socket");
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	// ������� ������� �� 3 �������� ������� ����������
	err = listen(sock, 3);
	if (err<0) {
		perror("Server: listen queue failure");
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	// �������������� ��������� ������������ ������� �����-������.
	// ��� �������� �� ��������� ������������ �������� �����������,
	// � ����� ����� ��������� ��� ����������� ������ �� �����������
	// ���������� �������� FD_SETSIZE.
	FD_ZERO(&active_set);
	FD_SET(sock, &active_set);

	// �������� ����������� ���� �������� ��������� �������
	while (1) {
		// ��������, �� ��������� �� ������ � �����-���� ������.
		// � ����� �������� ���� �� ������������ ��������� ������.
		read_set = active_set;
    	if ( select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
			perror("Server: select  failure");
			WSACleanup();
			exit(EXIT_FAILURE);
		}
		// fd_set � windows �������� ������� ����� - ��� ������ ������������
		// ������ ���������. �������� � ����� ������.
		for (int j = 0; j<read_set.fd_count; j++) {
			i = read_set.fd_array[j];
			if (FD_ISSET(i, &read_set)) {
				if (i == sock) {
					// ������ ������ �� ����� ����������
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
					// ������ ������ � ��� ������������ ���������
                    fprintf(stdout, "socket = %d\n", i);
                    err = readFromClientHTTP(i, buf);
					if (err<0) {
						// ������ ��� ����� ������
						closesocket(i);
						fprintf(stdout, "close socket = %d\n", i);
					} else  {
						// ������ ��������� ���������
						writeToClientHTTP(i, buf);
                    }
                    FD_CLR(i, &active_set);
				}
			}
		}
	}
	WSACleanup();
}

int  readFromClientHTTP(int fd, char *buf)
{
	int  nbytes;

	nbytes = recv(fd, buf, BUFLEN, 0);
	fprintf(stdout, "reading %d bytes from socket %d\n", nbytes, fd);
	if (nbytes < 0) {
		// ������ ������
		int err = WSAGetLastError();
        printf("Socket read error %d\n", err);
        return -1;
	} else {
		// ���� ������
		printf("\n\nServer gets %d bytes:\n", nbytes);
        // �������� 0 � ����� ����������� ������
        buf[min(nbytes, BUFLEN - 1)] = 0;
        printf("%s\n", buf);
		return nbytes;
	}
}



#include <time.h>

void ProcessHTML(stringstream &html, string & request)
{
    // ������ - ���������� ������� � ���� comment  
#define TBUFLEN 64    // ������� �� ��������� ���������� ��������� ��� ���� �����              
                      // ������ �� ������ ���� ��� ��� ��-�� ����, ��� ��� �����������
                      // ������� tbuf � ��� ��� ������������� � ������ ctime_s 
                      // �� ���������� � ����������� ��������� ������ ����� �������� (64 � 256)
    char tbuf[TBUFLEN] = {0};
    time_t ttt;
    time(&ttt);
    ctime_s(tbuf, TBUFLEN, &ttt);
    printf("%s\n",tbuf);
   
    html.clear();
	html << "<!DOCTYPE html>\r\n";
	html << "<html><head><meta charset = \"cp1251\">\r\n";
	html << "<title>������� ������</title></head>\r\n";
	html << "<body>\r\n";
	html << "<h2>��� �� �.�.����������</h2>\r\n";
	//html << "<form name=\"myform\" action=\"http://localhost:5555\"  method=\"post\">\r\n";
    html << "<form name=\"myform\" action=\"http://localhost:5555\"  method=\"get\">\r\n";
    html << "<input name=\"request\" type=\"text\" width=\"20\"> request<br>\r\n";
	//html << "<input name=\"comment\" type=\"text\" width=\"20\" value=\"some text\"> comment<br>\r\n";
    html << "<input name=\"comment\" type=\"text\" width=\"20\" value=\"" << tbuf << "\"> comment<br>\r\n";
    html << "1 <input name=\"rb1\" type=\"radio\" value=\"check1\">\r\n";
	html << "2 <input name=\"rb1\" type=\"radio\" value=\"check2\" checked>\r\n";
	html << "3 <input name=\"rb2\" type=\"radio\" value=\"check3\">\r\n";
	html << "4 <input name=\"rb2\" type=\"radio\" value=\"check4\"><br>\r\n";
	html << "A <input name=\"ch1\" type=\"checkbox\" value=\"checkA\">\r\n";
	html << "B <input name=\"ch2\" type=\"checkbox\" checked>\r\n";
	html << "C <input name=\"ch3\" type=\"checkbox\"><br>\r\n";
	html << "<input name=\"sub\" type=\"submit\" value=\"Send\">\r\n";
	html << "</form>\r\n";
	html << "</body></html>\r\n";




}


void  writeToClientHTTP(int fd, char *buf)
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

    // ����� �� �������������� �������    
    if (request != "/" && request.find("request") == string::npos)  {
        head << "HTTP/1.1 404 Not found\r\n";
        head << "Connection: close\r\n";
        head << "Content-length: 0\r\n";
        head << "\r\n";
        nbytes = send(fd, head.str().c_str(), head.str().length() + 1, 0);
        return;
    }

    // ����� �� ������������ �������
    // ������������ HTML
	ProcessHTML(html, request);
    int html_length = html.str().length();

    // ������������ HTTP ���������
	head << "HTTP/1.1 200 OK\r\n";
	head << "Connection: keep-alive\r\n";
	head << "Content-type: text/html\r\n";
	head << "Content-length: " << html_length << "\r\n";
	head << "\r\n";

    // ����� ��������� � html
	nbytes = send(fd, head.str().c_str(), head.str().length() + 1, 0);
	printf("http nb = %d\n", nbytes);
	nbytes = send(fd, html.str().c_str(), html.str().length() + 1, 0);
	printf("html nb = %d\n", nbytes);

	
	//cout << "Write back\n";
	//cout << head.str();
	//cout << html.str();

	if (nbytes<0) {
		perror("Server: write failure");
	}
}




