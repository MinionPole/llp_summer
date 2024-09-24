#include "network.h"

SOCKET create_server_socket() {
    //0
    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (erStat != 0) {
        printf("FE");
        return 1;
    }
    else
        printf("WSA OK\n");

    //1
    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSock == INVALID_SOCKET) {
        printf("INVALID");
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        printf("SOCKET CREATION NORM\n");
    return ServSock;
}

int bind_server_socket(SOCKET ServSock) {
    struct in_addr ip_to_num;
    int erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
    if (erStat <= 0) {
        printf("IP ERROR\n");
        return 1;
    }

    struct sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(1234);

    erStat = bind(ServSock, (struct sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        printf("Bind ERROR\n");
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        printf("Bind OK\n");
}

int listen_server_socket(SOCKET ServSock) {
    int erStat = listen(ServSock, 2);

    if (erStat != 0) {
        printf("LISTEN ERROR\n");
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {
        printf("LISTENING\n");
    }
}

char* receive_message(SOCKET sock, int* str_len) {

    int recieve_len_flag = recv(sock, str_len, sizeof(*str_len), 0);

    if (recieve_len_flag < 0)
    {
        perror("Error sending message length");
        return -1;
    }
    char* buffer = (char*)malloc(*str_len * sizeof(char));
    int recieve_mes_flag = recv(sock, buffer, (*str_len) * sizeof(char), 0);

    if (recieve_mes_flag < 0)
    {
        perror("Error sending message length");
        return -1;
    }
    //printf("%s", buffer);

    return buffer;
}

void send_message(SOCKET ClientConn, char* answer, int* ans_size) {
    int temp = *ans_size;

    int send_len_flag = send(ClientConn, &temp, sizeof(temp), 0);

    if (send_len_flag < 0)
    {
        perror("Error sending message length");
        return -1;
    }
    int send_mes_flag = send(ClientConn, answer, temp, 0);

    if (send_len_flag < 0)
    {
        perror("Error sending message length");
        return -1;
    }

    return NULL;
}