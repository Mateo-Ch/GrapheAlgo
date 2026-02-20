#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "graphe.h"

int* genererTableauCouts( Graphe *graphe, Sommet *source )
{
    int *d = malloc( graphe->nbElements * sizeof(int) );
    if ( d == NULL ) { return NULL; }

    for ( int i = 0; i < graphe->nbElements; i++ )
    {
        if ( strcmp( graphe->sommets[i]->nom, source->nom ) == 0 )
        {
            d[i] = 0;
        }
        else
        {
            d[i] = INT_MAX;
        }
    }

    return d;
}

// ATTENTION C'EST FULL CHATGPT PSK IL EST TARD (a revoir)

/*
 * Algorithme de Bellman-Ford.
 * Calcule et affiche les plus courts chemins depuis `source` vers tous les
 * autres sommets du graphe.  Détecte également les cycles de poids négatif.
 *
 * Complexité : O(V * E)
 */
void bellmanFord( Graphe *graphe, Sommet *source )
{
    /* Vérification des paramètres */
    if ( graphe == NULL || source == NULL ) { return; }

    int srcIndex = trouverIndexSommet( graphe, source );
    if ( srcIndex == -1 )
    {
        printf("Source introuvable dans le graphe.\n");
        return;
    }

    /* Initialisation du tableau de distances */
    int *d = genererTableauCouts( graphe, source );
    if ( d == NULL ) { return; }

    /* ---- Phase de relaxation : |V| - 1 itérations ---- */
    for (int i = 1; i < graphe->nbElements; i++)
    {
        for (int j = 0; j < graphe->nbArcs; j++)
        {
            Arc   *arc  = graphe->arcs[j];
            int    u    = trouverIndexSommet( graphe, arc->sDep );
            int    v    = trouverIndexSommet( graphe, arc->sArr );

            /* On ne relaxe que si u est déjà atteignable */
            if ( d[u] != INT_MAX && d[v] > d[u] + arc->cout )
            {
                d[v] = d[u] + arc->cout;
            }
        }
    }

    /* ---- Détection des cycles de poids négatif ---- */
    int cycleNegatif = 0;
    for (int j = 0; j < graphe->nbArcs; j++)
    {
        Arc *arc = graphe->arcs[j];
        int  u   = trouverIndexSommet( graphe, arc->sDep );
        int  v   = trouverIndexSommet( graphe, arc->sArr );

        if ( d[u] != INT_MAX && d[v] > d[u] + arc->cout )
        {
            cycleNegatif = 1;
            break;
        }
    }

    /* ---- Affichage des résultats ---- */
    if ( cycleNegatif )
    {
        printf("Le graphe contient un cycle de poids négatif.\n");
    }
    else
    {
        printf("Plus courts chemins depuis '%s' :\n", source->nom);
        for (int i = 0; i < graphe->nbElements; i++)
        {
            if ( d[i] == INT_MAX )
            {
                printf("  -> %-10s : inatteignable\n", graphe->sommets[i]->nom);
            }
            else
            {
                printf("  -> %-10s : %d\n", graphe->sommets[i]->nom, d[i]);
            }
        }
    }

    free(d);
}

/*
 * Algorithme de Dijkstra.
 * Calcule et affiche les plus courts chemins depuis `source` vers tous les
 * autres sommets du graphe orienté pondéré (poids positifs ou nuls).
 *
 * Complexité : O(V² + E)
 */
void dijkstra( Graphe *graphe, Sommet *source )
{
    /* Vérification des paramètres */
    if ( graphe == NULL || source == NULL ) { return; }

    int srcIndex = trouverIndexSommet( graphe, source );
    if ( srcIndex == -1 )
    {
        printf("Source introuvable dans le graphe.\n");
        return;
    }

    int n = graphe->nbElements;

    /* Initialisation du tableau de distances : d[v] = +∞, d[source] = 0 */
    int *d = genererTableauCouts( graphe, source );
    if ( d == NULL ) { return; }

    /* visite[u] = 1 si u appartient à E (sommets déjà visités), 0 sinon (appartient à F) */
    int *visite = calloc( n, sizeof(int) );
    if ( visite == NULL ) { free(d); return; }

    /* Tantque F ≠ ∅ (il reste des sommets non visités) */
    for (int iter = 0; iter < n; iter++)
    {
        /* u ← v | d[v] = min{ d[x] | x ∈ F } */
        int u = -1;
        for (int i = 0; i < n; i++)
        {
            if ( !visite[i] && ( u == -1 || d[i] < d[u] ) )
            {
                u = i;
            }
        }

        /* Si le sommet minimal est inatteignable, les sommets restants le sont aussi */
        if ( u == -1 || d[u] == INT_MAX ) { break; }

        /* F ← F − {u}  /  E ← E ∪ {u} */
        visite[u] = 1;

        /* Pour tout arc (u, v) : relâchement */
        for (int j = 0; j < graphe->nbArcs; j++)
        {
            Arc *arc = graphe->arcs[j];
            int  dep = trouverIndexSommet( graphe, arc->sDep );
            int  arr = trouverIndexSommet( graphe, arc->sArr );

            if ( dep == u && !visite[arr] )
            {
                /* Si d[v] > d[u] + w(u,v) alors d[v] ← d[u] + w(u,v) */
                if ( d[arr] > d[u] + arc->cout )
                {
                    d[arr] = d[u] + arc->cout;
                }
            }
        }
    }

    /* Affichage des résultats */
    printf("Plus courts chemins depuis '%s' (Dijkstra) :\n", source->nom);
    for (int i = 0; i < n; i++)
    {
        if ( d[i] == INT_MAX )
        {
            printf("  -> %-10s : inatteignable\n", graphe->sommets[i]->nom);
        }
        else
        {
            printf("  -> %-10s : %d\n", graphe->sommets[i]->nom, d[i]);
        }
    }

    free(d);
    free(visite);
}