#pragma once
#include "ast.h"
#include "xml.h"
#include "tablework.h"

char* execute_ast(struct FileMapping* Maping, struct ast* to_execute, int* ans_size);

void get_conditions_size(char* table_name, int* conditions_num, struct ast* filter_ast);

void get_conditions(char* table_name, struct queryCondition* conditions, struct ast* filter_ast, int cond_num);