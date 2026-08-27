#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void *element;
    int element_type;
} Lelement;

struct List {
    int size;
    int capacity;
    Lelement **tab;
};

static int l_init(List *list) {
    if (!list) return 0;
    list->size = 0;
    list->capacity = 4;
    list->tab = malloc(list->capacity * sizeof(Lelement *));
    if (!(list->tab)) return 0;
    return 1;
}

List *l_new() {
    List *list = malloc(sizeof(List));
    if (!list) return NULL;

    if (!l_init(list)) {
        free(list);
        return NULL;
    }
    return list;
}

static Lelement *create_lelement(void *element, int element_type) {
    Lelement *new_l_element = malloc(sizeof(Lelement));

    if (!new_l_element) return NULL;

    new_l_element->element_type = element_type;

    new_l_element->element = element;
    return new_l_element;
}

static int adapt_capacity(List *list) {
    // à appeler au début des fonctions
    int new_capacity;
    Lelement **new_tab;
    if (!list) return 0;

    new_capacity = list->capacity;

    if (list->size == list->capacity) new_capacity = list->capacity * 2;
    else if (list->size <= list->capacity / 4 && list->capacity / 2 >= 4) new_capacity = list->capacity / 2;

    if (new_capacity != list->capacity) {
        new_tab = realloc(list->tab, new_capacity * sizeof(Lelement *));
        if (!new_tab) return 0;
        list->tab = new_tab;
    }
    list->capacity = new_capacity;
    return 1;
}

static int delete_element(List *list, Lelement *l_element, int element_index, void (*destructor)(void *element_del, int element_type)) {
    // note : cette fonction met à jour la liste en plus de libérer l'élément
    int i;
    if (!list || !l_element) return 0;
    if (!adapt_capacity(list)) return 0;

    // on supprime l'élément stocké
    if (destructor) destructor(l_element->element, l_element->element_type);

    // on décale tous les éléments
    for (i=element_index + 1; i<list->size; i++) {
        list->tab[i - 1] = list->tab[i];
    }
    
    // on supprime le Lelement
    free(l_element);

    list->size--;
    return 1;
}

int l_append(List *list, void *element, int element_type) {
    if (!list) return 0;
    if (!adapt_capacity(list)) return 0;
    Lelement *new_l_element = create_lelement(element, element_type);

    if (!new_l_element) return 0;

    list->tab[list->size] = new_l_element;
    list->size++;

    return 1;
}

int l_set(List *list, int index, void *element, int element_type) {
    if (!list || index >= list->size) return 0;
    if (index < 0 && list->size == 0) return 0;
    while (index < 0) index = list->size+index;

    list->tab[index]->element = element;
    list->tab[index]->element_type = element_type;
    return 1;
}

int l_insert(List *list, int index, void *element, int element_type) {
    int i;
    if (!list) return 0;
    if (!adapt_capacity(list)) return 0;

    if (index >= list->size) index = list->size;
    if (index < 0 && list->size == 0) index = 0;
    while (index < 0) index = list->size+index;
    
    Lelement *new_l_element = create_lelement(element, element_type);
    if (!new_l_element) return 0;

    // on décale tout vers la droite
    for (i=list->size; i>index; i--) {
        list->tab[i] = list->tab[i - 1];
    }

    // on met le nouvel élément dans le tableau
    list->tab[index] = new_l_element;
    list->size++;
    return 1;

}

int l_from_table(List *list, void *table, int table_size, size_t element_size, int element_type) {
    if (!table || !list || list->size > 0) return 0;
    int i;

    // l_init(list); // ne surtout pas mettre, car sinon list->tab est remplacé sans être libéré

    for (i=0; i < table_size; i++) {
        // (char *) obligatoire (type à 1 octet) car void souvent pas compatible avec arithmétique
        if (!l_append(list, (char *)table + i * element_size, element_type)) {
            l_clear(list, NULL);
            return 0;
        }
    }
    return 1;
}

void l_print(List *list, void (*print)(void *element_print, int element_type)) {
    int i;
    if (!list) {
        printf("NULL list");
        return;
    }
    printf("BEGIN\n");

    if (print) {
        for (i=0; i<list->size;i++) {
            print(list->tab[i]->element, list->tab[i]->element_type);
        }
    }

    printf("END\n");
}

int l_pop(List *list, int index, void (*destructor)(void *element_del, int element_type)) {
    if (!list || list->size == 0) return 0;
    if (index >= list->size) return 0;
    if (index < 0 && list->size == 0) index = 0;
    while (index < 0) index = list->size+index;

    if (!delete_element(list, list->tab[index], index, destructor)) return 0;
    return 1;
}

int l_index(List *list, void *element) {
    // retourne -1 si non trouvé
    int i;
    if (!list || list->size == 0) return -1;

    for (i=0; i<list->size; i++) {
        if (list->tab[i]->element == element) return i;
    }
    return -1;
}

int l_reverse(List *list) {
    if (!list) return 0;
    int i=0, j=list->size - 1;
    Lelement *temp;

    while (i<j) {
        temp = list->tab[i];
        list->tab[i] = list->tab[j];
        list->tab[j] = temp;
        i++;
        j--;
    }
    return 1;
}

int l_remove(List *list, void *element, void (*destructor)(void *element_del, int element_type)) {
    if (!list || list->size == 0) return 0;
    int index = l_index(list, element);
    if (index < 0) return 0;
    return l_pop(list, index, destructor);
}

int l_clear(List *list, void (*destructor)(void *element_del, int element_type)) {
    int i, taille;
    if (!list) return 0;
    if (list->size == 0) return 1; // liste déjà vide

    taille = list->size;
    for (i=0; i<taille; i++) {
        if (!delete_element(list, list->tab[0], 0, destructor)) return 0;
    }
    return 1;
}

int l_free(List *list) {
    // non libérée si la liste n'est pas vide
    if (!list || list->size > 0) return 0;
    free(list->tab);
    free(list);
    return 1;
}

void *l_get(List *list, int index) {
    if (!list || list->size == 0) return NULL;
    if (index >= list->size) return NULL;
    if (index < 0 && list->size == 0) index = 0;
    while (index < 0) index = list->size+index;

    return list->tab[index]->element;
}

int l_get_type(List *list, int index) {
    if (!list || list->size == 0) return TYPE_ERROR;
    if (index >= list->size) return TYPE_ERROR;
    if (index < 0 && list->size == 0) index = 0;
    while (index < 0) index = list->size+index;
    return list->tab[index]->element_type;
}

int l_get_size(List *list) {
    if (!list) return 0;
    return list->size;
}

int l_sub(List *dest, List *src, int begin, int end) {
    int i;
    if (!dest || !src || dest->size != 0) return 0;

    if (begin >= src->size) begin = src->size;
    if (end >= src->size) end = src->size;

    if (begin < 0 && src->size == 0) begin = 0;
    while (begin < 0) begin = src->size+begin;
    if (end < 0 && src->size == 0) end = 0;
    while (end < 0) end = src->size+end;

    if (begin > end) return 0; // ne sert pas trop mais on met quand même au cas où

    for (i=begin; i<end; i++) {
        if (!l_append(dest, src->tab[i]->element, src->tab[i]->element_type)) {
            l_clear(dest, NULL);
            return 0;
        }
    }
    return 1;
}


int l_sort(List *list, int (*compare)(void *a, void *b, int type_a, int type_b)) {
    int i, j;
    Lelement *temp;
    if (!list || !compare) return 0;

    for (i=1; i<list->size; i++) {
        temp = list->tab[i];

        j = i;
        while (j > 0 && compare(list->tab[j-1]->element, temp->element, list->tab[j-1]->element_type, temp->element_type) > 0) {
            list->tab[j] = list-> tab[j-1];
            j--;
        }
        list->tab[j] = temp;
    }
    return 1;
}