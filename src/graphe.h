#ifndef GRAPHE_H
#define GRAPHE_H

/* Séparateurs du format de fichier */
#define SEP_LIGNE       "\n"
#define SEP_PROPRIETE   ";"
#define SEP_VOISIN      ","
#define SEP_VOISIN_COUT ":"

typedef struct {
    char *nom;
} Sommet;

typedef struct {
    Sommet *depart;
    Sommet *arrivee;
    int     cout;
} Arc;

typedef struct {
    int      nbSommets;
    int      nbArcs;
    Sommet **sommets;
    Arc    **arcs;
} Graphe;

/* Lecture fichier */
char   *lireFichier(const char *chemin);

/* Construction */
Graphe *creerGraphe(const char *texte);
void    detruireGraphe(Graphe *graphe);

/* Recherche */
Sommet *trouverSommet(const Graphe *graphe, const char *nom);
int     indiceSommet(const Graphe *graphe, const Sommet *sommet);

#endif
