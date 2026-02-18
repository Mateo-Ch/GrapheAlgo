
#ifndef _GRAPHE_H_
#define _GRAPHE_H_

typedef struct Sommet Sommet;
typedef struct Arete Arete;

struct Arete
{
    Sommet* destination;
    int poids;
};

struct Sommet
{
    char* nom;
    
    // Arêtes sortantes
    int nbSuivants;
    Arete* suivants;
};

typedef struct
{
    int taille;
    Sommet* sommets;
} Graphe;

typedef struct {
    char* nom;
    int poids;
} VoisinPoids;

#endif