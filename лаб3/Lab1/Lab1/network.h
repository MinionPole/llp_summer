#pragma once
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <winsock2.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

SOCKET create_server_socket();

int bind_server_socket(SOCKET ServSock);

int listen_server_socket(SOCKET ServSock);

char* receive_message(SOCKET sock, int* str_len);

void send_message(SOCKET ClientConn, char* answer, int* ans_size);