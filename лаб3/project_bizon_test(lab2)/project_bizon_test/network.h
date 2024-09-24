#pragma once
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <winsock2.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#define DEFAULT_BUFFER_SIZE 4096

#pragma comment(lib, "Ws2_32.lib")

SOCKET create_client_socket();

int connect_server_socket(SOCKET ClientSock);

char* receive_message(SOCKET sock, int* str_len);

void send_message(SOCKET ClientConn, char* answer, int* ans_size);