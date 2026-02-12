
#ifndef _GRAPHE_H_
#define _GRAPHE_H_

typedef struct
{
    char* nom;

    long long couts[];
} Sommet;

typedef struct
{
    int taille;
    Sommet sommets[];
} Graphe;

#endif