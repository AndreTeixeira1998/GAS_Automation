#ifndef __LINKED_LIST__
#define __LINKED_LIST__

typedef struct _list_element {
	/* stored pointer */
	void* ptr;

	/* pointers to neighbour elements */
    struct _list_element *prev;
    struct _list_element *next;
} list_element;

typedef struct {
	list_element *start;
	list_element *end;
	int size;
} list;

#define LIST_START 0
#define LIST_END 1

#define LL_iterator(list, elem_name) for(list_element* elem_name = listStart(list); elem_name != NULL; elem_name = elem_name->next)

/**
 * @brief Creates a new LinkedList
 * 
 * @return list* Pointer to list. NULL if error.
 */
list* newList ();

/**
 * @brief Deletes a list, releasing all reserved memory
 * 
 * @param lst Pointer to list
 */
void deleteList(list* lst);

/**
 * @brief Indicates the size of the List
 * 
 * @param lst Pointer to the List
 * @return int Number of elements on the list. -1 if list is NULL.
 */
int listSize(list* lst);

/**
 * @brief Returns the first element of the List
 * 
 * @param lst Pointer to the List
 * @return list_element* Pointer to the first element of the list. NULL if list = NULL
 */
list_element* listStart(list* lst);

/**
 * @brief Returns the last element of the List
 * 
 * @param lst Pointer to the List
 * @return list_element* Pointer to the last element of the list. NULL if list = NULL
 */
list_element* listEnd(list* lst);

/**
 * @brief Returns the element stored in the specified position
 * 
 * @param lst Pointer to the List
 * @param pos Position of the element
 * @return list_element* Pointer to the requested element. NULL if invalid
 */
list_element* listElement(list* lst, unsigned int pos);

/**
 * @brief Inserts a element onto the List
 * 
 * @param lst Pointer to the List
 * @param ptr Pointer to insert
 * @param pos Position to insert the element in. If NULL, inserts in the last position.
 * @return list_element* Pointer to new element, or NULL if error.
 */
list_element* listInsert(list* lst, void* ptr, list_element* pos);

/**
 * @brief Removes the specified element
 * 
 * @param lst Pointer to the List 
 * @param pos Element to remove
 * @return list_element* Pointer to the element next to the one being removed. NULL if error.
 */
list_element* listRemove(list* lst, list_element* pos);

#endif