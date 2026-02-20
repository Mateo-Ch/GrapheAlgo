#include <stdio.h>
#include <stdlib.h>

#include "graphe.h"
#include "solveur.h"

int main( int argc, char *argv[] )
{
    if ( argc != 2 ) { return -1; }

    char *texte = getContenuFichier( argv[1] );
    if ( texte == NULL ) { return -1; }

    Graphe *graphe = creerGraphe( texte );
    Sommet *source = trouverSommet( graphe, "A" );

    bellmanFord( graphe, source );
    printf("\n-------------\n");
    dijkstra( graphe, source );

    free(texte);
    detruireGraphe(graphe);
    
    return 0;
}