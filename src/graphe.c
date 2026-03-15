#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphe.h"

/* ---------- Utilitaires ---------- */

/* Compte les occurrences du caractère c dans la chaîne. */
static int compterChar(const char *s, char c)
{
    int n = 0;
    while (*s) { if (*s++ == c) n++; }
    return n;
}

/* Compte le nombre de lignes non vides dans le texte. */
static int compterLignes(const char *texte)
{
    int n = 0;
    char *copie = strdup(texte);
    char *save, *tok = strtok_r(copie, SEP_LIGNE, &save);
    while (tok) {
        if (strlen(tok) > 0) n++;
        tok = strtok_r(NULL, SEP_LIGNE, &save);
    }
    free(copie);
    return n;
}

/* ---------- Lecture fichier ---------- */

char *lireFichier(const char *chemin)
{
    FILE *f = fopen(chemin, "r");
    if (!f) { fprintf(stderr, "Impossible d'ouvrir : %s\n", chemin); return NULL; }

    fseek(f, 0, SEEK_END);
    long taille = ftell(f);
    rewind(f);

    char *texte = malloc(taille + 1);
    if (!texte) { fclose(f); return NULL; }

    int lu = fread(texte, 1, taille, f);
    texte[lu] = '\0';

    fclose(f);
    return texte;
}

/* ---------- Recherche ---------- */

Sommet *trouverSommet(const Graphe *graphe, const char *nom)
{
    for (int i = 0; i < graphe->nbSommets; i++)
        if (strcmp(graphe->sommets[i]->nom, nom) == 0)
            return graphe->sommets[i];
    return NULL;
}

int indiceSommet(const Graphe *graphe, const Sommet *sommet)
{
    for (int i = 0; i < graphe->nbSommets; i++)
        if (graphe->sommets[i] == sommet)
            return i;
    return -1;
}

/* ---------- Construction ---------- */

/* Crée un sommet à partir d'une ligne "NOM;..." */
static Sommet *creerSommet(const char *ligne)
{
    char *copie = strdup(ligne);
    char *nom   = strtok(copie, SEP_PROPRIETE);

    Sommet *s = malloc(sizeof(Sommet));
    s->nom = strdup(nom ? nom : "?");

    free(copie);
    return s;
}

/* Ajoute les arcs décrits dans une ligne au graphe. */
static void ajouterArcs(const char *ligne, Graphe *graphe, int *idx)
{
    char *copie = strdup(ligne);
    char *save1, *save2;

    char *nomDepart = strtok_r(copie, SEP_PROPRIETE, &save1);
    char *reste     = strtok_r(NULL, "", &save1);

    Sommet *depart = trouverSommet(graphe, nomDepart);
    if (!depart || !reste) { free(copie); return; }

    char *bloc = strtok_r(reste, SEP_VOISIN, &save2);
    while (bloc) {
        char *nomArr = strtok(bloc, SEP_VOISIN_COUT);
        char *coutStr = strtok(NULL, SEP_VOISIN_COUT);

        if (nomArr && coutStr) {
            Sommet *arrivee = trouverSommet(graphe, nomArr);
            if (arrivee) {
                Arc *arc = malloc(sizeof(Arc));
                arc->depart  = depart;
                arc->arrivee = arrivee;
                arc->cout    = atoi(coutStr);
                graphe->arcs[(*idx)++] = arc;
            }
        }
        bloc = strtok_r(NULL, SEP_VOISIN, &save2);
    }
    free(copie);
}

Graphe *creerGraphe(const char *texte)
{
    Graphe *g = malloc(sizeof(Graphe));
    g->nbSommets = compterLignes(texte);
    g->nbArcs    = compterChar(texte, SEP_VOISIN_COUT[0]);
    g->sommets   = malloc(g->nbSommets * sizeof(Sommet *));
    g->arcs      = malloc(g->nbArcs    * sizeof(Arc *));

    /* Passe 1 : créer les sommets */
    char *copie = strdup(texte);
    char *save, *ligne = strtok_r(copie, SEP_LIGNE, &save);
    int i = 0;
    while (ligne) {
        if (strlen(ligne) > 0) g->sommets[i++] = creerSommet(ligne);
        ligne = strtok_r(NULL, SEP_LIGNE, &save);
    }
    free(copie);

    /* Passe 2 : créer les arcs */
    copie = strdup(texte);
    ligne = strtok_r(copie, SEP_LIGNE, &save);
    int arcIdx = 0;
    while (ligne) {
        ajouterArcs(ligne, g, &arcIdx);
        ligne = strtok_r(NULL, SEP_LIGNE, &save);
    }
    free(copie);

    g->nbArcs = arcIdx; /* ajuste au nombre réel */
    return g;
}

void detruireGraphe(Graphe *graphe)
{
    for (int i = 0; i < graphe->nbSommets; i++) {
        free(graphe->sommets[i]->nom);
        free(graphe->sommets[i]);
    }
    for (int i = 0; i < graphe->nbArcs; i++)
        free(graphe->arcs[i]);

    free(graphe->sommets);
    free(graphe->arcs);
    free(graphe);
}
