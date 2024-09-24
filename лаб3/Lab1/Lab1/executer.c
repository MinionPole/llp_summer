#include "executer.h"

char* execute_ast(struct FileMapping* Maping, struct ast* to_execute, int* ans_size) {
	char* ret = "wrong";
	if (to_execute == NULL) {
		return ret;
	}
	if (to_execute->nodetype == NT_DROP) {
		struct drop_ast* drop_a = (struct drop_ast*)to_execute;
		struct table_data temp;
		strncpy_s(temp.table_name, sizeof(temp.table_name) - 1, drop_a->num, strlen(drop_a->num));

		int flag = deleteTable(Maping, &temp, -1);
		if (flag) {
			ret = "error";
		}
		else {
			ret = "done";
		}
	}

	if (to_execute->nodetype == NT_CREATE) {
		struct create_table_ast* create_a = (struct create_table_ast*)to_execute;
		struct table_data temp;
		strncpy_s(temp.table_name, sizeof(temp.table_name) - 1, create_a->table_num, strlen(create_a->table_num));
		temp.row_size = create_a->raw_size;

		int flag = addTable(Maping, &temp);
		if (flag) {
			ret = "error";
		}
		else {
			ret = "done";
		}
	}

	if (to_execute->nodetype == NT_INSERT) {
		struct insert_ast* insert_a = (struct insert_ast*)to_execute;
		struct table_data temp;
		strncpy_s(temp.table_name, sizeof(temp.table_name) - 1, insert_a->table_num, strlen(insert_a->table_num));
		get_table_data(Maping, &temp, -1);
		struct cell* raw = malloc(sizeof(struct cell) * temp.row_size);
		struct list_ast* list_with_data = insert_a->list_ast;
		while (list_with_data != NULL) {
			struct pair_ast* pair = (struct pair_ast*)list_with_data->now;
			if (pair->value->nodetype == NT_INTVAL) {
				raw[pair->raw_num - 1].flag = INT_DATA;
				raw[pair->raw_num - 1].int_data = ((struct int_ast*)pair->value)->value;
			}
			if (pair->value->nodetype == NT_DOUBLEVAL) {
				raw[pair->raw_num - 1].flag = DOUBLE_DATA;
				raw[pair->raw_num - 1].double_data = ((struct double_ast*)pair->value)->value;
			}
			if (pair->value->nodetype == NT_STRINGVAL) {
				raw[pair->raw_num - 1].flag = STRING_DATA;
				raw[pair->raw_num - 1].string_link.string_data = ((struct string_ast*)pair->value)->value;
				raw[pair->raw_num - 1].string_link.str_len = strlen(((struct string_ast*)pair->value)->value);
			}
			list_with_data = list_with_data->next;
		}

		int adden = 0;
		add_raw(Maping, raw, temp.row_size, &temp, &adden);
		free(raw);
		ret = "ok";
	}

	if (to_execute->nodetype == NT_SELECT) {
		struct select_ast* select_a = (struct select_ast*)to_execute;

		struct selectQueryStruct selectQuery;
		selectQuery.ret = NULL;
		selectQuery.Maping = Maping;
		struct table_data temp;
		strncpy_s(temp.table_name, sizeof(temp.table_name) - 1, select_a->name, strlen(select_a->name));
		selectQuery.from_select = &temp;
		get_table_data(Maping, selectQuery.from_select, -1);
		selectQuery.conditions_num = 0;
		int ind_to_add = 0;
		get_conditions_size(select_a->name, &selectQuery.conditions_num, select_a->filter_part);
		selectQuery.conditions = malloc(sizeof(struct queryCondition) * selectQuery.conditions_num);
		get_conditions(select_a->name, selectQuery.conditions, select_a->filter_part, &ind_to_add);

		selectFromTable(&selectQuery);
		ret = data2xml(selectQuery.ret);

		free(selectQuery.conditions);
	}

	if (to_execute->nodetype == NT_JOIN) {
		struct join_ast* join_a = (struct join_ast*)to_execute;

		struct JoinQueryStruct joinq;

		joinq.join_cond.first_column = join_a->num1 - 1;
		joinq.join_cond.second_column = join_a->num2 - 1;
		
		joinq.ret = NULL;
		joinq.Maping = Maping;
		struct table_data temp1;
		
		strncpy_s(temp1.table_name, sizeof(temp1.table_name) - 1, join_a->name1, strlen(join_a->name1));
		joinq.table1_join = &temp1;

		struct table_data temp2;
		strncpy_s(temp2.table_name, sizeof(temp2.table_name) - 1, join_a->name2, strlen(join_a->name2));
		joinq.table2_join = &temp2;

		get_table_data(Maping, joinq.table1_join, -1);
		get_table_data(Maping, joinq.table2_join, -1);
		joinq.conditions_num1 = 0;
		joinq.conditions_num2 = 0;
		int start1 = 0;
		int start2 = 0;
		get_conditions_size(join_a->name1, &joinq.conditions_num1, join_a->filter_part);
		joinq.conditions1 = malloc(sizeof(struct queryCondition) * joinq.conditions_num1);
		get_conditions(join_a->name1, joinq.conditions1, join_a->filter_part, &start1);

		get_conditions_size(join_a->name2, &joinq.conditions_num2, join_a->filter_part);
		joinq.conditions2 = malloc(sizeof(struct queryCondition) * joinq.conditions_num2);
		get_conditions(join_a->name2, joinq.conditions2, join_a->filter_part, &start2);


		joinTable(&joinq);
		ret = data2xml(joinq.ret);

		free(joinq.conditions1);
		free(joinq.conditions2);
	}

	if (to_execute->nodetype == NT_JOIN) {

	}

	if (to_execute->nodetype == NT_DELETE) {

	}
	*ans_size = strlen(ret);
	return ret;
}

void get_conditions_size(char* table_name, int* conditions_num, struct ast* filter_ast) {
	if (filter_ast == NULL) {
		return;
	}
	else {
		struct filter_list_ast* temp = (struct filter_list_ast*)filter_ast;
		get_conditions_size(table_name, conditions_num, temp->next);
		if (temp->now->nodetype == NT_FILTER_LIST)
			get_conditions_size(table_name, conditions_num, temp->now);
		else
			if (!strcmp( ( (struct tablef_raw_ast*)(((struct filter_pair_ast*)temp->now)->left_oper) )->varname, table_name))
				(*conditions_num) = (*conditions_num) + 1;
	}
}

void set_sign(struct queryCondition* condition, struct sign_ast* sign) {
	if (!strcmp(sign->sign, ">"))
		condition->conds = GREATER;
	if (!strcmp(sign->sign, "<"))
		condition->conds = LESS;
	if (!strcmp(sign->sign, "=="))
		condition->conds = EQUAL;
	if (!strcmp(sign->sign, "!="))
		condition->conds = NOT_EQUAL;
}

void set_value(struct queryCondition* condition, struct ast* sign) {
	if (sign->nodetype == NT_INTVAL) {
		condition->type = INT_DATA;
		condition->int_data = ((struct int_ast*)sign)->value;
	}
	if (sign->nodetype == NT_DOUBLEVAL) {
		condition->type = DOUBLE_DATA;
		condition->double_data = ((struct double_ast*)sign)->value;
	}
	if (sign->nodetype == NT_STRINGVAL) {
		condition->type = STRING_DATA;
		condition->string_data = ((struct string_ast*)sign)->value;
	}
	
}

void get_conditions(char* table_name, struct queryCondition* conditions, struct ast* filter_ast, int* cond_num) {
	if (filter_ast == NULL) {
	}
	else {
		struct filter_list_ast* temp = (struct filter_list_ast*)filter_ast;
		get_conditions(table_name, conditions, temp->next, cond_num);
		if(temp->now->nodetype == NT_FILTER_LIST)
			get_conditions(table_name, conditions, temp->now, cond_num);
		else
		if (!strcmp(((struct tablef_raw_ast*)(((struct filter_pair_ast*)temp->now)->left_oper))->varname, table_name)) {
			struct filter_pair_ast* pair = (struct filter_pair_ast*)temp->now;
			set_sign(&conditions[*cond_num], pair->sign);
			set_value(&conditions[*cond_num], pair->right_oper);
			conditions[*cond_num].stolbec_num = ((struct tablef_raw_ast*)(((struct filter_pair_ast*)temp->now)->left_oper))->raw_num - 1;
			(*cond_num)++;
		}
	}
}