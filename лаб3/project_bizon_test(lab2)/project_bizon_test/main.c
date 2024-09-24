#include <stdio.h>
#include "ast.h"
#include "xml.h"
#include <libxml/parser.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "network.h"

int main(int argc, char** argv) {
    extern FILE* yyin;
    errno_t err;

    SOCKET clientSock = create_client_socket();
    connect_server_socket(clientSock);


    bool sending_queries = true;
    while (sending_queries) {
        char buffer[DEFAULT_BUFFER_SIZE];
        printf("> ");
        //int printed = scanf_s("%[^\n]%*c", buffer, (unsigned)_countof(buffer));
        if (fgets(buffer, sizeof(buffer), stdin)) {
            buffer[strcspn(buffer, "\n")] = '\0';
        }

        if (!strcmp(buffer, "exit")) {
            int len = 6;
            send_message(clientSock, "xxxxxx", &len);
            sending_queries = false;
            continue;
        }

        if (!strcmp(buffer, "open")) {
            char buffer2[DEFAULT_BUFFER_SIZE];
            printf("> ");
            if (fgets(buffer2, sizeof(buffer2), stdin)) {
                buffer2[strcspn(buffer2, "\n")] = '\0';
            }

            err = fopen_s(&yyin, &buffer2, "r");
            if (err != 0) {
                fprintf(stderr, "can't open file %s\n", argv[1]);
                return 1;
            }

            struct ast* ret = parse_ast();
            /*char* to_server = ast2xml(((struct list_ast*)ret)->now);
            printf("%s\n\n\n", to_server);*/

            while (ret != NULL) {
                char* to_server = ast2xml(((struct list_ast*)ret)->now);
                //printf("%s\n\n\n", to_server);

                int len = strlen(to_server);
                send_message(clientSock, to_server, &len);
                char* ans = receive_message(clientSock, &len);
                printf("%s\n", ans);
                ret = ((struct list_ast*)ret)->next;
            }

        }


    }

    /*err = fopen_s(&yyin, "example.txt", "r");
    if (err != 0) {
        fprintf(stderr, "can't open file %s\n", argv[1]);
        return 1;
    }
    struct ast* ret = parse_ast();

    char* to_server = ast2xml(ret);
    printf("%s\n\n\n", to_server);
    fclose(yyin);

    err = fopen_s(&yyin, "customs.txt", "r");
    if (err != 0) {
        fprintf(stderr, "can't open file %s\n", argv[1]);
        return 1;
    }
    ret = parse_ast();
    to_server = ast2xml(ret);
    printf("%s\n\n\n", to_server);*/

    return 0;
}