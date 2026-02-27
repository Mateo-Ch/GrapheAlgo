#include "gui.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raymath.h"

#define TITLE "Solveur de graphe"

#define WIDTH 1280
#define MIN_WIDTH 640

#define HEIGHT 960
#define MIN_HEIGHT 480

#define TARGET_FPS 60

#define RAYON_PAR_DEFAUT 10.0f

#define TAILLE_POLICE 14

#define DISTANCE_NOEUD 75.0f

// GRAPHE

Noeud* trouverNoeud( char *nom, const GrapheGUI* graphe )
{
    for (int i = 0; i < graphe->nbNoeuds; i++)
    {
        if ( strcmp(nom, graphe->noeuds[i]->texte) == 0 )
        {
            return graphe->noeuds[i];
        }
    }
    return NULL;
}

Noeud* sommetToNoeud( const Sommet *sommet )
{
    Noeud *noeud  = (Noeud* ) malloc( sizeof( Noeud  ) );
    noeud->cercle = (Cercle*) malloc( sizeof( Cercle ) );

    noeud->texte = strdup( sommet->nom );

    noeud->cercle->couleur = LIME;
    noeud->cercle->rayon = RAYON_PAR_DEFAUT;
    noeud->cercle->position = (Vector2) {GetRandomValue(100, 1000), GetRandomValue(100, 1000)};

    return noeud;
}

// a revoir
Fleche* arcToFleche( const Arc *arc, const GrapheGUI *graphe )
{
    Fleche *fleche = (Fleche*) malloc( sizeof(Fleche) );
    fleche->texte = malloc( 6 * sizeof(char) );

    itoa(arc->cout, fleche->texte, 10);
    // int resultat = sprintf(fleche->texte, "%d", arc->cout);
    // if ( resultat < 0 )
    // {
    //     fleche->texte = "0";
    // }

    Noeud *dep = trouverNoeud( arc->sDep->nom, graphe );
    Noeud *arr = trouverNoeud( arc->sArr->nom, graphe );
    if ( !dep || !arr ) { return NULL; }

    fleche->depart = dep;
    fleche->arrive = arr;
    fleche->couleur = BLACK;

    return fleche;
}

GrapheGUI* creerGrapheVirtuel( const Graphe *graphe )
{
    GrapheGUI *grapheGUI = (GrapheGUI*) malloc(sizeof(GrapheGUI));

    grapheGUI->nbNoeuds = graphe->nbElements;
    grapheGUI->nbFleches = graphe->nbArcs;

    grapheGUI->noeuds  = (Noeud**) malloc(graphe->nbElements * sizeof(Noeud*));
    grapheGUI->fleches = (Fleche**) malloc(graphe->nbArcs * sizeof(Fleche*));

    for (int i = 0; i < graphe->nbElements; i++) { grapheGUI->noeuds[i] = sommetToNoeud( graphe->sommets[i] ); }
    for (int i = 0; i < graphe->nbArcs; i++) { grapheGUI->fleches[i] = arcToFleche( graphe->arcs[i], grapheGUI ); }

    return grapheGUI;
}

// RENDU

void dessinerNoeud( Noeud *noeud )
{
    DrawCircleV( noeud->cercle->position, noeud->cercle->rayon, noeud->cercle->couleur );
    DrawCircleLinesV( noeud->cercle->position, noeud->cercle->rayon, BLACK );
    DrawText( noeud->texte, noeud->cercle->position.x, noeud->cercle->position.y, TAILLE_POLICE, BLACK );
}

void dessinerFleche( Fleche *fleche )
{
    DrawLineV( fleche->depart->cercle->position, fleche->arrive->cercle->position, BLACK );
}

// SIMULATION

void simulation( const GrapheGUI *graphe )
{
    for (int i = 0; i < graphe->nbFleches; i++)
    {
        float k = 0.01;
        Noeud *noeudDepart = graphe->fleches[i]->depart;
        Noeud *noeudArrive = graphe->fleches[i]->arrive;

        Vector2 vDirection = Vector2Subtract(noeudDepart->cercle->position, noeudArrive->cercle->position);
        float vDirLength = Vector2Length( vDirection ) - DISTANCE_NOEUD;
        Vector2 force = Vector2Normalize( vDirection );
        force = Vector2Scale( force, -k * vDirLength );
        //printf("%f : %f | length %f\n", force.x, force.y, vDirLength);
        noeudDepart->cercle->position = Vector2Add( noeudDepart->cercle->position, force );
    }
}

// WINDOW

void freeGUI()
{
    // Free les structures
    CloseWindow();
}

void activerGUI( const GrapheGUI *graphe )
{
    while ( !WindowShouldClose() )
    {
        BeginDrawing();
        PollInputEvents();
        ClearBackground( LIGHTGRAY );

        simulation( graphe );
        
        for (int i = 0; i < graphe->nbNoeuds; i++)
        {
            dessinerNoeud( graphe->noeuds[i] );
        }
        
        for (int i = 0; i < graphe->nbFleches; i++)
        {
            dessinerFleche( graphe->fleches[i] );
        }

        EndDrawing();
    }

    freeGUI();
}

void creerGUI( const Graphe *gph )
{
    InitWindow( WIDTH, HEIGHT, TITLE );
    if ( !IsWindowReady() ) { return; }

    SetWindowState( FLAG_VSYNC_HINT );
    SetWindowState( FLAG_WINDOW_RESIZABLE );
    SetWindowMinSize( MIN_WIDTH, MIN_HEIGHT );
    SetTargetFPS( TARGET_FPS );

    GrapheGUI *graphe = creerGrapheVirtuel( gph );

    activerGUI(graphe);
}