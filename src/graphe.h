
#ifndef _GRAPHE_H_
#define _GRAPHE_H_

typedef struct Sommet Sommet;

struct Sommet
{
    char* nom;
    int cout;

    // suivant
    int nbSuivants;
    Sommet** suivants;
};

typedef struct
{
    int taille;
    Sommet* sommets;
} Graphe;

#endif