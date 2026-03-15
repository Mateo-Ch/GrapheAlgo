#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "raylib.h"
#include "raymath.h"
#include "gui.h"
#include "solveur.h"

/* ---------- Constantes ---------- */

#define TITRE             "Solveur de graphe"
#define LARGEUR           1280
#define HAUTEUR           960
#define LARGEUR_MIN       320
#define HAUTEUR_MIN       240
#define FPS_CIBLE         60

#define POLICE_TAILLE     20
#define FLECHE_EPAISSEUR  3.0f
#define NOEUD_RAYON       25.0f

#define SIM_REPULSION     5000.0f
#define SIM_DISTANCE      120.0f
#define SIM_RESSORT       0.01f
#define SIM_AMORTISSEMENT 0.9f

/* ---------- Conversion graphe → GUI ---------- */

static Noeud *trouverNoeud(const GrapheGUI *g, const char *nom)
{
    for (int i = 0; i < g->nbNoeuds; i++)
        if (strcmp(g->noeuds[i]->texte, nom) == 0)
            return g->noeuds[i];
    return NULL;
}

static Noeud *sommetVersNoeud(const Sommet *sommet)
{
    Noeud  *n = malloc(sizeof(Noeud));
    Cercle *c = malloc(sizeof(Cercle));

    n->texte    = strdup(sommet->nom);
    n->position = (Vector2){ GetRandomValue(100, 1000), GetRandomValue(100, 1000) };
    c->couleur  = WHITE;
    c->rayon    = NOEUD_RAYON;
    n->cercle   = c;
    return n;
}

static Fleche *arcVersFleche(const Arc *arc, const GrapheGUI *g)
{
    Noeud *dep = trouverNoeud(g, arc->depart->nom);
    Noeud *arr = trouverNoeud(g, arc->arrivee->nom);
    if (!dep || !arr) return NULL;

    Fleche *f = malloc(sizeof(Fleche));
    int len   = snprintf(NULL, 0, "%d", arc->cout);
    f->texte  = malloc(len + 1);
    snprintf(f->texte, len + 1, "%d", arc->cout);
    f->depart  = dep;
    f->arrivee = arr;
    f->couleur = BLACK;
    return f;
}

static GrapheGUI *creerGrapheGUI(const Graphe *g)
{
    GrapheGUI *gui = malloc(sizeof(GrapheGUI));
    gui->nbNoeuds  = g->nbSommets;
    gui->nbFleches = g->nbArcs;
    gui->noeuds    = malloc(g->nbSommets * sizeof(Noeud *));
    gui->fleches   = malloc(g->nbArcs    * sizeof(Fleche *));

    for (int i = 0; i < g->nbSommets; i++)
        gui->noeuds[i] = sommetVersNoeud(g->sommets[i]);

    for (int i = 0; i < g->nbArcs; i++)
        gui->fleches[i] = arcVersFleche(g->arcs[i], gui);

    return gui;
}

/* ---------- Coloration du chemin ---------- */

/* Remet toutes les flèches en noir, puis colorie en rouge
   les arcs appartenant à l'arbre des plus courts chemins. */
static void colorierChemins(GrapheGUI *gui, const Graphe *g, const Resultat *r)
{
    for (int i = 0; i < gui->nbFleches; i++)
        gui->fleches[i]->couleur = BLACK;

    if (!r) return;

    for (int v = 0; v < g->nbSommets; v++) {
        if (r->pred[v] == -1) continue;
        const char *nomDep = g->sommets[r->pred[v]]->nom;
        const char *nomArr = g->sommets[v]->nom;

        for (int i = 0; i < gui->nbFleches; i++) {
            Fleche *f = gui->fleches[i];
            if (strcmp(f->depart->texte, nomDep) == 0 &&
                strcmp(f->arrivee->texte, nomArr) == 0) {
                f->couleur = RED;
                break;
            }
        }
    }
}

/* ---------- Rendu ---------- */

static void dessinerNoeud(const Noeud *n)
{
    DrawCircleV(n->position, n->cercle->rayon, n->cercle->couleur);
    DrawCircleLinesV(n->position, n->cercle->rayon, BLACK);

    int larg = MeasureText(n->texte, POLICE_TAILLE);
    DrawText(n->texte,
             (int)(n->position.x - larg / 2.0f),
             (int)(n->position.y - POLICE_TAILLE / 2.0f),
             POLICE_TAILLE, BLACK);
}

static void dessinerTete(const Fleche *f)
{
    Vector2 dir = Vector2Subtract(f->arrivee->position, f->depart->position);
    float dist  = Vector2Length(dir);
    if (dist == 0) return;
    dir = Vector2Scale(dir, 1.0f / dist);

    float longueur = 15.0f, largeur = 8.0f;
    Vector2 pointe = Vector2Subtract(f->arrivee->position,
                                     Vector2Scale(dir, f->arrivee->cercle->rayon));
    Vector2 base   = Vector2Subtract(pointe, Vector2Scale(dir, longueur));
    Vector2 perp   = (Vector2){ -dir.y, dir.x };

    DrawTriangle(pointe,
                 Vector2Subtract(base, Vector2Scale(perp, largeur)),
                 Vector2Add(base, Vector2Scale(perp, largeur)),
                 f->couleur);
}

static void dessinerFleche(const Fleche *f)
{
    Vector2 dir = Vector2Normalize(Vector2Subtract(f->arrivee->position, f->depart->position));
    Vector2 fin = Vector2Subtract(f->arrivee->position,
                                  Vector2Scale(dir, f->arrivee->cercle->rayon + 15.0f));

    DrawLineEx(f->depart->position, fin, FLECHE_EPAISSEUR, f->couleur);

    /* Étiquette au milieu */
    Vector2 milieu = Vector2Scale(Vector2Add(f->depart->position, fin), 0.5f);
    int larg = MeasureText(f->texte, POLICE_TAILLE);
    DrawRectangle((int)(milieu.x - larg / 2.0f) - 5,
                  (int)(milieu.y - POLICE_TAILLE / 2.0f) - 5,
                  larg + 10, POLICE_TAILLE + 10, LIGHTGRAY);
    DrawText(f->texte,
             (int)(milieu.x - larg / 2.0f),
             (int)(milieu.y - POLICE_TAILLE / 2.0f),
             POLICE_TAILLE, BLACK);

    dessinerTete(f);
}

/* Panneau des distances affiché en bas à droite. */
static void dessinerPanneauResultats(const Graphe *g, const Resultat *r, const char *algo)
{
    if (!r) return;

    const int ligneH  = 22;
    const int padding = 10;
    const int largeur = 200;
    int hauteur       = padding * 2 + 20 + g->nbSommets * ligneH;
    int x             = GetScreenWidth()  - largeur - padding;
    int y             = GetScreenHeight() - hauteur  - padding;

    DrawRectangle(x, y, largeur, hauteur, Fade(BLACK, 0.70f));
    DrawRectangleLinesEx((Rectangle){ x, y, largeur, hauteur }, 1, GRAY);

    DrawText(algo, x + padding, y + padding, 18, YELLOW);

    for (int i = 0; i < g->nbSommets; i++) {
        char ligne[64];
        if (r->distances[i] == INT_MAX)
            snprintf(ligne, sizeof(ligne), "%-8s : inf", g->sommets[i]->nom);
        else
            snprintf(ligne, sizeof(ligne), "%-8s : %d",  g->sommets[i]->nom, r->distances[i]);

        Color couleur = (r->pred[i] != -1 || r->distances[i] == 0) ? RED : WHITE;
        DrawText(ligne, x + padding, y + padding + 20 + i * ligneH, 18, couleur);
    }
}

/* ---------- Simulation de forces ---------- */

static void simuler(const GrapheGUI *g)
{
    /* Répulsion entre tous les nœuds */
    for (int i = 0; i < g->nbNoeuds; i++) {
        for (int j = i + 1; j < g->nbNoeuds; j++) {
            Noeud *a = g->noeuds[i];
            Noeud *b = g->noeuds[j];
            Vector2 v = Vector2Subtract(a->position, b->position);
            float dist = Vector2Length(v);
            if (dist < 1.0f) dist = 1.0f;
            Vector2 force = Vector2Scale(Vector2Normalize(v),
                                         SIM_REPULSION / (dist * dist));
            a->position = Vector2Add(a->position, force);
            b->position = Vector2Subtract(b->position, force);
        }
    }

    /* Attraction le long des arcs */
    for (int i = 0; i < g->nbFleches; i++) {
        Noeud *a = g->fleches[i]->depart;
        Noeud *b = g->fleches[i]->arrivee;
        Vector2 v = Vector2Subtract(a->position, b->position);
        float dist = Vector2Length(v);
        if (dist == 0) continue;
        Vector2 force = Vector2Scale(Vector2Normalize(v),
                                     -SIM_RESSORT * (dist - SIM_DISTANCE) * SIM_AMORTISSEMENT);
        a->position = Vector2Add(a->position, force);
        b->position = Vector2Subtract(b->position, force);
    }

    /* Garder les nœuds dans la fenêtre */
    for (int i = 0; i < g->nbNoeuds; i++) {
        Noeud *n = g->noeuds[i];
        n->position.x = Clamp(n->position.x, NOEUD_RAYON, GetScreenWidth()  - NOEUD_RAYON);
        n->position.y = Clamp(n->position.y, NOEUD_RAYON, GetScreenHeight() - NOEUD_RAYON);
    }
}

/* ---------- Interaction ---------- */

static Noeud *noeudSousCurseur(const GrapheGUI *g)
{
    Vector2 souris = GetMousePosition();
    for (int i = g->nbNoeuds - 1; i >= 0; i--)
        if (CheckCollisionPointCircle(souris, g->noeuds[i]->position, g->noeuds[i]->cercle->rayon))
            return g->noeuds[i];
    return NULL;
}

static bool dessinerBouton(Bouton b)
{
    Vector2 souris = GetMousePosition();
    bool survol    = CheckCollisionPointRec(souris, b.rect);
    Color couleur  = survol ? ColorBrightness(b.couleur, -0.2f) : b.couleur;

    DrawRectangleRec(b.rect, couleur);
    DrawRectangleLinesEx(b.rect, 2, DARKGRAY);

    int larg = MeasureText(b.label, 12);
    DrawText(b.label,
             b.rect.x + (b.rect.width  - larg) / 2,
             b.rect.y + (b.rect.height - 12)   / 2,
             12, WHITE);

    return survol && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

/* ---------- Boucle principale ---------- */

static void boucle(GrapheGUI *gui, Graphe *graphe)
{
    Noeud    *selection = NULL;
    Resultat *resultat  = NULL;
    const char *nomAlgo = NULL;

    Bouton btnDijkstra = { (Rectangle){ 10,  40, 150, 40 }, "Dijkstra (A)",     DARKGREEN  };
    Bouton btnBellman  = { (Rectangle){ 170, 40, 150, 40 }, "Bellman-Ford (A)", DARKPURPLE };

    while (!WindowShouldClose()) {

        /* Drag & drop des nœuds */
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            selection = noeudSousCurseur(gui);
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selection)
            selection->position = GetMousePosition();
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            selection = NULL;

        simuler(gui);

        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawFPS(10, 10);

        for (int i = 0; i < gui->nbFleches; i++) dessinerFleche(gui->fleches[i]);
        for (int i = 0; i < gui->nbNoeuds;  i++) dessinerNoeud(gui->noeuds[i]);

        if (dessinerBouton(btnDijkstra)) {
            Sommet *s = trouverSommet(graphe, "A");
            if (s) {
                detruireResultat(resultat);
                resultat = dijkstra(graphe, s);
                nomAlgo  = "Dijkstra depuis A";
                colorierChemins(gui, graphe, resultat);
            }
        }

        if (dessinerBouton(btnBellman)) {
            Sommet *s = trouverSommet(graphe, "A");
            if (s) {
                detruireResultat(resultat);
                resultat = bellmanFord(graphe, s);
                nomAlgo  = "Bellman-Ford depuis A";
                colorierChemins(gui, graphe, resultat);
            }
        }

        dessinerPanneauResultats(graphe, resultat, nomAlgo);

        EndDrawing();
    }

    detruireResultat(resultat);
    CloseWindow();
}

/* ---------- Point d'entrée ---------- */

void creerGUI(Graphe *graphe)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(LARGEUR, HAUTEUR, TITRE);
    if (!IsWindowReady()) return;

    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(LARGEUR_MIN, HAUTEUR_MIN);
    SetTargetFPS(FPS_CIBLE);

    GrapheGUI *gui = creerGrapheGUI(graphe);
    boucle(gui, graphe);
}