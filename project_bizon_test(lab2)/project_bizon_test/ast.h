#pragma once

#include <stdio.h>
#include <stdlib.h>


//external
extern struct ast* parse_ast();

typedef enum ntype {
    /* keywords */
    NT_FOR, NT_RETURN, NT_FILTER, NT_INSERT,
    NT_UPDATE, NT_REMOVE, NT_CREATE, NT_DROP,
    NT_PAIR, NT_FILTER_CONDITION, NT_FILTER_EXPR,
    NT_ATTR_NAME, NT_LIST, NT_FILTER_LIST, NT_CREATE_PAIR, NT_MERGE, NT_FOR_QUERY,

    /* variable types */
    NT_INTEGER, NT_FLOAT, NT_STRING, NT_BOOLEAN,

    
    NT_SIGN, NT_TABLE_RAW,

    /* values */
    NT_INTVAL, NT_DOUBLEVAL, NT_STRINGVAL, NT_BOOLVAL,

    /*SQL type queries*/
    NT_JOIN, NT_DELETE, NT_SELECT, NT_UPDATE_SQL
} ntype_t;

struct ast {
    ntype_t nodetype;
    struct ast* l;
    struct ast* r;
};

struct int_ast {
    ntype_t nodetype;
    int value;
};

struct double_ast {
    ntype_t nodetype;
    double value;
};

struct bool_ast {
    ntype_t nodetype;
    int value;
};

struct string_ast {
    ntype_t nodetype;
    char* value;
};

struct pair_ast {
    ntype_t nodetype;
    int raw_num;
    struct ast* value;
};

struct list_ast {
    ntype_t nodetype;
    struct ast* now;
    struct ast* next;
};

struct filter_list_ast {
    ntype_t nodetype;
    struct ast* now;
    int op; // 1 - and, 2 - or
    struct ast* next;
};


struct drop_ast {
    ntype_t nodetype;
    char* num;
};


struct create_table_ast {
    ntype_t nodetype;
    char* table_num;
    int raw_size;
};

struct insert_ast {
    ntype_t nodetype;
    char* table_num;
    struct ast* list_ast;
};

struct for_ast {
    ntype_t nodetype;
    char* varname;
    int table_num;
};


struct filter_pair_ast {
    ntype_t nodetype;
    struct ast* left_oper;
    struct ast* sign;
    struct ast* right_oper;
};

struct tablef_raw_ast {
    ntype_t nodetype;
    char* varname;
    int raw_num;
};

struct sign_ast {
    ntype_t nodetype;
    char* sign;
};

struct for_query_ast {
    ntype_t nodetype;
    struct ast* for_part;
    struct ast* filter_part;
    struct ast* conclusion_part;
};

struct return_ast {
    ntype_t nodetype;
    struct ast* list;
};

struct remove_ast {
    ntype_t nodetype;
    struct ast* list;
};

struct update_ast {
    ntype_t nodetype;
    char* name;
    struct ast* list;
};

struct select_ast {
    ntype_t nodetype;
    char* name; // table_name
    struct ast* filter_part; // filter
};

struct delete_ast {
    ntype_t nodetype;
    char* name; // table_name
    struct ast* filter_part; // filter
};

struct join_ast {
    ntype_t nodetype;
    char* name1; // table_name
    char* name2; // table_name
    int num1; // столбец таблицы 1
    int num2;// столбец таблицы 2
    struct ast* filter_part; // filter
};


struct ast* newdrop(char* table_num);
struct ast* newint(int value);
struct ast* newdouble(double value);
struct ast* newstring(char* value);
struct ast* newbool(int value);
struct ast* newpair(int raw_num, struct ast* value);
struct ast* newlist(struct ast* now, struct ast* next);
struct ast* newfilterlist(struct ast* now, int op, struct ast* next);
struct ast* newinsert(char* table_num, struct ast* list_ast);
struct ast* newforast(char* varname, int table_num);

struct ast* newfilterpair(struct ast* left_oper, struct ast* sign, struct ast* right_oper);
struct ast* newtablefraw(char* varname, int raw_num);
struct ast* newsign(char* sign);
struct ast* newreturn(struct ast* sign);
struct ast* newremove(struct ast* sign);
struct ast* newupdate(char* name, struct ast* list);
struct ast* newforquery(struct ast* for_part, struct ast* filter_part, struct ast* conclusion_part);

struct ast* newselect(char* name, struct ast* filter_part);
struct ast* newjoin(char* name1, char* name2, int column1, int column2, struct ast* filter_part);
struct ast* newdelete(char* name, struct ast* filter_part);


struct ast* create_tableAst(char* table_num, int raw_size);



void print_ast(FILE* file, struct ast* ast, int level);
void free_ast(struct ast* ast);





