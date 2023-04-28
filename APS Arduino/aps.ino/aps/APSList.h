#pragma once
/// <summary>
/// Generic List that uses char* to represent pointers to any object you want. 
/// This class is not responsible for managing individual items' memory.
/// </summary>
typedef struct APSList {
	void** elements;
	unsigned long length;
	unsigned long capacity;
} List;

List* List_Create();

/// <summary>
/// Append item to list.
/// </summary>
/// <param name="list"></param>
void List_Append(List* list, void* item);

/// <summary>
/// Insert an item at a specified index to the list. Index must be in the range [0, length] or behaviour is undefined.
/// </summary>
/// <param name="list"></param>
/// <param name="item"></param>
/// <param name="index"></param>
void List_Insert(List* list, void* item, unsigned int index);

/// <summary>
/// Removes an item at the specified index in the list. Index must be in the range [0, length) or behaviour is undefined.
/// </summary>
/// <param name="list"></param>
/// <param name="index"></param>
void* List_Remove(List* list, unsigned int index);

/// <summary>
/// (RemoveC - RemoveCopy) Removes an item if in the list. Does not do anything if item is NULL. Returns NULL if no matches are found.
/// </summary>
/// <param name="list"></param>
/// <param name="item"></param>
/// <returns></returns>
void* List_RemoveC(List* list, void* item);

/// <summary>
/// Removes the last item in the list.
/// </summary>
/// <param name="list"></param>
/// <returns></returns>
void* List_Pop(List* list);

/// <summary>
/// Frees all allocated memory for the list. DOES NOT FREE POINTERS IF THEY ARE STORED IN THE LIST.
/// </summary>
/// <param name="list"></param>
void List_Destroy(List* list);

unsigned long List_Length(List* list);


