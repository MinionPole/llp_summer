#include <stdio.h>  
#include <stdlib.h>  
#include "fileWork.h"
#include "tablework.h"
#include "xml.h"
#include <time.h>
#include "executer.h"


int sum_len = 0;

int generate_rand_cell(struct cell* to_gener) {
    int val = rand();
    if (val % 3 == 0) {
        to_gener->flag = INT_DATA;
        to_gener->int_data = (abs(rand()) % 100) + 1;
    }
    if (val % 3 == 1) {
        to_gener->flag = DOUBLE_DATA;
        to_gener->double_data = ((rand()) / 100) + 1;
    }
    if (val % 3 == 2) {
        int len = (abs(rand()) % 30) + 1;
        to_gener->flag = STRING_DATA;
        to_gener->string_link.str_len = len;
        to_gener->string_link.string_data = malloc(len * sizeof(char));
        sum_len += len;
        for (int i = 0; i < len; i++) {
            to_gener->string_link.string_data[i] = 'a' + (abs(rand()) % 25);
        } 
    }
}



int main(void) {



    printf("%d", charsOnList);

    char* filename = "file.db";


    initFile(filename);

    struct FileMapping* myMapping = openMapping(filename);

    SOCKET servSock = create_server_socket();
    bind_server_socket(servSock);
    listen_server_socket(servSock);

    //Client socket creation and acception in case of connection
    struct sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

    int clientInfo_size = sizeof(clientInfo);

    SOCKET ClientConn = accept(servSock, (struct sockaddr*)&clientInfo, &clientInfo_size);
    int while_flag = 1;

    while (while_flag) {
        //printf("hell0?\n");
        char* request = NULL;
        int mes_size = 0;
        request = receive_message(ClientConn, &mes_size);

        if (mes_size >= 3 && request[0] == 'x' && request[1] == 'x' && request[2] == 'x') {
            while_flag = 0;
            continue;
        }

        struct ast* query = parse_xml_to_ast(request, mes_size);

        int ans_size = 0;
        char* answer = NULL;

        answer = execute_ast(myMapping, query, &ans_size);

        send_message(ClientConn, answer, &ans_size);

        if (request != NULL)
            free(request);
    }
    return 0;

    const char* xmlString2 = "<?xml version=\"1.0\"?>"
        "<root>"
        "  <insert tabname=\"roman\">"
        "    <list>"
        "      <pair key=\"3\">"
        "        <int>3</int>"
        "      </pair>"
        "      <pair key=\"2\">"
        "        <int>2</int>"
        "      </pair>"
        "      <pair key=\"1\">"
        "        <string>'roman'</string>"
        "      </pair>"
        "    </list>"
        "  </insert>"
        "</root>";

    const char* xmlString = "<?xml version=\"1.0\"?>"
        "<root>"
        "  <select tabname=\"roman\">"
        "    <filter operation=\"-1\">"
        "      <filter_expr cmp=\"&lt;\">"
        "        <table_raw tabname=\"roman\" raw_num=\"2\"/>"
        "        <int>4</int>"
        "      </filter_expr>"
        "    </filter>"
        "  </select>"
        "</root>";

    struct table_data table3;
    strncpy_s(table3.table_name, sizeof(table3.table_name) - 1, "roman", strlen("roman"));
    table3.row_size = 3;
    //addTable(myMapping, &table3);

    /*struct ast* get2 = parse_xml_to_ast(xmlString2, strlen(xmlString2));
    int len2 = 0;
    printf("\n%s\n", execute_ast(myMapping, get2, &len2));*/

    struct ast* get = parse_xml_to_ast(xmlString, strlen(xmlString));
    int len = 0;
    printf("%s", execute_ast(myMapping, get, &len));




    /*int pages = getHowManyPages(myMapping);
    printf("количество страниц %d", pages);

    struct table_data table1;
    table1.table_name[0] = '1';
    table1.row_size = 1;

    struct table_data table2;
    table2.table_name[0] = '2';
    table2.row_size = 2;

    struct table_data table3;
    table3.table_name[0] = '3';
    table3.row_size = 3;

    struct table_data table4;
    table4.table_name[0] = '4';
    table4.row_size = 4;


    int res = 0;
    addTable(myMapping, &table1);
    //addTable(myMapping, &table2);
    addTable(myMapping, &table3);
    addTable(myMapping, &table4);

    struct table_data new_table_1;
    new_table_1.table_name[0] = '1';
    get_table_data(myMapping, &new_table_1, -1);
    new_table_1.row_size = 2;

    updateTable(myMapping, &table1, &new_table_1, -1);

    //res = deleteTable(myMapping, &table2, -1);
    printf("delete result %d", res);

    struct cell raw1[2];
    raw1[0].int_data = 1;
    raw1[0].flag = INT_DATA;
    raw1[1].int_data = 2;
    raw1[1].flag = INT_DATA;

    int adden = 0;
    get_table_data(myMapping, &new_table_1, -1);
    //add_raw(myMapping, raw1, 2, &new_table_1, &adden);


    struct cell raw2[3];
    raw2[0].int_data = 1;
    raw2[0].flag = INT_DATA;
    raw2[1].double_data = 1.77;
    raw2[1].flag = DOUBLE_DATA;
    raw2[2].string_link.str_len = 3;
    raw2[2].string_link.string_data = "hey";

    adden = 0;
    get_table_data(myMapping, &table3, -1);
    //add_raw(myMapping, raw2, 3, &table3, &adden);
    int glob_ind = 0;;
    for (int zzz = 0; zzz <= 1000; zzz++) {

        for (int i = 0; i < 1000; i++) {
            adden = 0;
            struct table_data temp_table;
            temp_table.table_name[0] = '3';
            get_table_data(myMapping, &temp_table, -1);

            struct cell* to_add = malloc(sizeof(struct cell) * temp_table.row_size);
            (*to_add).int_data = glob_ind++;
            (*to_add).flag = INT_DATA;
            for (int j = 1; j < temp_table.row_size; j++)
                generate_rand_cell(&(to_add[j]));
            add_raw(myMapping, to_add, 3, &temp_table, &adden);

            for (int j = 1; j < temp_table.row_size; j++)
                if (to_add[j].flag == STRING_DATA)
                    free(to_add[j].string_link.string_data);
            free(to_add);
            //printf("%d correct added\n", i + 1);
        }

        clock_t start = clock(); // Запоминаем начальное время
        struct queryCondition conditions1;
        conditions1.conds = GREATER;
        conditions1.type = INT_DATA;
        conditions1.int_data = glob_ind - 1000 / 3;
        conditions1.stolbec_num = 0;

        struct selectQueryStruct selectQuery;
        selectQuery.conditions = &conditions1;

        selectQuery.conditions_num = 1;
        selectQuery.from_select = &table3;
        selectQuery.Maping = myMapping;
        selectQuery.ret = NULL;


        get_table_data(myMapping, &table3, -1);

        selectFromTable(&selectQuery);
        freeFullRawSequence(selectQuery.ret);
        clock_t end = clock(); // Запоминаем конечное время
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC; // Вычисляем время в секундах
        printf("%f\n", time_taken);
    }

    for (int i = 0; i < 100; i++) {
        adden = 0;
        struct table_data temp_table;
        temp_table.table_name[0] = '4';
        get_table_data(myMapping, &temp_table, -1);

        struct cell* to_add = malloc(sizeof(struct cell) * temp_table.row_size);
        (*to_add).int_data = i + 1;
        (*to_add).flag = INT_DATA;
        for (int j = 1; j < temp_table.row_size; j++)
          generate_rand_cell(&(to_add[j]));
        add_raw(myMapping, to_add, 4, &temp_table, &adden);

        for (int j = 1; j < temp_table.row_size; j++)
          if (to_add[j].flag == STRING_DATA)
            free(to_add[j].string_link.string_data);
        free(to_add);
    }
    struct queryCondition conditions1;
    conditions1.conds = LESS;
    conditions1.type = INT_DATA;
    conditions1.int_data = 10;
    conditions1.stolbec_num = 0;

    struct queryCondition conditions2;
    conditions2.conds = EQUAL;
    conditions2.type = INT_DATA;
    conditions2.int_data = 3;
    conditions2.stolbec_num = 0;

    /*struct selectQueryStruct selectQuery;
    selectQuery.conditions = &conditions;

    selectQuery.conditions_num = 1;
    selectQuery.from_select = &table3;
    selectQuery.Maping = myMapping;
    selectQuery.ret = NULL;


    get_table_data(myMapping, &table3, -1);
    selectFromTable(&selectQuery);

    printFullRawSequence(selectQuery.ret);
    freeFullRawSequence(selectQuery.ret);

    struct JoinQueryStruct joinq;
    joinq.conditions1 = &conditions1;
    joinq.conditions2 = &conditions2;
    joinq.conditions_num1 = 1;
    joinq.conditions_num2 = 1;

    get_table_data(myMapping, &table3, -1);
    get_table_data(myMapping, &table4, -1);
    joinq.table1_join = &table3;
    joinq.table2_join = &table4;

    joinq.Maping = myMapping;

    joinq.ret = NULL;
    joinq.join_cond.first_column = 0;
    joinq.join_cond.second_column = 0;

    joinTable(&joinq);
    printFullRawSequence(joinq.ret);


    /*struct queryCondition conditions2[2];
    conditions2[0].conds = GREATER;
    conditions2[0].type = INT_DATA;
    conditions2[0].int_data = 50;
    conditions2[0].stolbec_num = 0;

    conditions2[1].conds = LESS;
    conditions2[1].type = INT_DATA;
    conditions2[1].int_data = 83;
    conditions2[1].stolbec_num = 0;
    
    
    deleteRaw(myMapping, &table3, &conditions2, 2);

    ans = NULL;

    get_table_data(myMapping, &table3, -1);
    ans = selectFromTable(myMapping, &table3, &conditions, 1, ans);
    printFullRawSequence(ans);
    freeFullRawSequence(ans);*/
    

    return 0;
}