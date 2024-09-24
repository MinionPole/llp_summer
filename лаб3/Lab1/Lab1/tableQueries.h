#pragma once
#include "classes.h"
struct selectQueryStruct {
	struct FileMapping* Maping;
	struct table_data* from_select;
	struct queryCondition* conditions;
	int conditions_num;
	struct raw_sequence* ret;
};

struct JoinQueryStruct {
	struct FileMapping* Maping;
	
	struct table_data* table1_join;
	struct queryCondition* conditions1;
	int conditions_num1;
	
	struct table_data* table2_join;
	struct queryCondition* conditions2;
	int conditions_num2; 
	
	struct joinCond join_cond;
	struct raw_sequence* ret;
};