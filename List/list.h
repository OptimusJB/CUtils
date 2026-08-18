#ifndef LIST_H
#define LIST_H
#include <stddef.h> // pour size_t

#define TYPE_ERROR -2

#define TYPE_UNDEFINED -1
#define TYPE_INT 0
#define TYPE_DOUBLE 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 3
#define TYPE_STRING 4

typedef struct List List;

List *l_new();
int l_free(List *list);

int l_from_table(List *list, void *table, int table_size, size_t element_size, int element_type);

int l_insert(List *list, int index, void *element, int element_type);
int l_append(List *list, void *element, int element_type);
int l_set(List *list, int index, void *element, int element_type);

int l_pop(List *list, int index, void (*destructor)(void *element_del, int element_type));
int l_remove(List *list, void *element, void (*destructor)(void *element_del, int element_type));
int l_clear(List *list, void (*destructor)(void *element_del, int element_type));

int l_sub(List *dest, List *src, int begin, int end);

int l_reverse(List *list);
int l_sort(List *list, int (*compare)(void *a, void *b, int type_a, int type_b));

void l_print(List *list, void (*print)(void *element_print, int element_type));
void *l_get(List *list, int index);
int l_index(List *list, void *element);
int l_get_type(List *list, int index);
int l_get_size(List *list);

#endif