#pragma once
#include "fileWork.h"

struct FileMapping* openMapping(char* fileName) {
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DWORD dwhighSize = 0;
	DWORD dwLowSize = GetFileSize(hFile, dwhighSize);
	if (dwLowSize == INVALID_FILE_SIZE) {
		CloseHandle(hFile);
		return NULL;
	}

	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
		0, 0, NULL);

	if (hMapping == NULL) {
		CloseHandle(hFile);
		return NULL;
	}

	struct FileMapping* mapping = (struct FileMapping*)malloc(sizeof(struct FileMapping));
	if (mapping == NULL) {
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return NULL;
	}

	mapping->hFile = hFile;
	mapping->hMapping = hMapping;
	mapping->lowSize = (size_t)dwLowSize;
	mapping->highSize = (size_t)dwhighSize;

	return mapping;
}

void updateMapping(struct FileMapping* oldMaping) {

	DWORD dwhighSize = 0;
	DWORD dwLowSize = GetFileSize(oldMaping->hFile, dwhighSize);
	if (dwLowSize == INVALID_FILE_SIZE) {
		CloseHandle(oldMaping->hFile);
		return NULL;
	}

	HANDLE hMapping = CreateFileMapping(oldMaping->hFile, NULL, PAGE_READWRITE,
		0, 0, NULL);

	if (hMapping == NULL) {
		CloseHandle(oldMaping);
		return NULL;
	}

	CloseHandle(oldMaping->hMapping);

	oldMaping->hMapping = hMapping;
	oldMaping->lowSize = (size_t)dwLowSize;
	oldMaping->highSize = (size_t)dwhighSize;
}

void freeMapping(struct FileMapping* Maping) {
	CloseHandle(Maping->hFile);
	CloseHandle(Maping->hMapping);
	free(Maping);
}

void initFile(char* fileName) {
	HANDLE created = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, NULL);
	addNewListToFileFromFile(created);
	addNewListToFileFromFile(created);
	addNewListToFileFromFile(created);
	addNewListToFileFromFile(created);
	CloseHandle(created);

	struct FileMapping* myMapping = openMapping(fileName);
	setMyPageFreeWithCells(myMapping, 1);
	setMyPageFreeWithTableData(myMapping, 2);
	setMyPageFreeWithCells(myMapping, 3);
	setMyPageFreeWithCharData(myMapping, 4);

	setHowManyPages(myMapping, 4);
	setTablePageFinal(myMapping, 2);
	setFreeListPageFinal(myMapping, 3);
	struct string_ref ref;
	ref.page_num = 4;
	ref.char_num = 0;

	setStringPageFinal(myMapping, &ref);
	freeMapping(myMapping);
}

void openMyPage(struct FileMapping* a, int num, struct map_file_of_view* ret) {
	size_t start = (num - 1) * list_size / windows_page_size;
	size_t delta = (num - 1) * list_size - start * windows_page_size;
	ret->start = MapViewOfFile(a->hMapping, FILE_MAP_ALL_ACCESS, 0, start * windows_page_size, delta + list_size);
	DWORD temp = GetLastError();
	ret->my_page_start = (ret->start + delta);
}

void freeMyPage(struct map_file_of_view ret) {
	UnmapViewOfFile(ret.start);
}


void addNewListToFileFromFile(HANDLE hFile) {
	char newList[list_size];
	memset(newList, '0', sizeof(newList));

	LPDWORD written = 0;
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, &newList, sizeof(char) * list_size, written, NULL);
}


void addNewListFromMapping(struct FileMapping* Maping) {
	addNewListToFileFromFile(Maping->hFile);
	updateMapping(Maping);
}

void addNewRegisteredListFromMapping(struct FileMapping* Maping) {
	addNewListFromMapping(Maping);
	int new_list_num = getHowManyPages(Maping) + 1;
	registerFreePage(Maping, new_list_num);
	setHowManyPages(Maping, new_list_num);
	setMyPageFreeWithCells(Maping, new_list_num);
}


int registerFreePage(struct FileMapping* Maping, int num) {
	struct map_file_of_view page;
	int end_page = getFreeListPageFinal(Maping);
	openMyPage(Maping, end_page, &page);
	struct cell* now = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		if (now->flag == EMPTY) {
			now->flag = FREE_PAGE_DATA;
			now->int_data = num;
			return 0; // success
		}
		now++;
	}
	
	int new_list_num = getFreePage(Maping);
	setNextPageCell(&page, new_list_num);
	freeMyPage(page);
	openMyPage(Maping, new_list_num, &page);
	setPreviousPageCell(&page, end_page);
	freeMyPage(page);
	setFreeListPageFinal(Maping, new_list_num);
	return registerFreePage(Maping, num);
}

void setMyPageFreeWithCells(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	struct cell* empty = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		empty->flag = EMPTY;
		empty++;
	}

	empty->flag = PREVIOUS_PAGE;
	empty->int_data = -1;
	empty++;

	empty->flag = NEXT_PAGE;
	empty->int_data = -1;

	freeMyPage(page);
}

void setMyPageFreeWithCharData(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	char* empty = (char*)page.my_page_start;
	for (int i = 0; i < (charsOnList); i++) {
		*(empty) = '\0';
		empty++;
	}
	setNextPageCell(&page, -1);
	setPreviousPageCell(&page, -1);
	freeMyPage(page);
}

void setMyPageFreeWithTableData(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	struct table_data* empty = (struct table_data*)page.my_page_start;
	for (int i = 0; i < tableStructOnList; i++) {
		empty->table_flag = 0;
		empty++;
	}
	setNextPageCell(&page, -1);
	setPreviousPageCell(&page, -1);
	freeMyPage(page);
}

int sequenceDeleter(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	int next_page = getNextPageCell(&page);
	freeMyPage(page);
	setMyPageFreeWithCells(a, num);
	registerFreePage(a, num);
	if (next_page != -1) {
		return sequenceDeleter(a, next_page);
	}
	return 0;
}

int isMyPageFreeWithCells(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	int flag = 1;
	struct cell* empty = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		if (empty->flag != EMPTY) {
			flag = 0;
			break;
		}
		empty++;
	}
	freeMyPage(page);
	return flag;
}

int rawDeleter(struct FileMapping* a, struct table_data* from_delete, int num_page, int cell_num, int raw_len) {
	struct map_file_of_view page;
	openMyPage(a, num_page, &page);
	struct cell* my_cell = (struct cell*)page.my_page_start;
	my_cell += cell_num;
	for (int i = cell_num; i < cellsOnList && raw_len != 0; i++) {
		my_cell->flag = EMPTY;
		raw_len--;
		my_cell++;
	}

	if (raw_len != 0) {
		int next = getNextPageCell(&page);
		rawDeleter(a, from_delete, next, 0, raw_len);
	}
	if (isMyPageFreeWithCells(a, num_page) == 1) {
		if (getPreviousPageCell(&page) != -1 && getNextPageCell(&page) != -1) { // не первая и не последняя в таблице
			// сжать надо
			int prev_page = getPreviousPageCell(&page);
			int next_page = getNextPageCell(&page);
			freeMyPage(page);

			openMyPage(a, prev_page, &page);
			setNextPageCell(&page, next_page);
			freeMyPage(page);

			openMyPage(a, next_page, &page);
			setPreviousPageCell(&page, prev_page);

			setMyPageFreeWithCells(a, num_page);
			registerFreePage(a, num_page);
		}
	}
	freeMyPage(page);
	return 0;
}



int getFreePage(struct FileMapping* Maping) {
	int ret = -1;
	// поискать в старых
	struct map_file_of_view page;
	int final_page = getFreeListPageFinal(Maping);
	openMyPage(Maping, final_page, &page);
	struct cell* my_cell = (struct cell*)page.my_page_start;
	my_cell += (cellsOnList - 1);
	for (int i = cellsOnList - 1; i >= 0; i--) {
		if (my_cell->flag == FREE_PAGE_DATA) {
			my_cell->flag = EMPTY;
			ret = my_cell->int_data;
			freeMyPage(page);
			break;
		}
		my_cell--;
	}


	if (ret == -1) {
		if (getPreviousPageCell(&page) == -1) { // у нас просто нет свободных листов
			addNewRegisteredListFromMapping(Maping);
			ret = getFreePage(Maping);
		}
		else {
			// нынешняя страница пуста, обновим конец нашего списка, а нашу же страницу скинем как пустую
			int page_before_final = getPreviousPageCell(&page);
			freeMyPage(page);
			setMyPageFreeWithCells(Maping, final_page);
			ret = final_page;

			setFreeListPageFinal(Maping, page_before_final);
			openMyPage(Maping, page_before_final, &page);
			setNextPageCell(&page, -1);
		}
		freeMyPage(page);


	}
	return ret;
}


int fillWithStringData(struct FileMapping* Maping, struct string_ref* what_write, struct string_ref* where_write, int* how_many_wrote) {
	struct map_file_of_view page;
	openMyPage(Maping, where_write->page_num, &page);
	char* my_char = (char*)page.my_page_start;
	my_char += where_write->char_num;
	int ret = 0;
	for (int i = where_write->char_num; i < charsOnList && (*how_many_wrote) < what_write->str_len; i++) {
		(*my_char) = what_write->string_data[(*how_many_wrote)];
		(*how_many_wrote) = (*how_many_wrote) + 1;
		my_char++;
	}
	if ((*how_many_wrote) != what_write->str_len)
		ret = -1;
	freeMyPage(page);
	return ret;
}

int addStringToFile(struct FileMapping* Maping, struct string_ref* what_write, int* how_many_wrote) {
	int ret = 0;
	struct map_file_of_view page;
	struct string_ref end_page;
	getStringPageFinal(Maping, &end_page);
	int res = 0;
	what_write->page_num = end_page.page_num;
	what_write->char_num = end_page.char_num;
	int how_many_wrote_already = *how_many_wrote;
	res = fillWithStringData(Maping, what_write, &end_page, how_many_wrote);
	if (res == -1 || (end_page.char_num + (what_write->str_len - (how_many_wrote_already))) == charsOnList) {
		int new_list_num = getFreePage(Maping);
		openMyPage(Maping, end_page.page_num, &page);
		setNextPageCell(&page, new_list_num);
		freeMyPage(page);
		setMyPageFreeWithCharData(Maping, new_list_num);
		openMyPage(Maping, new_list_num, &page);
		setPreviousPageCell(&page, end_page.page_num);
		freeMyPage(page);
		struct string_ref last_page_info;
		last_page_info.page_num = new_list_num;
		last_page_info.char_num = 0;
		setStringPageFinal(Maping, &last_page_info);
		return addStringToFile(Maping, what_write, how_many_wrote);
	}
	else {
		struct string_ref last_page_info;
		last_page_info.page_num = end_page.page_num;
		last_page_info.char_num = end_page.char_num + (what_write->str_len - (how_many_wrote_already));
		setStringPageFinal(Maping, &last_page_info);
	}
	return ret;
}

int readWithStringData(struct FileMapping* Maping, struct string_ref* to_read, int* how_many_read, char* ans) {
	struct map_file_of_view page;
	openMyPage(Maping, to_read->page_num, &page);
	char* my_char = (char*)page.my_page_start;
	my_char += to_read->char_num;
	int ret = 0;
	for (int i = to_read->char_num; i < charsOnList && (*how_many_read) < to_read->str_len; i++) {
		ans[(*how_many_read)] = (*my_char);
		(*how_many_read) = (*how_many_read) + 1;
		my_char++;
	}
	if ((*how_many_read) != to_read->str_len)
		ret = -1;
	return ret;
}

int getStringFromFile(struct FileMapping* Maping, struct string_ref* to_read, int* how_many_read, char* ans) {
	int res = readWithStringData(Maping, to_read, how_many_read, ans);
	if (res == -1) {
		struct map_file_of_view page;
		openMyPage(Maping, to_read->page_num, &page);
		struct string_ref new_to_read;
		new_to_read.page_num = getNextPageCell(&page);
		new_to_read.char_num = 0;
		new_to_read.str_len = to_read->str_len;
		freeMyPage(page);
		getStringFromFile(Maping, &new_to_read, how_many_read, ans);
	}
	return 0;
}

int deleteWithStringData(struct FileMapping* Maping, struct string_ref* what_delete, struct string_ref* where_delete, int* how_many_deleten) {
	struct map_file_of_view page;
	openMyPage(Maping, where_delete->page_num, &page);
	char* my_char = (char*)page.my_page_start;
	my_char += where_delete->char_num;
	int ret = 0;
	for (int i = where_delete->char_num; i < charsOnList && how_many_deleten < what_delete->str_len; i++) {
		(*my_char) = '\0';
		(*how_many_deleten) = (*how_many_deleten) + 1;
	}
	if ((*how_many_deleten) != what_delete->str_len)
		ret = -1;
	return ret;
}

int deleteStringFromFile(struct FileMapping* Maping, struct string_ref* to_delete, struct string_ref* in_what_page, int* how_many_deleten) {
	int ret = deleteWithStringData(Maping, to_delete, in_what_page, how_many_deleten);
	int now_page_num = in_what_page->page_num;
	if (ret == -1) { // удалили не всё
		struct map_file_of_view page;
		openMyPage(Maping, now_page_num, &page);
		in_what_page->page_num = getNextPageCell(&page);
		in_what_page->char_num = 0;
		freeMyPage(page);
		deleteStringFromFile(Maping, to_delete, in_what_page, how_many_deleten);
		if (now_page_num != to_delete->page_num) {
		// нужно сделать страницу свободной
			setMyPageFreeWithCells(Maping, now_page_num);
			registerFreePage(Maping, now_page_num);
		}
	}
	else
		if (now_page_num != to_delete->page_num) {
			// если наша конечная страница не равна начальной, значит нужно поддержать двусвязный список
			struct map_file_of_view page;
			openMyPage(Maping, now_page_num, &page);
			setPreviousPageCell(&page, to_delete->page_num);
			freeMyPage(page);

			openMyPage(Maping, to_delete->page_num, &page);
			setNextPageCell(&page, now_page_num);
			freeMyPage(page);
		}
	return 0;
}

int getFirstFreeCell(struct FileMapping* Maping, int page_num) {
	struct map_file_of_view page;
	openMyPage(Maping, page_num, &page);
	struct cell* now = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		if (now->flag == EMPTY) {
			freeMyPage(page);
			return i;
		}
		now++;
	}
	freeMyPage(page);
	return -1;
}