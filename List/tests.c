#include "list.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>


void print(void * element, int type) {
    if (type == TYPE_CHAR) printf("%c ", *((char *)element));
    else if (type == TYPE_INT) printf("%d ", *((int *)element));
}

int compare(void *a, void *b, int type_a, int type_b) {
    return *(int *)a-*(int *)b;
}

int run_tests() {
    List *list = l_new();
    int i;
    int *element;
    char tab[6] = {'s', 'a', 'l', 'u', 't', '\0'};
    int tab2[12] = {5, 7, 4, 3, 2, 0, 9, 8, -1, 1, 3, 6};
    int tab2_sorted[12] = {-1, 0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9};
    int tab2_removed[11] = {5, 7, 4, 3, 2, 9, 8, -1, 1, 3, 6};

    // test append
    for (i = 0; i < 10; i++) {
        l_append_copy(list, &i, sizeof(int), TYPE_INT);
        assert(l_get_size(list) == i + 1);
    }
    for (i = 0; i < 10; i++) {
        assert(*((int *)l_get(list, i)) == i);
        assert(l_get_type(list, i) == TYPE_INT);
    }

    // test insert
    i = 2;
    l_insert_copy(list, 0, &i, sizeof(int), TYPE_INT);
    assert(l_get_size(list) == 11);
    assert(l_get_type(list, 0) == TYPE_INT);
    assert(*(int *)l_get(list, 0) == 2);

    i = 54;
    l_insert_copy(list, 11, &i, sizeof(int), TYPE_INT);
    assert(l_get_size(list) == 12);
    assert(l_get_type(list, 11) == TYPE_INT);
    assert(*(int *)l_get(list, 11) == 54);

    i = 32;
    l_insert_copy(list, 5, &i, sizeof(int), TYPE_INT);
    assert(l_get_size(list) == 13);
    assert(l_get_type(list, 5) == TYPE_INT);
    assert(*(int *)l_get(list, 5) == 32);

    i = 90;
    l_insert_copy(list, -2, &i, sizeof(int), TYPE_INT);
    assert(l_get_size(list) == 14);
    assert(l_get_type(list, 11) == TYPE_INT);
    assert(*(int *)l_get(list, 11) == 90);

    i = 100;
    l_insert_copy(list, 50, &i, sizeof(int), TYPE_INT);
    assert(l_get_size(list) == 15);
    assert(l_get_type(list, 14) == TYPE_INT);
    assert(*(int *)l_get(list, 14) == 100);

    // test clear
    l_clear(list, NULL);
    assert(l_get_size(list) == 0);

    // test l_from_table
    l_from_table_copy(list, tab, 6, sizeof(char), TYPE_CHAR);
    for (i = 0; i < 6; i++) {
        assert(*(char *)l_get(list, i) == tab[i]);
    }

    // test sort
    l_clear(list, NULL);
    l_from_table_copy(list, tab2, 12, sizeof(int), TYPE_INT);
    l_sort(list, compare);

    for (i=0;i<12;i++) assert(*(int *)l_get(list, i) == tab2_sorted[i]);

    // test reverse
    l_reverse(list);
    for (i=0;i<12;i++) assert(*(int *)l_get(list, i) == tab2_sorted[11-i]);

    l_clear(list, NULL);
    l_reverse(list);
    assert(l_get_size(list) == 0);

    // test index
    l_from_table_copy(list, tab2, 12, sizeof(int), TYPE_INT);
    element = l_get(list, 5);
    assert(l_index(list, element) == 5);

    i = 15;
    element = &i;
    assert(l_index(list, element) == -1);

    l_clear(list, NULL);
    assert(l_index(list, element) == -1);

    // test remove
    l_from_table_copy(list, tab2, 12, sizeof(int), TYPE_INT);
    element = l_get(list, 5);
    l_remove(list, element, NULL);
    for (i=0;i<11;i++) assert(*(int *)l_get(list, i) == tab2_removed[i]);

    i = 15;
    element = &i;
    assert(l_remove(list, element, NULL) == 0);
    assert(l_get_size(list) == 11);

    // test set
    l_clear(list, NULL);
    l_from_table_copy(list, tab2, 12, sizeof(int), TYPE_INT);
    l_set_copy(list, 5, &tab[0], sizeof(char), TYPE_CHAR);
    assert(l_get_size(list) == 12);
    assert(*(char *)l_get(list, 5) == 's');

    assert(l_set_copy(list, 55, &tab[0], sizeof(char), TYPE_CHAR) == 0);

    l_set_copy(list, -2, &tab[1], sizeof(char), TYPE_CHAR);
    assert(l_get_size(list) == 12);
    assert(*(char *)l_get(list, 10) == 'a');

    printf("tests réussis\n");
    return 1;
}