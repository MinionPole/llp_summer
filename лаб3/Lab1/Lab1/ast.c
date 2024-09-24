#pragma once
#include "ast.h"

struct ast* newdrop(char* table_num) {
    struct drop_ast* drop = malloc(sizeof(struct drop_ast));
    if (!drop) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    drop->nodetype = NT_DROP;
    drop->num = table_num;
    return (struct ast*)drop;
}

struct ast* create_tableAst(char* table_num, int raw_size) {
    struct create_table_ast* create = malloc(sizeof(struct create_table_ast));
    if (!create) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    create->nodetype = NT_CREATE;
    create->table_num = table_num;
    create->raw_size = raw_size;
    return (struct ast*)create;
}

struct ast* newint(int value) {
    struct int_ast* int_a = malloc(sizeof(struct int_ast));
    if (!int_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    int_a->nodetype = NT_INTVAL;
    int_a->value = value;
    return (struct ast*)int_a;
}

struct ast* newdouble(double value) {
    struct double_ast* double_a = malloc(sizeof(struct double_ast));
    if (!double_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    double_a->nodetype = NT_DOUBLEVAL;
    double_a->value = value;
    return (struct ast*)double_a;
}

struct ast* newstring(char* value) {
    struct string_ast* string_a = malloc(sizeof(struct string_ast));
    if (!string_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    string_a->nodetype = NT_STRINGVAL;
    string_a->value = value;
    return (struct ast*)string_a;
}

struct ast* newbool(int value) {
    struct bool_ast* bool_a = malloc(sizeof(struct bool_ast));
    if (!bool_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    bool_a->nodetype = NT_BOOLVAL;
    bool_a->value = value;
    return (struct ast*)bool_a;
}

struct ast* newpair(int raw_num, struct ast* value) {
    struct pair_ast* pair_a = malloc(sizeof(struct pair_ast));
    if (!pair_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    pair_a->nodetype = NT_PAIR;
    pair_a->raw_num = raw_num;
    pair_a->value = value;
    return (struct ast*)pair_a;
}

struct ast* newlist(struct ast* now, struct ast* next) {
    struct list_ast* list_a = malloc(sizeof(struct list_ast));
    if (!list_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    list_a->nodetype = NT_LIST;
    list_a->next = next;
    list_a->now = now;
    return (struct ast*)list_a;
}

struct ast* newinsert(char* table_num, struct ast* list_ast) {
    struct insert_ast* insert_a = malloc(sizeof(struct insert_ast));
    if (!insert_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    insert_a->nodetype = NT_INSERT;
    insert_a->list_ast = list_ast;
    insert_a->table_num = table_num;
    return (struct ast*)insert_a;
}

struct ast* newforast(char* varname, int table_num) {
    struct for_ast* for_a = malloc(sizeof(struct for_ast));
    if (!for_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    for_a->nodetype = NT_FOR;
    for_a->table_num = table_num;
    for_a->varname = varname;
    return (struct ast*)for_a;
}

struct ast* newfilterpair(struct ast* left_oper, struct ast* sign, struct ast* right_oper) {
    struct filter_pair_ast* filter_pair_a = malloc(sizeof(struct filter_pair_ast));
    if (!filter_pair_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    filter_pair_a->nodetype = NT_FILTER_EXPR;
    filter_pair_a->left_oper = left_oper;
    filter_pair_a->sign = sign;
    filter_pair_a->right_oper = right_oper;
    return (struct ast*)filter_pair_a;
}

struct ast* newtablefraw(char* varname, int raw_num) {
    struct tablef_raw_ast* table_raw_a = malloc(sizeof(struct tablef_raw_ast));
    if (!table_raw_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    table_raw_a->nodetype = NT_TABLE_RAW;
    table_raw_a->varname = varname;
    table_raw_a->raw_num = raw_num;
    return (struct ast*)table_raw_a;
}

struct ast* newsign(char* sign) {
    struct sign_ast* sign_a = malloc(sizeof(struct sign_ast));
    if (!sign_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    sign_a->nodetype = NT_SIGN;
    sign_a->sign = sign;
    return (struct ast*)sign_a;
}

struct ast* newfilterlist(struct ast* now, int op, struct ast* next) {
    struct filter_list_ast* filt_list_a = malloc(sizeof(struct filter_list_ast));
    if (!filt_list_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    filt_list_a->nodetype = NT_FILTER_LIST;
    filt_list_a->now = now;
    filt_list_a->op = op;
    filt_list_a->next = next;
    return (struct ast*)filt_list_a;
}

struct ast* newreturn(struct ast* list) {
    struct return_ast* return_a = malloc(sizeof(struct return_ast));
    if (!return_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    return_a->nodetype = NT_RETURN;
    return_a->list = list;
    return (struct ast*)return_a;
}

struct ast* newremove(struct ast* list) {
    struct remove_ast* remove_a = malloc(sizeof(struct remove_ast));
    if (!remove_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    remove_a->nodetype = NT_REMOVE;
    remove_a->list = list;
    return (struct ast*)remove_a;
}

struct ast* newupdate(char* name, struct ast* list) {
    struct update_ast* update_a = malloc(sizeof(struct update_ast));
    if (!update_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    update_a->nodetype = NT_UPDATE;
    update_a->name = name;
    update_a->list = list;
    return (struct ast*)update_a;
}

struct ast* newforquery(struct ast* for_part, struct ast* filter_part, struct ast* conclusion_part) {
    struct for_query_ast* for_query_a = malloc(sizeof(struct for_query_ast));
    if (!for_query_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    for_query_a->nodetype = NT_FOR_QUERY;
    for_query_a->for_part = for_part;
    for_query_a->filter_part = filter_part;
    for_query_a->conclusion_part = conclusion_part;
    return (struct ast*)for_query_a;
}

struct ast* newselect(char* name, struct ast* filter_part) {
    struct select_ast* select_query_a = malloc(sizeof(struct select_ast));
    if (!select_query_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    select_query_a->nodetype = NT_SELECT;
    select_query_a->name = name;
    select_query_a->filter_part = filter_part;
    return (struct ast*)select_query_a;
}

struct ast* newjoin(char* name1, char* name2, int column1, int column2, struct ast* filter_part) {
    struct join_ast* join_a = malloc(sizeof(struct join_ast));
    if (!join_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    join_a->nodetype = NT_JOIN;
    join_a->name1 = name1;
    join_a->name2 = name2;
    join_a->num1 = column1;
    join_a->num2 = column2;
    join_a->filter_part = filter_part;
    return (struct ast*)join_a;
}

struct ast* newdelete(char* name, struct ast* filter_part) {
    struct delete_ast* delete_query_a = malloc(sizeof(struct delete_ast));
    if (!delete_query_a) {
        fprintf(stderr, "out of space");
        return NULL;
    }
    delete_query_a->nodetype = NT_DELETE;
    delete_query_a->name = name;
    delete_query_a->filter_part = filter_part;
    return (struct ast*)delete_query_a;
}

void print_spaces(int cnt) {
    printf("\n");
    for (int i = 0; i < cnt; i++) {
        printf(" ");
    }
}

void print_ast(FILE* stream, struct ast* ast, int level) {
    print_spaces(level * 2);
    //printf("\n%d  ", level);
    if (ast == NULL)
        return;
    if (ast->nodetype == NT_DROP) {
        struct drop_ast* dropast = (struct drop_ast*)ast;
        printf("drop %s", dropast->num);
    }
    if (ast->nodetype == NT_CREATE) {
        struct create_table_ast* create = (struct create_table_ast*)ast;
        printf("create %s %d", create->table_num, create->raw_size);
    }
    if (ast->nodetype == NT_INSERT) {
        struct insert_ast* ins = (struct insert_ast*)ast;
        printf("insert %s", ins->table_num);
        print_ast(stream, ins->list_ast, level + 1);
    }
    if (ast->nodetype == NT_LIST) {
        struct list_ast* list = (struct list_ast*)ast;
        printf("list_node");
        print_ast(stream, list->now, level + 1);
        if(list->next != NULL)
            print_ast(stream, list->next, level + 1);
    }
    if (ast->nodetype == NT_PAIR) {
        struct pair_ast* pair_a = (struct pair_ast*)ast;
        printf("pair %d", pair_a->raw_num);
        print_ast(stream, pair_a->value, level + 1);
    }
    if (ast->nodetype == NT_STRINGVAL) {
        struct string_ast* s_a = (struct string_ast*)ast;
        printf("%s ", s_a->value);
    }
    if (ast->nodetype == NT_INTVAL) {
        struct int_ast* i_a = (struct int_ast*)ast;
        printf("%d ", i_a->value);
    }
    if (ast->nodetype == NT_DOUBLEVAL) {
        struct double_ast* d_a = (struct double_ast*)ast;
        printf("%f ", d_a->value);
    }
    if (ast->nodetype == NT_FOR) {
        struct for_ast* for_a = (struct for_ast*)ast;
        printf("for %s in %d", for_a->varname, for_a->table_num);
    }
    if (ast->nodetype == NT_SIGN) {
        struct sign_ast* sign_a = (struct sign_ast*)ast;
        printf("%s", sign_a->sign);
    }
    if (ast->nodetype == NT_TABLE_RAW) {
        struct tablef_raw_ast* tablef_raw_a = (struct tablef_raw_ast*)ast;
        printf("%s.%d", tablef_raw_a->varname, tablef_raw_a->raw_num);
    }
    if (ast->nodetype == NT_FILTER_EXPR) {
        struct filter_pair_ast* filter_pair_a = (struct filter_pair_ast*)ast;
        printf("filter_expression");
        print_ast(stream, filter_pair_a->left_oper, level + 1);
        print_ast(stream, filter_pair_a->sign, level + 1);
        print_ast(stream, filter_pair_a->right_oper, level + 1);
    }
    if (ast->nodetype == NT_FILTER_LIST) {
        struct filter_list_ast* filter_list_a = (struct filter_list_ast*)ast;
        printf("filter_list_node");
        print_ast(stream, filter_list_a->now, level + 1);
        if(filter_list_a->op != NULL)
            if (filter_list_a->op == 1) {
                print_spaces((level + 1) * 2);
                printf("AND");
            }
            else {
                print_spaces((level + 1) * 2);
                printf("OR");
            }
        if(filter_list_a->next != NULL)
            print_ast(stream, filter_list_a->next, level + 1);
    }
    if (ast->nodetype == NT_RETURN) {
        struct return_ast* return_a = (struct return_ast*)ast;
        printf("return");
        print_ast(stream, return_a->list, level + 1);
    }
    if (ast->nodetype == NT_UPDATE) {
        struct update_ast* update_a = (struct update_ast*)ast;
        printf("update %s", update_a->name);
        print_ast(stream, update_a->list, level + 1);
    }
    if (ast->nodetype == NT_REMOVE) {
        struct return_ast* return_a = (struct return_ast*)ast;
        printf("remove");
        print_ast(stream, return_a->list, level + 1);
    }
    if (ast->nodetype == NT_FOR_QUERY) {
        struct for_query_ast* for_query_a = (struct for_query_ast*)ast;
        printf("FOR_QUERY");
        print_ast(stream, for_query_a->for_part, level + 1);
        if(for_query_a->filter_part != NULL)
            print_ast(stream, for_query_a->filter_part, level + 1);
        print_ast(stream, for_query_a->conclusion_part, level + 1);
    }
    if (ast->nodetype == NT_SELECT) {
        struct select_ast* select_query_a = (struct select_ast*)ast;
        printf("SELECT_QUERY ");
        printf("%s", select_query_a->name);
        if(select_query_a->filter_part != NULL)
            print_ast(stream, select_query_a->filter_part, level + 1);
    }
    if (ast->nodetype == NT_DELETE) {
        struct delete_ast* delete_query_a = (struct delete_ast*)ast;
        printf("DELETE_QUERY ");
        printf("%s", delete_query_a->name);
        if (delete_query_a->filter_part != NULL)
            print_ast(stream, delete_query_a->filter_part, level + 1);
    }
    if (ast->nodetype == NT_JOIN) {
        struct join_ast* join_a = (struct join_ast*)ast;
        printf("JOIN_QUERY ");
        printf(" %s.%d   %s.%d", join_a->name1, join_a->num1, join_a->name2, join_a->num2);
        if (join_a->filter_part != NULL)
            print_ast(stream, join_a->filter_part, level + 1);
    }



}



