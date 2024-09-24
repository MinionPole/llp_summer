#pragma once
#include "fileWork.h"
int getNextPageCell(struct map_file_of_view* page); 
int getPreviousPageCell(struct map_file_of_view* page);

// set last cells
void setNextPageCell(struct map_file_of_view* page, int num);
void setPreviousPageCell(struct map_file_of_view* page, int num);

//lists_nums
//0 - pages
//1 - table
//2 - free
//3 - string

// pages
int getHowManyPages(struct FileMapping* a);

void setHowManyPages(struct FileMapping* a, int x);

// table
int getTablePageFinal(struct FileMapping* a);

void setTablePageFinal(struct FileMapping* a, int x);

// free
int getFreeListPageFinal(struct FileMapping* a);

void setFreeListPageFinal(struct FileMapping* a, int x);

// string
void getStringPageFinal(struct FileMapping* a, struct string_ref* ref);

void setStringPageFinal(struct FileMapping* a, struct string_ref* ref);