#include "LinkedList.h"
#include <stdlib.h>

list_element *newElement(void *ptr) {
    // Allocates memory for the element
    list_element *item = (list_element *)malloc(sizeof(list_element));
    if (item == NULL) {
        return NULL;
    }

    item->ptr = ptr;

    // Item does not have neigbours yet
    item->next = NULL;
    item->prev = NULL;

    return item;
}

list *newList() {
    // Create List
    list *lst = (list *)malloc(sizeof(list));
    if (lst == NULL) {
        return NULL;
    }

    // List is empty
    lst->start = NULL;
    lst->end = NULL;
    lst->size = 0;

    return lst;
}

void deleteList(list *lst) {
    list_element *aux;

    if (lst == NULL) {
        return;
    }

    while (lst->start) {
        aux = lst->start;
        lst->start = lst->start->next;
        free(aux);
    }

    free(lst);
    return;
}

int listSize(list *lst) {
    if (lst == NULL) {
        return -1;
    }

    return lst->size;
}

list_element *listStart(list *lst) {
    if (lst == NULL) {
        return NULL;
    }

    return lst->start;
}

list_element *listEnd(list *lst) {
    if (lst == NULL) {
        return NULL;
    }

    return lst->end;
}

list_element *listElement(list *lst, unsigned int pos) {
    list_element *curr = NULL;

    if (lst == NULL || pos < 0 || pos >= lst->size) {
        return NULL;
    }

    curr = lst->start;
    for (int i = 0; i < pos; i++) {
        curr = curr->next;
    }

    return curr;
}

list_element *listInsert(list *lst, void *ptr, list_element *pos) {
    list_element *curr = NULL;

    if (lst == NULL || ptr == NULL) {
        return NULL;
    }

    // Create a new element
    curr = newElement(ptr);

    if (curr == NULL) {
        return NULL;
    }

    (lst->size)++;

    // Edge case: Inset at the end of the list
    if (pos == NULL) {
        // If it is the very first element
        if (lst->start == NULL) {
            lst->start = lst->end = curr;
        }
        else {
            curr->prev = lst->end;
            lst->end->next = curr;
            lst->end = curr;
        }
        return curr;
    }

    // Special case: Insert at the start of th list
    if (pos == lst->start) {
        curr->next = lst->start;
        lst->start->prev = curr;
        lst->start = curr;
        return curr;
    }

    // Swap pointers to insert element
    curr->prev = pos->prev;
    curr->next = pos;
    pos->prev->next = curr;
    pos->prev = curr;

    return curr;
}

list_element *listRemove(list *lst, list_element *pos) {
    list_element *aux;

    if (lst == NULL || pos == NULL) {
        return NULL;
    }

    (lst->size)--;

    // Swap pointers to remove element
    if (pos->prev != NULL) {
        pos->prev->next = pos->next;
    }
    else {
        // Is first
        lst->start = pos->next;
    }

    if (pos->next != NULL) {
        pos->next->prev = pos->prev;
    }
    else {
        // Is last
        lst->end = pos->prev;
    }

    // Free memory of the element
    aux = pos->next;
    free(pos);

    return aux;
}
