
#ifndef _GRAPHE_H_
#define _GRAPHE_H_

typedef struct Sommet Sommet;

struct Sommet
{
    char* nom;
    int cout;

    // suivant
    Sommet* suivant;
    long* couts;
};

typedef struct
{
    int taille;
    Sommet* sommets;
} Graphe;

#endif