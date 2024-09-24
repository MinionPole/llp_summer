#include "network.h"

SOCKET create_client_socket() {
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
    SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSock == INVALID_SOCKET) {
        printf("INVALID");
        closesocket(ClientSock);
        WSACleanup();
        return 1;
    }
    else
        printf("SOCKET CREATION NORM\n");
    return ClientSock;
}

int connect_server_socket(SOCKET ClientSock) {
    struct in_addr ip_to_num;
    inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
    struct sockaddr_in servInfo;

    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(1234);

    int connet_status = connect(ClientSock, (struct sockaddr*)&servInfo, sizeof(servInfo));

    if (connet_status != 0) {
        printf("Connection to Server is FAILED. Error # %d", WSAGetLastError());
        closesocket(ClientSock);
        WSACleanup();
        return 1;
    }
    else
        printf("Connection established SUCCESSFULLY. Ready to send a message to Server\n");
}

char* receive_message(SOCKET sock, int* str_len) {

    int recieve_len_flag = recv(sock, str_len, sizeof(*str_len), 0);

    if (recieve_len_flag < 0)
    {
        perror("Error sending message length");
        return -1;
    }
    char* buffer = (char*)malloc(*str_len * sizeof(char) + 1);
    int recieve_mes_flag = recv(sock, buffer, (*str_len) * sizeof(char), 0);

    if (recieve_mes_flag < 0)
    {
        perror("Error sending message length");
        return -1;
    }
    buffer[*str_len] = '\0';
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