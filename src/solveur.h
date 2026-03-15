#ifndef SOLVEUR_H
#define SOLVEUR_H

#include "graphe.h"

/* Résultat d'un algorithme de plus court chemin.
   distances[i] = coût depuis la source vers le sommet i.
   pred[i]      = indice du prédécesseur de i dans l'arbre (-1 si aucun). */
typedef struct {
    int *distances;
    int *pred;
    int  n;
} Resultat;

Resultat *dijkstra(const Graphe *graphe, const Sommet *source);
Resultat *bellmanFord(const Graphe *graphe, const Sommet *source);
void      detruireResultat(Resultat *r);

#endif