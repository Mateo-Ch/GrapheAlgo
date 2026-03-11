#define _GNU_SOURCE
#include "gui.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raymath.h"

#define TITLE "Solveur de graphe"

#define WIDTH 1280
#define MIN_WIDTH 320

#define HEIGHT 960
#define MIN_HEIGHT 240

#define TARGET_FPS 60

#define TAILLE_POLICE 20
#define EPAISSEUR_FLECHES 3.0f
#define RAYON_PAR_DEFAUT 25.0f

#define FORCE_REPULSION 300.0f
#define DISTANCE_NOEUD 250.0f
#define K 0.03f

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
    noeud->position = (Vector2) {GetRandomValue(100, 1000), GetRandomValue(100, 1000)}; // j'ai pas mieux { 0, 0 } marche pas
    
    noeud->cercle->couleur = WHITE;
    noeud->cercle->rayon = RAYON_PAR_DEFAUT;

    return noeud;
}

Fleche* arcToFleche(const Arc *arc, const GrapheGUI *graphe)
{
    Fleche *fleche = (Fleche*) malloc(sizeof(Fleche));

    int len = snprintf(NULL, 0, "%d", arc->cout);
    
    fleche->texte = (char*) malloc(len + 1);

    if ( fleche->texte )
    {
        snprintf(fleche->texte, len + 1, "%d", arc->cout);
    }

    fleche->depart = trouverNoeud(arc->sDep->nom, graphe);
    fleche->arrive = trouverNoeud(arc->sArr->nom, graphe);

    if ( !fleche->depart || !fleche->arrive )
    {
        free(fleche->texte);
        free(fleche);
        return NULL;
    }

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
    DrawCircleV( noeud->position, noeud->cercle->rayon, noeud->cercle->couleur );
    DrawCircleLinesV( noeud->position, noeud->cercle->rayon, BLACK );

    int largeurTexte = MeasureText(noeud->texte, TAILLE_POLICE);

    float posX = noeud->position.x - (largeurTexte / 2.0f);
    float posY = noeud->position.y - (TAILLE_POLICE / 2.0f);

    DrawText( noeud->texte, (int)posX, (int)posY, TAILLE_POLICE, BLACK );
}

void dessinerTeteFleche(Fleche *fleche)
{
    Vector2 start = fleche->depart->position;
    Vector2 end   = fleche->arrive->position;

    float longueurTete = 15.0f;   
    float largeurTete  = 8.0f;   
    float rayonNoeud   = fleche->arrive->cercle->rayon;

    Vector2 dir = Vector2Subtract(end, start);
    float distance = Vector2Length(dir);
    
    // Sécurité pour éviter la division par zéro si les nœuds sont superposés
    if (distance > 0) dir = Vector2Scale(dir, 1.0f / distance);
    else return;

    // 2. Calculer la pointe (p1) au bord du cercle de destination
    Vector2 p1 = Vector2Subtract(end, Vector2Scale(dir, rayonNoeud));

    // 3. Vecteur perpendiculaire pour la largeur
    Vector2 perp = (Vector2){ -dir.y, dir.x };

    // 4. Base du triangle (en arrière de la pointe p1)
    Vector2 base = Vector2Subtract(p1, Vector2Scale(dir, longueurTete));

    // 5. Coins de la base
    Vector2 p2 = Vector2Add(base, Vector2Scale(perp, largeurTete));
    Vector2 p3 = Vector2Subtract(base, Vector2Scale(perp, largeurTete));

    // IMPORTANT : L'ordre p1, p3, p2 assure souvent un meilleur rendu (Sens anti-horaire)
    DrawTriangle(p1, p3, p2, BLACK);
}

void dessinerFleche( Fleche *fleche )
{
    Vector2 start = fleche->depart->position;
    Vector2 end   = fleche->arrive->position;
    Vector2 dir   = Vector2Normalize(Vector2Subtract(end, start));
    
    // On arrête la ligne là où la tête de flèche commence (base)
    float offsetBase = fleche->arrive->cercle->rayon + 15.0f; // rayon + longueurTete
    Vector2 ligneFin = Vector2Subtract(end, Vector2Scale(dir, offsetBase));

    //écrire le cout de l'arc au milieu de la ligne
    Vector2 midPoint = Vector2Scale(Vector2Add(start, ligneFin), 0.5f);
    int largeurTexte = MeasureText(fleche->texte, TAILLE_POLICE);
    DrawLineEx( start, ligneFin, EPAISSEUR_FLECHES, BLACK );
    DrawRectangle((int)(midPoint.x - largeurTexte / 2.0f) - 5, (int)(midPoint.y - TAILLE_POLICE / 2.0f) - 5, largeurTexte + 10, TAILLE_POLICE + 10, LIGHTGRAY);
    DrawText(fleche->texte, (int)(midPoint.x - largeurTexte / 2.0f), (int)(midPoint.y - TAILLE_POLICE / 2.0f), TAILLE_POLICE, BLACK );

    dessinerTeteFleche( fleche );
}

// SIMULATION

Noeud* testDeCollisions( const GrapheGUI *graphe )
{
    Vector2 mousePos = GetMousePosition();

    for (int i = graphe->nbNoeuds - 1; i >= 0; i--)
    {
        if ( CheckCollisionPointCircle(mousePos, graphe->noeuds[i]->position, graphe->noeuds[i]->cercle->rayon) )
        {
            return graphe->noeuds[i];
        }
    }
    return NULL;
}

/* void simulation( const GrapheGUI *graphe )
{
    for(int i = 0; i < graphe->nbNoeuds; i++)
    {
        for(int j = 0; j < graphe->nbNoeuds; j++)
        {
            if ( i == j ) { continue; }

            Noeud *noeudDepart = graphe->noeuds[i];
            Noeud *noeudArrive = graphe->noeuds[j];

            Vector2 vDirection = Vector2Subtract(noeudDepart->position, noeudArrive->position);
            float distance = Vector2Length( vDirection );
            if (distance > 0 && distance < DISTANCE_NOEUD)
            {
                Vector2 direction = Vector2Normalize(vDirection);
                float k = FORCE_REPULSION / distance;
                Vector2 force = Vector2Scale(direction, k);
                noeudDepart->position = Vector2Add(noeudDepart->position, force);
            }

            noeudDepart->position.x = Clamp(noeudDepart->position.x, RAYON_PAR_DEFAUT, GetScreenWidth() - RAYON_PAR_DEFAUT);
            noeudDepart->position.y = Clamp(noeudDepart->position.y, RAYON_PAR_DEFAUT, GetScreenHeight() - RAYON_PAR_DEFAUT);
        }
    }

    for (int i = 0; i < graphe->nbFleches; i++)
    {
        Noeud *noeudDepart = graphe->fleches[i]->depart;
        Noeud *noeudArrive = graphe->fleches[i]->arrive;

        Vector2 vDirection = Vector2Subtract(noeudDepart->position, noeudArrive->position);
        float vDirLength = Vector2Length( vDirection ) - DISTANCE_NOEUD;
        Vector2 force = Vector2Normalize( vDirection );
        force = Vector2Scale( force, -K * vDirLength );

        noeudDepart->position = Vector2Add( noeudDepart->position, force );

        noeudDepart->position.x = Clamp(noeudDepart->position.x, RAYON_PAR_DEFAUT, GetScreenWidth() - RAYON_PAR_DEFAUT);
        noeudDepart->position.y = Clamp(noeudDepart->position.y, RAYON_PAR_DEFAUT, GetScreenHeight() - RAYON_PAR_DEFAUT);
    }
} */

void simulation(const GrapheGUI *graphe)
{
    // --- REPULSION BETWEEN ALL NODES ---
    for (int i = 0; i < graphe->nbNoeuds; i++)
    {
        for (int j = i + 1; j < graphe->nbNoeuds; j++)
        {
            Noeud *a = graphe->noeuds[i];
            Noeud *b = graphe->noeuds[j];

            Vector2 v = Vector2Subtract(a->position, b->position);
            float distance = Vector2Length(v);

            if (distance > 0 && distance < DISTANCE_NOEUD)
            {
                Vector2 dir = Vector2Normalize(v);
                float k = FORCE_REPULSION / distance;

                Vector2 force = Vector2Scale(dir, k);

                a->position = Vector2Add(a->position, force);
                b->position = Vector2Subtract(b->position, force);
            }
        }
    }

    // --- ATTRACTION ALONG EDGES ---
    for (int i = 0; i < graphe->nbFleches; i++)
    {
        Noeud *a = graphe->fleches[i]->depart;
        Noeud *b = graphe->fleches[i]->arrive;

        Vector2 v = Vector2Subtract(a->position, b->position);
        float length = Vector2Length(v);

        if (length == 0) continue;

        float delta = length - DISTANCE_NOEUD;

        Vector2 force = Vector2Normalize(v);
        force = Vector2Scale(force, -K * delta);

        a->position = Vector2Add(a->position, force);
        b->position = Vector2Subtract(b->position, force);
    }

    // --- CLAMP POSITIONS (once per node) ---
    for (int i = 0; i < graphe->nbNoeuds; i++)
    {
        Noeud *n = graphe->noeuds[i];

        n->position.x = Clamp(n->position.x,
                              RAYON_PAR_DEFAUT,
                              GetScreenWidth() - RAYON_PAR_DEFAUT);

        n->position.y = Clamp(n->position.y,
                              RAYON_PAR_DEFAUT,
                              GetScreenHeight() - RAYON_PAR_DEFAUT);
    }
}

// WINDOW

void freeGUI()
{
    CloseWindow();
}

void activerGUI( const GrapheGUI *graphe )
{
    Noeud* selection = NULL;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawFPS(10, 10);

        if ( IsMouseButtonPressed(MOUSE_LEFT_BUTTON) )
        {
            selection = testDeCollisions(graphe);
        }

        if ( IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selection != NULL )
        {
            selection->position = GetMousePosition();
        }

        if ( IsMouseButtonReleased(MOUSE_LEFT_BUTTON) )
        {
            selection = NULL;
        }

        simulation(graphe);

        for (int i = 0; i < graphe->nbFleches; i++)
        {
            dessinerFleche(graphe->fleches[i]);
        }

        for (int i = 0; i < graphe->nbNoeuds; i++)
        {
            dessinerNoeud(graphe->noeuds[i]);
        }

        EndDrawing();
    }

    freeGUI();
}

void creerGUI( const Graphe *gph )
{
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    InitWindow( WIDTH, HEIGHT, TITLE );
    if ( !IsWindowReady() ) { return; }

    SetWindowState( FLAG_VSYNC_HINT );
    SetWindowState( FLAG_WINDOW_RESIZABLE );
    SetWindowMinSize( MIN_WIDTH, MIN_HEIGHT );
    SetTargetFPS( TARGET_FPS );

    GrapheGUI *graphe = creerGrapheVirtuel( gph );
    activerGUI(graphe);
}