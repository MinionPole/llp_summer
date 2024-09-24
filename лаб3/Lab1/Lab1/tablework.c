#pragma once
#include "tablework.h"

int addTable(struct FileMapping* Maping, struct table_data* to_add) {
	int ret = -1;
	struct map_file_of_view page;
	int finalTablePage = getTablePageFinal(Maping);
	openMyPage(Maping, finalTablePage, &page);

	struct table_data* now = (struct table_data*)page.my_page_start;
	for (int i = 0; i < tableStructOnList; i++) {
		if (now->table_flag == 0) {
			int first_table_list = getFreePage(Maping);
			now->last_page_num = first_table_list;
			now->rows_count = 0;
			now->row_size = to_add->row_size;
			now->table_flag = 1;
			for(int i = 0;i < TABLE_NAME_MAX_SIZE;i++)
				now->table_name[i] = to_add->table_name[i];
			return 0;
		}
		now++;
	}

	int new_list_num = getFreePage(Maping);
	setNextPageCell(&page, new_list_num);
	freeMyPage(page);
	setMyPageFreeWithTableData(Maping, new_list_num);
	openMyPage(Maping, new_list_num, &page);
	setPreviousPageCell(&page, finalTablePage);
	freeMyPage(page);
	setTablePageFinal(Maping, new_list_num);
	return addTable(Maping, to_add);
}

int tableNameChecker(char* name1, char* name2) {
	for (int i = 0; i < TABLE_NAME_MAX_SIZE; i++) {
		if (name1[i] != name2[i])
			return 0;
	}
	return 1;
}

int updateTable(struct FileMapping* Maping, struct table_data* old_table, struct table_data* new_table, int page_to_check) {
	if (page_to_check == -1) {
		page_to_check = getTablePageFinal(Maping);
	}
	struct map_file_of_view page;
	openMyPage(Maping, page_to_check, &page);

	struct table_data* now = (struct table_data*)page.my_page_start;
	for (int i = 0; i < tableStructOnList; i++) {
		if (tableNameChecker(now->table_name, old_table->table_name)) {
			now->last_page_num = new_table->last_page_num;
			now->rows_count = new_table->rows_count;
			freeMyPage(page);
			return 0;
		}
		now++;
	}

	int previous_page = getPreviousPageCell(&page);
	if(previous_page == -1){
		return 1;
	}
	else{
		freeMyPage(page);
		return updateTable(Maping, old_table, new_table, previous_page);
	}

}

int deleteTable(struct FileMapping* Maping, struct table_data* to_delete, int page_to_check) {
	if (page_to_check == -1) {
		page_to_check = getTablePageFinal(Maping);
	}
	struct map_file_of_view page;
	openMyPage(Maping, page_to_check, &page);

	struct table_data* now = (struct table_data*)page.my_page_start;
	for (int i = 0; i < tableStructOnList; i++) {
		if (tableNameChecker(now->table_name, to_delete->table_name)) {
			// удалить
			sequenceDeleter(Maping, now->last_page_num);
			now->last_page_num = -1;
			now->table_flag = 0;
			now->rows_count = to_delete->rows_count;
			freeMyPage(page);
			return 0;
		}
		now++;
	}

	int previous_page = getPreviousPageCell(&page);
	if (previous_page == -1) {
		return 1;
	}
	else {
		freeMyPage(page);
		return deleteTable(Maping, to_delete, previous_page);
	}
}

int get_table_data(struct FileMapping* Maping, struct table_data* to_add, int page_to_check) {
	if (page_to_check == -1) {
		page_to_check = getTablePageFinal(Maping);
	}
	struct map_file_of_view page;
	openMyPage(Maping, page_to_check, &page);

	struct table_data* now = (struct table_data*)page.my_page_start;
	for (int i = 0; i < tableStructOnList; i++) {
		if (tableNameChecker(now->table_name, to_add->table_name)) {
			// удалить
			to_add->rows_count = now->rows_count;
			to_add->row_size = now->row_size;
			to_add->table_flag = now->table_flag;
			to_add->last_page_num = now->last_page_num;
			freeMyPage(page);
			return 0;
		}
		now++;
	}

	int previous_page = getPreviousPageCell(&page);
	if (previous_page == -1) {
		return 1;
	}
	else {
		freeMyPage(page);
		return get_table_data(Maping, to_add, previous_page);
	}
}

int fillCellData(struct FileMapping* Maping, int page_num, int cell_num, struct cell* addRaw, int RawLen, int* adden) {
	struct map_file_of_view page;
	openMyPage(Maping, page_num, &page);
	struct cell* now = (struct cell*)page.my_page_start;
	now += cell_num;
	for (int i = cell_num; i < cellsOnList && (*adden) < RawLen; i++) {
		now->flag = addRaw[(*adden)].flag;
		if (now->flag == INT_DATA || now->flag == DOUBLE_DATA) {
			now->double_data = addRaw[(*adden)].double_data;
			now->int_data = addRaw[(*adden)].int_data;
		}
		if (now->flag == STRING_DATA) {
			int string_adden = 0;
			addStringToFile(Maping, &(addRaw[(*adden)].string_link), &string_adden);
			now->string_link.char_num = addRaw[(*adden)].string_link.char_num;
			now->string_link.page_num = addRaw[(*adden)].string_link.page_num;
			now->string_link.str_len = addRaw[(*adden)].string_link.str_len;
		}
		now++;
		(*adden) = (*adden) + 1;
	}
	freeMyPage(page);
	if ((*adden) < RawLen)
		return -1; // лист закончился - нужен новый.
	return 0;
}

int add_raw(struct FileMapping* Maping, struct cell* addRaw, int RawLen, struct table_data* to_add, int *adden) {
	struct map_file_of_view page;

	int last_cell = getFirstFreeCell(Maping, to_add->last_page_num);

	int res = 0;
	if (last_cell == -1 || fillCellData(Maping, to_add->last_page_num, last_cell, addRaw, RawLen, adden) == -1) {
	// заводим новую страницу
		int new_page = getFreePage(Maping);
		
		openMyPage(Maping, to_add->last_page_num, &page);
		setNextPageCell(&page, new_page);
		freeMyPage(page);

		openMyPage(Maping, new_page, &page);
		setPreviousPageCell(&page, to_add->last_page_num);
		freeMyPage(page);

		to_add->last_page_num = new_page;

		updateTable(Maping, to_add, to_add, -1);
		return add_raw(Maping, addRaw, RawLen, to_add, adden);
	}
	return 0;
}

struct raw_sequence* register_cell_to_raw_sequence(struct FileMapping* Maping, struct raw_sequence* ret, int* adden, int raw_size, struct cell* to_add, int page_num, int cell_num) {
	if (ret == NULL) {
		ret = malloc(sizeof(struct raw_sequence));
		ret->len = raw_size;
		ret->raw_data = malloc(sizeof(struct cell) * raw_size);
		ret->next = NULL;
		(*adden) = (raw_size - 1);
	}
	if ((*adden) == -1) {
		struct raw_sequence* new_raw = malloc(sizeof(struct raw_sequence));
		new_raw->len = raw_size;
		new_raw->raw_data = malloc(sizeof(struct cell) * raw_size);
		(*adden) = (raw_size - 1);
		new_raw->next = ret;
		ret = new_raw;
	}
	ret->raw_data[(*adden)].flag = to_add->flag;
	if (to_add->flag == INT_DATA || to_add->flag == DOUBLE_DATA) {
		ret->raw_data[(*adden)].double_data = to_add->double_data;
		ret->raw_data[(*adden)].int_data = to_add->int_data;
	}
	if (to_add->flag == STRING_DATA) {
		ret->raw_data[(*adden)].string_link.char_num = to_add->string_link.char_num;
		ret->raw_data[(*adden)].string_link.page_num = to_add->string_link.page_num;
		ret->raw_data[(*adden)].string_link.str_len = to_add->string_link.str_len;
		ret->raw_data[(*adden)].string_link.string_data = malloc(sizeof(char) * (to_add->string_link.str_len + 1));
		int readen_string = 0;
		getStringFromFile(Maping, &ret->raw_data[(*adden)].string_link, &readen_string, ret->raw_data[(*adden)].string_link.string_data);
		ret->raw_data[(*adden)].string_link.string_data[to_add->string_link.str_len] = '\0';
	}

	if ((*adden) == 0) {
		ret->page_num = page_num;
		ret->cell_num = cell_num;
	}
	(*adden)--;
	return ret;
}

struct raw_sequence* selectRaw(struct FileMapping* Maping, struct table_data* from_select, struct raw_sequence* ret) {
	int flag = 0;
	int page_num = from_select->last_page_num;
	struct map_file_of_view page;
	int adden = from_select->row_size;
	while (!flag) {
		openMyPage(Maping, page_num, &page);
		struct cell* now = (struct cell*)page.my_page_start;
		now += (cellsOnList - 1);
		for (int i = cellsOnList - 1; i >= 0; i--) {
			if (now->flag == INT_DATA || now->flag == STRING_DATA || now->flag == DOUBLE_DATA) {
				ret = register_cell_to_raw_sequence(Maping, ret, &adden, from_select->row_size, now, page_num, i);
			}
			now--;
		}
		int prev_page = getPreviousPageCell(&page);
		if (prev_page == -1) {
			flag = 1;
		}
		else
			page_num = prev_page;
		freeMyPage(page);
	}
	return ret;
}

int freeRawSequence(struct raw_sequence* ret) {
	for (int i = 0; i < ret->len; i++) {
		if (ret->raw_data[i].flag == STRING_DATA)
			free(ret->raw_data[i].string_link.string_data);
	}
	free(ret->raw_data);
	free(ret);
}

int freeFullRawSequence(struct raw_sequence* ret) {
	if (ret == NULL)
		return;
	freeFullRawSequence(ret->next);
	freeRawSequence(ret);
}

int printRawSequence(struct raw_sequence* ret) {
	for (int i = 0; i < ret->len; i++) {
		if (ret->raw_data[i].flag == INT_DATA)
			printf("%d ", ret->raw_data[i].int_data);

		if (ret->raw_data[i].flag == DOUBLE_DATA)
			printf("%f ", ret->raw_data[i].double_data);

		if (ret->raw_data[i].flag == STRING_DATA) {
			printf("%s ", ret->raw_data[i].string_link.string_data);
		}
	}
	printf("\n");
}

int printFullRawSequence(struct raw_sequence* ret) {
	if (ret == NULL)
		return;
	printFullRawSequence(ret->next);
	printRawSequence(ret);
}

int checkRawCond(struct FileMapping* Maping, struct queryCondition* condition, struct raw_sequence* ret) {
	if (condition->type != ret->raw_data[condition->stolbec_num].flag) {
		return 0;
	}
	if (condition->type == INT_DATA) {
		switch (condition->conds) {
		case GREATER: {return ret->raw_data[condition->stolbec_num].int_data > condition->int_data; break; }
		case LESS: {return ret->raw_data[condition->stolbec_num].int_data < condition->int_data; break; }
		case NOT_EQUAL: {return ret->raw_data[condition->stolbec_num].int_data == condition->int_data; break; }
		case EQUAL: {return ret->raw_data[condition->stolbec_num].int_data == condition->int_data; break; }
		default: return 0;
		}
	}

	if (condition->type == DOUBLE_DATA) {
		switch (condition->conds) {
		case GREATER: {return ret->raw_data[condition->stolbec_num].double_data > condition->double_data; break; }
		case LESS: {return ret->raw_data[condition->stolbec_num].double_data < condition->double_data; break; }
		case NOT_EQUAL: {return ret->raw_data[condition->stolbec_num].double_data == condition->double_data; break; }
		case EQUAL: {return ret->raw_data[condition->stolbec_num].double_data == condition->double_data; break; }
		default: {return 0; break; }
		}
	}

	if (condition->type == STRING_DATA) {
		int how_many_read = 0;
		//char* string_from_file = malloc(ret->raw_data[condition->stolbec_num].string_link.str_len * sizeof(char));
		//getStringFromFile(Maping, &ret->raw_data[condition->stolbec_num].string_link, &how_many_read, &string_from_file);
		int ret_data = 1;
		switch (condition->conds) {
		case EQUAL: {ret_data = !strcmp(ret->raw_data[condition->stolbec_num].string_link.string_data, condition->string_data); break; }
		case NOT_EQUAL:{
			ret_data = (strcmp(ret->raw_data[condition->stolbec_num].string_link.string_data, condition->string_data));
			break;
		}
		default: {ret_data = 0;  break; }
		}
		//free(string_from_file);
		return ret_data;
	}
	return 1;
}


struct raw_sequence* checkConditions(struct FileMapping* Maping, struct queryCondition* conditions, int conditions_num, struct raw_sequence* ret) {
	struct raw_sequence* previous_raw = NULL;
	struct raw_sequence* current_raw = ret;
	struct raw_sequence* next_raw = NULL;
	while (current_raw != NULL) {
		int flag = 1;
		next_raw = current_raw->next;
		// Проверить условия
		for (int i = 0; i < conditions_num; i++) {
			if (checkRawCond(Maping, &(conditions[i]), current_raw) == 0)
				flag = 0;
		}

		if (!flag) {
			// удалить raw из sequence
			if (previous_raw == NULL) {
				ret = next_raw;
				freeRawSequence(current_raw);
			}
			if (previous_raw != NULL) {
				previous_raw->next = next_raw;
				freeRawSequence(current_raw);
			}
		}
		else
			previous_raw = current_raw;
		current_raw = next_raw;
	}
	return ret;
}

int selectFromTable(struct selectQueryStruct *selectQuery) {
	selectQuery->ret = selectRaw(selectQuery->Maping, selectQuery->from_select, selectQuery->ret);
	selectQuery->ret = checkConditions(selectQuery->Maping, selectQuery->conditions, selectQuery->conditions_num, selectQuery->ret);
}

int deleter(struct FileMapping* Maping, struct table_data* from_delete, struct raw_sequence* ret) {
	if (ret == NULL)
		return 0;
	deleter(Maping, from_delete, ret->next);
	rawDeleter(Maping, from_delete, ret->page_num, ret->cell_num, ret->len);
}

int deleteRaw(struct FileMapping* Maping, struct table_data* from_delete, struct queryCondition* conditions, int conditions_num) {
	struct raw_sequence* ret = NULL;
	ret = selectRaw(Maping, from_delete, ret);
	ret = checkConditions(Maping, conditions, conditions_num, ret);
	deleter(Maping, from_delete, ret);
	freeFullRawSequence(ret);
}

int checkJoinEqual(struct cell* c1, struct cell* c2) {
	if (c1->flag != c2->flag)
		return 0;
	if (c1->flag == INT_DATA)
		return c1->int_data == c2->int_data;
	if (c1->flag == DOUBLE_DATA)
		return c1->double_data == c2->double_data;
	if (c1->string_link.string_data == "\'RATTC\'") {
		int a = 2;
		a++;
	}
	if (c1->flag == STRING_DATA)
		return !strcmp(c1->string_link.string_data, c2->string_link.string_data);
	return 0;
}



struct raw_sequence* joinTableIn(struct raw_sequence* ret1, struct raw_sequence* ret2, struct joinCond join_cond) {
	struct raw_sequence* ret1_cop = ret1;
	struct raw_sequence* ret2_cop = ret2;
	struct raw_sequence* ans = NULL;

	while (ret1_cop != NULL) {
		ret2_cop = ret2;
		while (ret2_cop != NULL) {
			if(checkJoinEqual(&ret1_cop->raw_data[join_cond.first_column], &ret2_cop->raw_data[join_cond.second_column])){
				struct raw_sequence* temp = malloc(sizeof(struct raw_sequence));
				temp->next = ans;
				temp->len = ret1->len + ret2_cop->len;
				temp->raw_data = malloc(sizeof(struct cell) * temp->len);
				for (int i = 0; i < ret1->len; i++) {
					temp->raw_data[i].flag = ret1_cop->raw_data[i].flag;
					temp->raw_data[i].int_data = ret1_cop->raw_data[i].int_data;
					temp->raw_data[i].double_data = ret1_cop->raw_data[i].double_data;
					temp->raw_data[i].string_link = ret1_cop->raw_data[i].string_link;
					/*if (temp->raw_data[i].flag == STRING_DATA) {
						temp->raw_data[i].string_link.string_data = malloc(sizeof(char) * ret1_cop->raw_data[i].string_link.str_len + 1);
						snprintf(temp->raw_data[i].string_link.string_data, sizeof(temp->raw_data[i].string_link.string_data), "%s", ret1_cop->raw_data[i].string_link.string_data);
					}*/
				}
				for (int i = 0; i < ret2_cop->len; i++) {
					temp->raw_data[ret1->len + i].flag = ret2_cop->raw_data[i].flag;
					temp->raw_data[ret1->len + i].int_data = ret2_cop->raw_data[i].int_data;
					temp->raw_data[ret1->len + i].double_data = ret2_cop->raw_data[i].double_data;
					temp->raw_data[ret1->len + i].string_link = ret2_cop->raw_data[i].string_link;
					/*if (temp->raw_data[ret1->len + i].flag == STRING_DATA) {
						temp->raw_data[ret1->len + i].string_link.string_data = malloc(sizeof(char) * ret2_cop->raw_data[i].string_link.str_len + 1);
						snprintf(temp->raw_data[ret1->len + i].string_link.string_data, sizeof(temp->raw_data[ret1->len + i].string_link.string_data), "%s", ret2_cop->raw_data[i].string_link.string_data);
					}*/
				}
				ans = temp;
			}
			ret2_cop = ret2_cop->next;
		}
		ret1_cop = ret1_cop->next;
	}
	return ans;
}

int joinTable(struct JoinQueryStruct* JoinQuery) {
	struct raw_sequence* ret1 = NULL;
	ret1 = selectRaw(JoinQuery->Maping, JoinQuery->table1_join, ret1);
	ret1 = checkConditions(JoinQuery->Maping, JoinQuery->conditions1, JoinQuery->conditions_num1, ret1);

	struct raw_sequence* ret2 = NULL;
	ret2 = selectRaw(JoinQuery->Maping, JoinQuery->table2_join, ret2);
	ret2 = checkConditions(JoinQuery->Maping, JoinQuery->conditions2, JoinQuery->conditions_num2, ret2);

	printFullRawSequence(ret1);
	printf("\n\n\n");
	printFullRawSequence(ret2);

	JoinQuery->ret = joinTableIn(ret1, ret2, JoinQuery->join_cond);
	//freeFullRawSequence(ret1);
	//freeFullRawSequence(ret2);
	return 0;
}