#pragma once
#define TABLE_NAME_MAX_SIZE 30

enum cell_flag
{
	STRING_LINK, // ссылка на строку
	STRING_DATA, // символ строки
	DOUBLE_DATA,
	INT_DATA,
	NEXT_PAGE,
	PREVIOUS_PAGE,
	LAST_PAGE,
	LAST_RAW_EL,
	EMPTY,
	FREE_PAGE_DATA
};

struct string_ref {
	char* string_data; // используется при внесении записи в таблицу
	int page_num;
	int char_num;
	int str_len;
};

struct raw_sequence {
	int page_num, cell_num;
	int len;
	struct cell* raw_data;
	struct raw_sequence* next;
};

struct cell {
	union {
		int int_data;
		double double_data;
		struct string_ref string_link;
	};
	enum cell_flag flag;
};

struct table_data {
	char table_name[TABLE_NAME_MAX_SIZE];
	int	last_page_num;
	int row_size;
	int rows_count;
	int table_flag; // 0 - nonactive, 1 - active
};

enum sign_cond {
	GREATER,
	LESS,
	NOT_EQUAL,
	EQUAL, 
	GREATER_EQUAL, // >=
	LESS_EQUAL // <=
};

struct queryCondition {
	int stolbec_num;
	enum sign_cond conds; // " >", " <", "!=", "==", ">=", "<="
	enum cell_flag type;
	union {
		int int_data;
		double double_data;
		char* string_data;
	};
};

struct joinCond {
	int first_column;
	int second_column;
};

