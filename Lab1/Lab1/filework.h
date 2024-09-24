#pragma once
#include <windows.h>
#include "classes.h"
#include "GetSet.h"
#define list_size 2048
#define windows_page_size 65536
#define help_info_service 1 // page where contains some tech info, like how many pages in the file
#define table_block_start 2
#define free_block_start 3
#define string_block_start 4
#define cellsOnList list_size/sizeof(struct cell) - 2 // last two are prev, next
#define tableStructOnList (cellsOnList) * sizeof(struct cell) / sizeof(struct table_data) // 
#define charsOnList ((cellsOnList) * sizeof(struct cell) / sizeof(char))

struct FileMapping {
	HANDLE hFile;
	HANDLE hMapping;
	size_t lowSize;
	size_t highSize;
};

struct map_file_of_view {
	char* start;
	char* my_page_start;
};

struct FileMapping* openMapping(char* fileName);

void initFile(char* fileName);

void updateMapping(struct FileMapping* oldMaping);

void freeMapping(struct FileMapping* Maping);
void addNewListToFileFromFile(HANDLE hFile);
void addNewListFromMapping(struct FileMapping* Maping);

int registerFreePage(struct FileMapping* Maping, int num);
int getFreePage(struct FileMapping* Maping);
void freeMyPage(struct map_file_of_view ret);
int sequenceDeleter(struct FileMapping* a, int num);

int pageCompresser(struct FileMapping* Maping, int page_num);

int rawDeleter(struct FileMapping* a, struct table_data* from_delete, int num_page, int cell_num, int raw_len);

void openMyPage(struct FileMapping* a, int num, struct map_file_of_view* ret);
void setMyPageFreeWithCells(struct FileMapping* a, int num);
void setMyPageFreeWithTableData(struct FileMapping* a, int num);
void setMyPageFreeWithCharData(struct FileMapping* a, int num);

int countFreeCellsOnPage(struct FileMapping* a, int page_num);

int getFirstFreeCell(struct FileMapping* Maping, int page_num);

int countCellsInRaw(struct cell raw[], int el_in_array);

// óêàçûâàåì çàïîëíèòü íà÷èíàÿ ñ êàêîãî-òî íîìåðà ÿ÷åéêè, òàêîé-òî ñòðàíèöû â ôàéëå íàøè äàííûå
int fillerDataToPage(struct FileMapping* Maping, int page_num, int cell_num, struct cell raw[], int el_in_array);

int addStringToFile(struct FileMapping* Maping, struct string_ref* what_write, int* how_many_wrote);

int getStringFromFile(struct FileMapping* Maping, struct string_ref* to_read, int* how_many_read, char* ans);

int deleteStringFromFile(struct FileMapping* Maping, struct string_ref* to_delete, struct string_ref* in_what_page, int* how_many_deleten);