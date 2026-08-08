#ifndef LIST_H
#define LIST_H
#include <stddef.h> // pour size_t

#define TYPE_UNDEFINED -1
#define TYPE_INTEGER 0
#define TYPE_DOUBLE 1
#define TYPE_FLOAT 2
#define TYPE_STRING 3

typedef struct Node Node;

typedef struct {
    int size;
    Node *head;
} List;

void l_init(List *list);
int l_from_table(List *list, void *table, int table_size, size_t element_size, int element_type, int do_copy);

int l_append(List *list, void *element, size_t element_size, int element_type);
int l_insert(List *list, int index, void *element, size_t element_size, int element_type);

int l_pop(List *list, int index, void (*destructor)(void *element_del, int element_type));
int l_remove(List *list, void *element, void (*destructor)(void *element_del, int element_type));

void l_print(List *list, void (*print)(void *element_print, int element_type));
void *l_get(List *list, int index);
int l_get_type(List *list, int index);
int l_get_size(List *list);

#endif