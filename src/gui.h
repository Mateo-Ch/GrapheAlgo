#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include "graphe.h"

typedef struct {
    Color  couleur;
    float  rayon;
} Cercle;

typedef struct {
    Cercle  *cercle;
    Vector2  position;
    char    *texte;
} Noeud;

typedef struct {
    Noeud  *depart;
    Noeud  *arrivee;
    Color   couleur;
    char   *texte;
} Fleche;

typedef struct {
    int      nbNoeuds;
    int      nbFleches;
    Noeud  **noeuds;
    Fleche **fleches;
} GrapheGUI;

typedef struct {
    Rectangle   rect;
    const char *label;
    Color       couleur;
} Bouton;

void creerGUI(Graphe *graphe);

#endif
