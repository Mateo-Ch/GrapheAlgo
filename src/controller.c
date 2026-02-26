#include <stdio.h>
#include <stdlib.h>

#include "graphe.h"
#include "solveur.h"
#include "raylib.h"

void fenetre()
{
    InitWindow( 1280, 960, "Graphe" );
    SetTargetFPS(60);
    Color background = { 255, 255, 255, 255 };

    while ( !WindowShouldClose() )
    {
        PollInputEvents();
        BeginDrawing();
        EndDrawing();
        ClearBackground( background );
    }
    
    CloseWindow();
}

int main( int argc, char *argv[] )
{
    if ( argc != 2 ) { return -1; }

    fenetre();
    
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