
#ifndef _GRAPHE_H_
#define _GRAPHE_H_

#define SEP_LIGNE       "\n"
#define SEP_PROPRIETE   ";"
#define SEP_VOISIN      ","
#define SEP_VOISIN_COUT ":"

typedef struct
{
    char* nom;
} Sommet;

typedef struct
{
    int cout;
    Sommet *sDep;
    Sommet *sArr;
} Arc;

typedef struct
{
    int nbElements;
    int nbArcs;

    Sommet **sommets;
    Arc **arcs;
} Graphe;

Sommet* trouverSommet( Graphe *graphe, char *nom );

// Création de graphes
Graphe* creerGraphe( char *txt );

// Résolution de graphes
int* genererTableauCouts( Graphe *graphe, Sommet *source );

#endif