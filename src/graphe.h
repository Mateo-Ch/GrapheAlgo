
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

int trouverIndexSommet( Graphe *graphe, Sommet *sommet );

Graphe* creerGraphe( char *txt );

char* getContenuFichier( char* fichier );

void detruireGraphe( Graphe *graphe );

#endif