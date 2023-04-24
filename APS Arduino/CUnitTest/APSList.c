#include <stdlib.h>

#include "APSList.h"

/*	Helper Methods	*/
void ModList_Resize(List* list, unsigned long newLength)
{
	void** newBlock = malloc(sizeof(void*) * newLength);
	for (int i = 0; i < list->length; i++)
		newBlock[i] = list->elements[i];
	list->elements = newBlock;
	list->capacity = newLength;
}

List* List_Create()
{
	List* modlist = malloc(sizeof(List));
	if (!modlist) return NULL;
	modlist->elements = malloc(sizeof(void*) * 1);
	modlist->length = 0;
	modlist->capacity = 1;
	return modlist;
}

void List_Append(List* list, void* item)
{
	//	If the list is at full capacity, double the size of the list
	if (list->length >= list->capacity)
		ModList_Resize(list, list->capacity * 2);
	list->elements[list->length] = item;
	list->length++;
}
void List_Insert(List* list, void* item, int index)
{
	int i;
	//	If the list is at full capacity, double the size of the list
	if (list->length >= list->capacity)
		ModList_Resize(list, list->capacity * 2);

	if (index < 0 || index > list->length)
		return;
	for (i = list->length; i > index; i--)
		list->elements[i] = list->elements[i - 1];
	list->elements[index] = item;
	list->length++;
}
void* List_Remove(List* list, int index)
{
	int i;
	void* item = NULL;
	if (index < 0 || index >= list->length)
		return item;
	item = list->elements[index];
	for (i = index; i < list->length - 1; i++)
		list->elements[i] = list->elements[i + 1];
	list->length--;
	return item;
}
void* List_RemoveC(List* list, void* item)
{
	int i;
	void* listItem = NULL;
	if (item == NULL)
		return NULL;
	for (i = 0; i < list->length; i++)
	{
		if (item == list->elements[i])
		{
			listItem = item;
			break;
		}
	}
	if (listItem == NULL)
		return NULL;
	for (; i < list->length; i++)
		list->elements[i] = list->elements[i + 1];
	list->length--;
	return item;
}

void* List_Pop(List* list)
{
	return List_Remove(list, list->length - 1);
}
void List_Destroy(List* list)
{
	free(list->elements);
	free(list);
}
unsigned long List_Length(List* list)
{
	return list->length;
}