#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node{
    void *element;
    int type;
    Node *previous;
    Node *next;
};

void l_init(List *list) {
    list->size = 0;
    list->head = NULL;
}

Node *create_node(void *element, size_t element_size, int element_type) {
    void *new_element;
    Node *new_noeud = malloc(sizeof(Node));

    if (!new_noeud) return NULL;

    new_noeud->type = element_type;
    new_noeud->next = NULL;
    new_noeud->previous = NULL;

    // si element_size > 0, on copie l'élément, sinon on prend le même
    if (element_size == 0) {
        new_noeud->element = element;
    }
    else {
        new_element = malloc(element_size);
        if (!new_element) {
            free(new_noeud);
            return NULL;
        }

        memcpy(new_element, element, element_size);
        new_noeud->element = new_element;
    }
    return new_noeud;
}

int delete_node(List *list, Node *node, void (*destructor)(void *element_del, int element_type)) {
    Node *noeud_suivant;
    Node *noeud_precedent;
    // on supprime l'élément stocké
    if (destructor) destructor(node->element, node->type);

    // on change les pointeurs
    noeud_suivant = node->next;
    noeud_precedent = node->previous;

    if (noeud_precedent && !noeud_suivant) noeud_precedent->next = NULL;
    else if (!noeud_precedent && noeud_suivant) {
        noeud_suivant->previous = NULL;
        list->head = noeud_suivant;
    }
    else if (!noeud_precedent && !noeud_suivant) list->head = NULL;
    else if (noeud_precedent && noeud_suivant) {
        noeud_precedent->next = noeud_suivant;
        noeud_suivant->previous = noeud_precedent;
    }

    // on supprime le noeud
    free(node);

    list->size--;
    return 1;
}

int l_append(List *list, void *element, size_t element_size, int element_type) {
    if (!list) return 0;
    Node *new_noeud = create_node(element, element_size, element_type);
    Node *noeud_actuel = list->head;

    if (!new_noeud) return 0;

    if (!noeud_actuel) {
        list->head = new_noeud;
        list->size++;
        return 1;
    }

    // on ajoute le noeud à la fin de la liste
    while (noeud_actuel->next != NULL) {
        noeud_actuel = noeud_actuel->next;
    }
    noeud_actuel->next = new_noeud;
    new_noeud->previous = noeud_actuel;
    list->size++;

    return 1;
}

int l_insert(List *list, int index, void *element, size_t element_size, int element_type) {
    if (!list) return 0;
    Node *new_noeud = create_node(element, element_size, element_type);
    Node *noeud_actuel = list->head;
    Node *noeud_precedent;
    int index_actuel = 0;

    if (index < 0 && list->size == 0) index = list->size;
    while (index < 0) index = list->size+index;

    if (!new_noeud) return 0;

    if (!noeud_actuel) {
        list->head = new_noeud;
        list->size++;
        return 1;
    }

    // on va à l'index voulu
    while (noeud_actuel->next != NULL && index_actuel < index) {
        noeud_actuel = noeud_actuel->next;
        index_actuel++;
    }

    // cas où l'index demandé est trop élevé
    if (index_actuel != index) {
        noeud_actuel->next = new_noeud;
        new_noeud->previous = noeud_actuel;
        list->size++;
        return 1;
    }

    // cas où on met l'élément au début
    if (index == 0) {
        noeud_actuel->previous = new_noeud;
        new_noeud->next = noeud_actuel;
        list->head = new_noeud;
        list->size++;
        return 1;
    }

    noeud_precedent = noeud_actuel->previous;
    noeud_precedent->next = new_noeud;
    noeud_actuel->previous = new_noeud;

    new_noeud->previous = noeud_precedent;
    new_noeud->next = noeud_actuel;
    list->size++;
    return 1;

}

int l_from_table(List *list, void *table, int table_size, size_t element_size, int element_type, int do_copy) {
    if (!table || !list /*|| list->head*/) return 0;
    int i;
    size_t taille_element = 0;

    l_init(list);

    if (do_copy) {
        taille_element = element_size;
    }

    for (i=0; i < table_size; i++) {
        // (char *) obligatoire (type à 1 octet) car void souvent pas compatible avec arithmétique
        if (!l_append(list, (char *)table + i * element_size, taille_element, element_type)) return 0;
    }
    return 1;
}

void l_print(List *list, void (*print)(void *element_print, int element_type)) {
    if (!list) {
        printf("not a list");
        return;
    }
    printf("BEGIN\n");
    Node *noeud_actuel = list->head;

    while (noeud_actuel != NULL) {
        print(noeud_actuel->element, noeud_actuel->type);
        noeud_actuel = noeud_actuel->next;
    }
    printf("END\n");
}

int l_pop(List *list, int index, void (*destructor)(void *element_del, int element_type)) {
    if (!list || !list->head) return 0;
    int index_actuel = 0;
    Node *noeud_actuel = list->head;

    if (index < 0 && list->size == 0) index = list->size; // normalement inutile grâce au !list->head
    while (index < 0) index = list->size+index;

    // on va à l'index voulu
    while (noeud_actuel->next != NULL && index_actuel < index) {
        noeud_actuel = noeud_actuel->next;
        index_actuel++;
    }

    // cas où l'index demandé est trop élevé
    if (index_actuel != index) return 0;


    if (!delete_node(list, noeud_actuel, destructor)) return 0;

    return 1;
}

int l_remove(List *list, void *element, void (*destructor)(void *element_del, int element_type)) {
    if (!list || !list->head) return 0;
    Node *noeud_actuel = list->head;
    void *element_actuel;

    while (noeud_actuel != NULL) {
        element_actuel = noeud_actuel->element;
        if (element_actuel == element) {
            if (!delete_node(list, noeud_actuel, destructor)) return 0;
            return 1;
        }
        noeud_actuel = noeud_actuel->next;
    }
    return 0;
}

void *l_get(List *list, int index) {
    if (!list || !list->head) return NULL;
    Node *noeud_actuel = list->head;
    int index_actuel = 0;

    if (index < 0 && list->size == 0) index = list->size; // normalement inutile grâce au !list->head
    while (index < 0) index = list->size+index;

    while (noeud_actuel != NULL) {
        if (index_actuel == index) return noeud_actuel->element;
        index_actuel++;
        noeud_actuel = noeud_actuel->next;
    }
    return NULL;
}

int l_get_type(List *list, int index) {
    if (!list || !list->head) return -1;
    Node *noeud_actuel = list->head;
    int index_actuel = 0;

    if (index < 0 && list->size == 0) index = list->size; // normalement inutile grâce au !list->head
    while (index < 0) index = list->size+index;

    while (noeud_actuel != NULL) {
        if (index_actuel == index) return noeud_actuel->type;
        index_actuel++;
        noeud_actuel = noeud_actuel->next;
    }
    return -1;
}

int l_get_size(List *list) {
    if (!list) return 0;
    return list->size;
}