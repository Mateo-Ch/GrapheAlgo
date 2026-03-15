#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "graphe.h"
#include "solveur.h"

/* ---------- Résultat ---------- */

static Resultat *creerResultat(const Graphe *g, int srcIdx)
{
    int n = g->nbSommets;

    Resultat *r  = malloc(sizeof(Resultat));
    r->n         = n;
    r->distances = malloc(n * sizeof(int));
    r->pred      = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        r->distances[i] = (i == srcIdx) ? 0 : INT_MAX;
        r->pred[i]      = -1;
    }
    return r;
}

void detruireResultat(Resultat *r)
{
    if (!r) return;
    free(r->distances);
    free(r->pred);
    free(r);
}

/* ---------- Bellman-Ford  O(V × E) ---------- */

Resultat *bellmanFord(const Graphe *g, const Sommet *source)
{
    int src = indiceSommet(g, source);
    if (src == -1) { printf("Source introuvable.\n"); return NULL; }

    Resultat *r = creerResultat(g, src);
    int *d = r->distances;

    /* |V| - 1 relaxations */
    for (int iter = 1; iter < g->nbSommets; iter++) {
        for (int j = 0; j < g->nbArcs; j++) {
            Arc *a = g->arcs[j];
            int  u = indiceSommet(g, a->depart);
            int  v = indiceSommet(g, a->arrivee);
            if (d[u] != INT_MAX && d[v] > d[u] + a->cout) {
                d[v]       = d[u] + a->cout;
                r->pred[v] = u;
            }
        }
    }

    /* Détection des cycles négatifs */
    for (int j = 0; j < g->nbArcs; j++) {
        Arc *a = g->arcs[j];
        int  u = indiceSommet(g, a->depart);
        int  v = indiceSommet(g, a->arrivee);
        if (d[u] != INT_MAX && d[v] > d[u] + a->cout) {
            printf("Cycle de poids négatif détecté.\n");
            detruireResultat(r);
            return NULL;
        }
    }

    return r;
}

/* ---------- Dijkstra  O(V² + E) ---------- */

Resultat *dijkstra(const Graphe *g, const Sommet *source)
{
    int src = indiceSommet(g, source);
    if (src == -1) { printf("Source introuvable.\n"); return NULL; }

    int n       = g->nbSommets;
    Resultat *r = creerResultat(g, src);
    int *d      = r->distances;
    int *vu     = calloc(n, sizeof(int));
    if (!vu) { detruireResultat(r); return NULL; }

    for (int iter = 0; iter < n; iter++) {

        /* Sommet non visité avec la distance minimale */
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!vu[i] && (u == -1 || d[i] < d[u])) u = i;

        if (u == -1 || d[u] == INT_MAX) break;
        vu[u] = 1;

        /* Relaxation des voisins */
        for (int j = 0; j < g->nbArcs; j++) {
            Arc *a   = g->arcs[j];
            int  dep = indiceSommet(g, a->depart);
            int  arr = indiceSommet(g, a->arrivee);
            if (dep == u && !vu[arr] && d[arr] > d[u] + a->cout) {
                d[arr]      = d[u] + a->cout;
                r->pred[arr] = u;
            }
        }
    }

    free(vu);
    return r;
}