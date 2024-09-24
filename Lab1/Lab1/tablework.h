#pragma once
#include "filework.h"
#include "tableQueries.h"
#include <string.h>
#include <stdio.h>


// table_work
int addTable(struct FileMapping* Maping, struct table_data* to_add);

int updateTable(struct FileMapping* Maping, struct table_data* old_table, struct table_data* new_table, int page_to_check);

int deleteTable(struct FileMapping* Maping, struct table_data* to_delete, int page_to_check);

int get_table_data(struct FileMapping* Maping, struct table_data* to_add, int page_to_check);

// raw_work
int add_raw(struct FileMapping* Maping, struct cell* addRaw, int RawLen, struct table_data* to_add, int* adden);

int selectFromTable(struct selectQueryStruct* selectQuery);

int deleteRaw(struct FileMapping* Maping, struct table_data* from_delete, struct queryCondition* conditions, int conditions_num);

int update_raw(struct FileMapping* Maping, struct table_data* to_add, struct queryCondition* conditions, int conditions_num, struct cell* newRaw, int* toZamena, int RawLen);

int joinTable(struct JoinQueryStruct* JoinQuery);