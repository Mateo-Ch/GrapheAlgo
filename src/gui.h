#ifndef _GUI_H_
#define _GUI_H_

#include "raylib.h"
#include "graphe.h"

typedef struct
{
    Vector2 position;
    Color couleur;
    float rayon;
} Cercle;

typedef struct
{
    Cercle *cercle;
    char *texte;
} Noeud;

typedef struct // J'ai pas mis d'épaisseur
{
    Noeud *depart;
    Noeud *arrive;
    Color couleur;
    char *texte;
} Fleche;

typedef struct
{
    int nbNoeuds;
    int nbFleches;

    Noeud **noeuds;
    Fleche **fleches;
} GrapheGUI;

void creerGUI();

#endif