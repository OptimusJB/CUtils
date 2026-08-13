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

struct List{
    int size;
    Node *head;
};

static int l_init(List *list) {
    if (!list) return 0;
    list->size = 0;
    list->head = NULL;
    return 1;
}

List *l_new() {
    List *list = malloc(sizeof(List));
    l_init(list);
    return list;
}

static Node *create_node(void *element, int element_type) {
    void *new_element;
    Node *new_noeud = malloc(sizeof(Node));

    if (!new_noeud) return NULL;

    new_noeud->type = element_type;
    new_noeud->next = NULL;
    new_noeud->previous = NULL;

    new_noeud->element = element;
    return new_noeud;
}

static int delete_node(List *list, Node *node, void (*destructor)(void *element_del, int element_type)) {
    // Note : dans cette fonction il vaut mieux éviter de return 0 après avoir fait une suppression partielle (car en cas d'échec l_clear garde le noeud actuel)
    if (!list || !node) return 0;
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

    // on supprime le noeud (doit être à la fin, on doit être sûr que ça retourne 1, pour éviter les problèmes avec l_clear)
    free(node);

    list->size--;
    return 1;
}

int l_append(List *list, void *element, int element_type) {
    if (!list) return 0;
    Node *new_noeud = create_node(element, element_type);
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

int l_set(List *list, int index, void *element, int element_type) {
    if (!list || !list->head) return 0;
    Node *noeud_actuel = list->head;
    int index_actuel = 0;
    void *new_element;

    if (index < 0 && list->size == 0) index = list->size;
    while (index < 0) index = list->size+index;

    // on va à l'index voulu
    while (noeud_actuel->next != NULL && index_actuel < index) {
        noeud_actuel = noeud_actuel->next;
        index_actuel++;
    }

    // cas où l'index demandé est trop élevé
    if (index_actuel != index) {
        return 0;
    }

    new_element = element;

    noeud_actuel->element = new_element;
    noeud_actuel->type = element_type;
    return 1;
}

int l_insert(List *list, int index, void *element, int element_type) {
    if (!list) return 0;
    Node *new_noeud = create_node(element, element_type);
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

int l_from_table(List *list, void *table, int table_size, size_t element_size, int element_type) {
    if (!table || !list || list->head) return 0;
    int i;

    l_init(list);

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

int l_index(List *list, void *element) {
    // retourne -1 si non trouvé
    if (!list || !list->head) return -1;
    Node *noeud_actuel = list->head;
    void *element_actuel;
    int index_actuel = 0;

    while (noeud_actuel != NULL) {
        element_actuel = noeud_actuel->element;
        if (element_actuel == element) {
            return index_actuel;
        }
        noeud_actuel = noeud_actuel->next;
        index_actuel++;
    }
    return -1;
}

int l_reverse(List *list) {
    if (!list) return 0;
    Node *noeud_actuel = list->head;
    Node *noeud_suivant;

    while (noeud_actuel) {
        noeud_suivant = noeud_actuel->next;
        noeud_actuel->next = noeud_actuel->previous;
        noeud_actuel->previous = noeud_suivant;

        list->head = noeud_actuel;
        noeud_actuel = noeud_suivant;
    }
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

int l_clear(List *list, void (*destructor)(void *element_del, int element_type)) {
    if (!list || !list->head) return 0;
    Node *noeud_actuel = list->head;
    Node *noeud_suivant;

    while (noeud_actuel != NULL) {
        noeud_suivant = noeud_actuel->next;
        if (!delete_node(list, noeud_actuel, destructor)) {
            list->head = noeud_actuel; // pour éviter que des noeuds se retrouvent dans la nature. On est sûr grâce à delete_node que noeud_actuel est toujours là.
            return 0;
        }
        noeud_actuel = noeud_suivant;
    }
    return 1;
}

int l_free(List *list) {
    if (!list || list->head) return 0;
    free(list);
    return 1;
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

int l_sub(List *dest, List *src, int begin, int end) {
    Node *src_actuel;
    Node *dest_actuel;
    Node *new_noeud;
    int index_actuel = 0;
    if (!dest || !src || dest->head || !src->head) return 0;
    src_actuel = src->head;
    dest_actuel = dest->head;

    if (begin >= src->size) begin = src->size;
    if (end >= src->size) end = src->size;

    if (begin < 0 && src->size == 0) begin = 0;
    while (begin < 0) begin = src->size+begin;
    if (end < 0 && src->size == 0) end = 0;
    while (end < 0) end = src->size+end;

    if (begin > end) return 0;

    while (index_actuel != begin && index_actuel != src->size - 1) {
        src_actuel = src_actuel->next;
        if (!src_actuel) return 0;  // ne s'exécute normalement jamais car begin est inférieur à la taille de src
        index_actuel++;
    }

    while (begin < end) {
        new_noeud = create_node(src_actuel->element, src_actuel->type);
        if (!new_noeud) {
            l_clear(dest, NULL);
            return 0;
        }

        if (!dest->head) { // première itération (clion dit "condition always true" mais c'est faux)
            dest->head = new_noeud;
            dest_actuel = dest->head;
        }
        else {
            dest_actuel->next = new_noeud;
            new_noeud->previous = dest_actuel;
            dest_actuel = dest_actuel->next;
        }
        dest->size++;
        src_actuel = src_actuel->next;
        begin++;
    }
    return 1;
}

int l_sort(List *list, int (*compare)(void *a, void *b, int type_a, int type_b)) {
    if (!list) return 0;
    Node *noeud_actuel;
    Node *gauche_actuel;
    Node *droite_actuel;
    Node *choisi;
    int moitie = list->size / 2;

    // cas de base
    if (list->size <= 1) return 1;

    // on crée les deux parties
    List *gauche = l_new();
    List *droite = l_new();

    if (!gauche && !droite) return 0;
    else if (gauche && !droite) {
        free(gauche);
        return 0;
    }
    else if (!gauche && droite) {
        free(droite);
        return 0;
    }


    if (!l_sub(gauche, list, 0, moitie) ||
    !l_sub(droite, list, moitie, list->size) ||

    !l_sort(gauche, compare) ||
    !l_sort(droite, compare)) {
        l_clear(gauche, NULL);
        l_clear(droite, NULL);
        l_free(gauche);
        l_free(droite);
        return 0;
    }


    // triage
    gauche_actuel = gauche->head;
    droite_actuel = droite->head;
    noeud_actuel = list->head;
    while ((gauche_actuel || droite_actuel) && noeud_actuel) {
        if (gauche_actuel && !droite_actuel) {
            choisi = gauche_actuel;
            gauche_actuel = gauche_actuel->next;
        }
        else if (!gauche_actuel && droite_actuel) {
            choisi = droite_actuel;
            droite_actuel = droite_actuel->next;
        }
        else if (gauche_actuel && droite_actuel) {
            if (compare(gauche_actuel->element, droite_actuel->element, gauche_actuel->type, droite_actuel->type) < 0) {
                choisi = gauche_actuel;
                gauche_actuel = gauche_actuel->next;
            }
            else {
                choisi = droite_actuel;
                droite_actuel = droite_actuel->next;
            }
        }

        noeud_actuel->element = choisi->element;
        noeud_actuel->type = choisi->type;

        noeud_actuel = noeud_actuel->next;
    }

    // on libère la mémoire
    l_clear(gauche, NULL);
    l_clear(droite, NULL);
    l_free(gauche);
    l_free(droite);
    return 1;
}