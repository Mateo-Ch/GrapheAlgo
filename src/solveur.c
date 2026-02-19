#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "graphe.h"

// ATTENTION C'EST FULL CHATGPT PSK IL EST TARD (a revoir)

/* Retourne l'index d'un sommet dans le tableau graphe->sommets, ou -1 si absent. */
static int trouverIndexSommet( Graphe *graphe, Sommet *sommet )
{
    for (int i = 0; i < graphe->nbElements; i++)
    {
        if ( graphe->sommets[i] == sommet )
        {
            return i;
        }
    }
    return -1;
}

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