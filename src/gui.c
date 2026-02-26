#include "gui.h"

#define TITLE "Solveur de graphe"

#define WIDTH 1280
#define MIN_WIDTH 640

#define HEIGHT 960
#define MIN_HEIGHT 480

#define TARGET_FPS 60

// GRAPHE

void creerGrapheVirtuel();
void sommetToNoeud();
void arcToFleche();

// WINDOW

void creerFenetre( Graphe *graphe )
{
    InitWindow( WIDTH, HEIGHT, TITLE );
    if ( !IsWindowReady() ) { return; }

    SetWindowState( FLAG_VSYNC_HINT );
    SetWindowState( FLAG_WINDOW_RESIZABLE );

    SetWindowMinSize( MIN_WIDTH, MIN_HEIGHT );

    SetTargetFPS( TARGET_FPS );
}

void activerFenetre()
{
    while ( !WindowShouldClose() )
    {
        BeginDrawing();
        PollInputEvents();
        ClearBackground( LIGHTGRAY );
        EndDrawing();
    }
}

void freeFenetre()
{
    // Free les structures
    CloseWindow();
}