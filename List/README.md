# List

## Présentation

Ces fichiers fournissent des fonctions permettant de reproduire
le comportement d'une liste. Il est possible d'y insérer une quantité infini
d'éléments de n'importe quel type.


## Table des matières

- [Bon à savoir](#bon-à-savoir)
- [l_new et l_free](#l_new-et-l_free)
- [l_from_table](#l_from_table)
- [l_insert](#l_insert)
- [l_append](#l_append)
- [l_set](#l_set)
- [l_pop](#l_pop)
- [l_remove](#l_remove)
- [l_clear](#l_clear)
- [l_sub](#l_sub)
- [l_reverse](#l_reverse)
- [l_sort](#l_sort)
- [l_print](#l_print)
- [l_get](#l_get)
- [l_index](#l_index)
- [l_get_type](#l_get_type)
- [l_get_size](#l_get_size)

### Bon à savoir
L'intégralité des fonctions disponibles retournent 1 en cas de succès et 0 en cas d'échec
(excepté l_index).

Dans toutes les fonctions, le paramètre element_type sert uniquement d'aide à l'utilisateur, afin qu'il puisse faire la différence
entre chaque élément si plusieurs types sont présents dans la liste (particulièrement utile pour l'affichage), et n'est donc pas utilisé par les fonctions en elles-même.  
Plusieurs macros sont déjà présentes (TYPE_INT, TYPE_CHAR, etc), mais il est tout à fait possible d'en ajouter d'autres.

La liste stocke et fonctionne uniquement avec le **pointeur** des éléments : modifier la valeur d'une variable précédemment ajoutée dans une liste modifiera également sa valeur dans la liste.  
Les fonctions ne modifient jamais le contenu pointé, c'est donc à l'utilisateur de gérer le contenu pointé.  
Afin de stocker des éléments persistants, il est possible d'utiliser malloc, mais il faut donc penser à le libérer de la mémoire lors de la suppression.

Dans le reste de ce readme, un élément fait en réalité référence à son pointeur

### l_new et l_free
Ces deux fonctions permettent de créer et de supprimer une liste de la mémoire.
l_new est le seul moyen d'obtenir une liste.

Note : l_free ne fonctionne que si la liste est vide.
Envisagez d'utiliser [l_clear](#l_clear) avant cette fonction

### l_from_table
Paramètres :
- liste à traiter
- tableau à traiter
- taille du tableau (Il s'agit bien du nombre d'éléments, et non de la taille du tableau en octet)
- taille d'un élément du tableau en octet (exemple : 4 pour un entier)
- type de l'élément (voir [Bon à savoir](#bon-à-savoir))

Il s'agit d'une fonction permettant de récupérer tous les éléments d'un tableau
pour les mettre dans une liste **vide**.

### l_insert
Paramètres :
- liste à traiter
- index où ajouter l'élément
- élément à ajouter
- type de l'élément (voir [Bon à savoir](#bon-à-savoir))

Il s'agit d'une fonction permettant d'insérer un élément dans la liste à l'index choisi.  
Si l'index est trop grand, l'élément sera placé à la fin de la liste.  
Si l'index est négatif, l'élément sera placé à partir de la fin (Note : Si l'index négatif est trop élevé, cela reboucle vers la fin).

### l_append
Paramètres :
- liste à traiter
- élément à ajouter
- type de l'élément (voir [Bon à savoir](#bon-à-savoir))

Il s'agit d'une fonction permettant d'insérer un élément à la fin de la liste.

### l_set
Paramètres :
- liste à traiter
- index de l'élément à changer
- nouvel élément
- type de l'élément (voir [Bon à savoir](#bon-à-savoir))

Il s'agit d'une fonction permettant de remplacer l'élément d'une liste par un autre à l'index choisi.  
Si l'index est trop grand, le remplacement n'aura pas lieu, et la fonction retournera 0.  
Si l'index est négatif, l'élément sera remplacé à partir de la fin (Note : Si l'index négatif est trop élevé, cela reboucle vers la fin).

Note : l_set remplace bien le pointeur dans la liste, et non le contenu pointé (voir [Bon à savoir](#bon-à-savoir)).
Si d'autres listes contiennent l'ancien élément, celles-ci ne seront pas impactées.

### l_pop
Paramètres :
- liste à traiter
- index de l'élément à supprimer
- fonction de libération de l'élément

Il s'agit d'une fonction permettant de supprimer un élément d'une liste à l'index choisi.  
Si l'index est trop grand, la suppression n'aura pas lieu.  
Si l'index est négatif, l'élément sera supprimé à partir de la fin (Note : Si l'index négatif est trop élevé, cela reboucle vers la fin).  

La fonction de libération est facultative (possibilité de mettre NULL). Cette fonction prend en paramètre l'élément à libérer et le type de l'élément (voir [Bon à savoir](#bon-à-savoir)).
Le but de cette fonction est de libérer l'élément dans le cas où celui-ci a été créé via malloc (ou équivalent).

### l_remove
Paramètres :
- liste à traiter
- élément à supprimer
- fonction de libération de l'élément (voir [l_pop](#l_pop) pour plus de détails)

Il s'agit d'une fonction permettant de supprimer un élément d'une liste à partir de son pointeur.  
Attention : la comparaison se fait sur le pointeur, et non sur le contenu pointé.

### l_clear
Paramètres :
- liste à traiter
- fonction de libération de l'élément (voir [l_pop](#l_pop) pour plus de détails)

Il s'agit d'une fonction permettant de supprimer tous les éléments d'une liste.

### l_sub
Paramètres :
- liste cible
- liste source
- index de début de la copie
- index de fin de copie (exclusif)

Il s'agit d'une fonction permettant de copier partiellement ou intégralement une liste dans une autre.  
La liste cible utilise les mêmes pointeurs que la liste source, donc changer la valeur pointée changera la valeur pour les deux listes.  
Si l'index de fin est trop grand, toute la liste sera copiée à partir de l'index de début.  
Si l'index de début ou de fin est négatif, l'index sera calculé à partir de la fin (Note : Si l'index négatif est trop élevé, cela reboucle vers la fin).

### l_reverse
Paramètres :
- liste à traiter

Il s'agit d'une fonction qui inverse l'ordre des éléments dans la liste.

### l_sort
Paramètres :
- liste à traiter
- fonction de comparaison

Il s'agit d'une fonction permettant de trier la liste (ordre croissant).  
La fonction de comparaison est une fonction prenant en paramètres deux éléments, ainsi que le type de ces deux éléments. 
Cette fonction doit retourner un nombre positif si l'élément a est supérieur à l'élément b, un nombre négatif si l'élément b est supérieur à l'élément a, et 0 si les deux éléments sont égaux.

### l_print
Paramètres :
- liste à traiter
- fonction d'affichage de l'élément

Il s'agit d'une fonction permettant d'afficher le contenu de la liste.  
La fonction d'affichage prend en paramètre un élément et le type de cet élément.
Le but de cette fonction est d'afficher l'élément.
Pour chaque élément, la fonction d'affichage sera appelée.

### l_get
Paramètres :
- liste à traiter
- index de l'élément à récupérer

Il s'agit d'une fonction permettant de récupérer l'élément à l'index choisi dans une liste.
Si l'index de fin est trop grand, la fonction retournera NULL. 
Si l'index de début ou de fin est négatif, l'index sera calculé à partir de la fin (Note : Si l'index négatif est trop élevé, cela reboucle vers la fin).

### l_index
Paramètres :
- liste à traiter
- élément à trouver

Il s'agit d'une fonction permettant de récupérer l'index d'un élément dans une liste.  
Attention : la comparaison se fait sur le pointeur, et non sur le contenu pointé.

Note : contrairement aux autres fonctions, cette fonction renvoie -1 si l'élément n'a pas été trouvé dans la liste.

### l_get_type
Paramètres :
- liste à traiter
- index de l'élément à récupérer

Il s'agit d'une fonction permettant de récupérer le type de l'élément (voir [Bon à savoir](#bon-à-savoir)) à l'index choisi dans une liste.

### l_get_size
Paramètres :
- liste à traiter

Il s'agit d'une fonction qui renvoie la taille actuelle de la liste.