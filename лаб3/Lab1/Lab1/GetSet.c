#pragma once
#include "GetSet.h"
// get last cells
int getNextPageCell(struct map_file_of_view* page) {
	struct cell* ret = (struct cell*)page->my_page_start;
	ret += (cellsOnList + 1);
	return ret->int_data;
}
int getPreviousPageCell(struct map_file_of_view* page) {
	struct cell* ret = (struct cell*)page->my_page_start;
	ret += (cellsOnList);
	return ret->int_data;
}

// set last cells
void setNextPageCell(struct map_file_of_view* page, int num) {
	struct cell* ret = (struct cell*)page->my_page_start;
	ret += (cellsOnList + 1);
	ret->int_data = num;
}

void setPreviousPageCell(struct map_file_of_view* page, int num) {
	struct cell* ret = (struct cell*)page->my_page_start;
	ret += (cellsOnList);
	ret->int_data = num;
}

//lists_nums
//0 - pages
//1 - table
//2 - free
//3 - string

// pages
int getHowManyPages(struct FileMapping* a) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	int ret = now_cell->int_data;
	freeMyPage(page);
	return ret;
}

void setHowManyPages(struct FileMapping* a, int x) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell->int_data = x;
	freeMyPage(page);
}

// table
int getTablePageFinal(struct FileMapping* a) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell += 1;
	int ret = now_cell->int_data;
	freeMyPage(page);
	return ret;
}

void setTablePageFinal(struct FileMapping* a, int x) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell += 1;
	now_cell->int_data = x;
	freeMyPage(page);
}

// free
int getFreeListPageFinal(struct FileMapping* a) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell += 2;
	int ret = now_cell->int_data;
	freeMyPage(page);
	return ret;
}

void setFreeListPageFinal(struct FileMapping* a, int x) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell += 2;
	now_cell->int_data = x;
	freeMyPage(page);
}

// string
void getStringPageFinal(struct FileMapping* a, struct string_ref* ref) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell += 3;
	ref->page_num = now_cell->string_link.page_num;
	ref->char_num = now_cell->string_link.char_num;
	freeMyPage(page);
}

void setStringPageFinal(struct FileMapping* a, struct string_ref* ref) {
	struct map_file_of_view page;
	openMyPage(a, help_info_service, &page);
	struct cell* now_cell = (struct cell*)(page.my_page_start);
	now_cell += 3;
	now_cell->string_link.page_num = ref->page_num;
	now_cell->string_link.char_num = ref->char_num;
	freeMyPage(page);
}