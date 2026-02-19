#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphe.h"
#include "utils.h"

// Utile pour resoudre des graphes

int* genererTableauCouts( Graphe *graphe, Sommet *source )
{
    int *d;
    
    d = malloc( graphe->nbElements * sizeof(int) );
    if ( d == NULL ) { return NULL; }

    for ( int i = 0; i < graphe->nbElements; i++ )
    {
        if ( strcmp( graphe->sommets[i]->nom, source->nom ) == 0 )
        {
            d[i] = 0;
        }
        else
        {
            d[i] = INT_MAX;
        }
    }

    return d;
}

// Utile pour les graphes

int getTailleGraphe( char* texte )
{
    int taille = 0;
    char* position;
    char* copie = strdup(texte);
    char* token = strtok_r(copie, SEP_LIGNE, &position);

    while (token != NULL)
    {
        if (strlen(token) > 0) { taille++; }
        token = strtok_r(NULL, SEP_LIGNE, &position);
    }
    
    free(copie);
    return taille;
}

Sommet* trouverSommet( Graphe *graphe, char *nom )
{
    if ( graphe == NULL || strlen(nom) == 0 ) { return NULL; }

    for (int i = 0; i < graphe->nbElements; i++)
    {
        if ( strcmp( graphe->sommets[i]->nom, nom ) == 0 )
        {
            return graphe->sommets[i];
        }
    }

    return NULL;
}

void afficherGraphe( Graphe *graphe )
{
    printf("GRAPHE, %d sommet(s)\n", graphe->nbElements);
    for (int i = 0; i < graphe->nbElements; i++)
    {
        printf("\tSOMMET: %s\n", graphe->sommets[i]->nom);
    }

    printf("Liens : %d\n", graphe->nbArcs);
    for (int i = 0; i < graphe->nbArcs; i++)
    {
        printf("\tLIEN: (%s) --[%d]-> (%s)\n", graphe->arcs[i]->sDep->nom, graphe->arcs[i]->cout, graphe->arcs[i]->sArr->nom);
    }
}

void detruireGraphe( Graphe *graphe )
{
    for (int i = 0; i < graphe->nbElements; i++)
    {
        free(graphe->sommets[i]->nom);
        free(graphe->sommets[i]);
    }

    for (int i = 0; i < graphe->nbArcs; i++)
    {
        free(graphe->arcs[i]);
    }

    free(graphe->sommets);
    free(graphe->arcs);
    free(graphe);
}

// Création de GRAPHE

Sommet* creerSommet( char *txt )
{
    if ( strlen(txt) == 0 || txt == NULL )
    {
        return NULL;
    }

    Sommet *sommet = (Sommet*) malloc(sizeof(Sommet));
    char *copieLigne = strdup(txt);
    char *nom = strtok(copieLigne, SEP_PROPRIETE);
    
    if (nom)
    {
        sommet->nom = strdup(nom);
    }
    else
    {
        sommet->nom = strdup("Inconnu");
    }
    
    free(copieLigne);
    return sommet;
}

void creerArcs(char *lng, Graphe *graphe, int *indexArc) // Ajout du pointeur d'index
{
    if (graphe == NULL || lng == NULL) { return; }

    char *copieLigne = strdup(lng);
    char *savePtrLigne;
    
    char *nomDepart = strtok_r(copieLigne, SEP_PROPRIETE, &savePtrLigne);
    Sommet *sDep = trouverSommet(graphe, nomDepart);
    
    char *reste = strtok_r(NULL, "", &savePtrLigne);
    
    if (sDep != NULL && reste != NULL)
    {
        char *savePtrVoisin;
        char *blocVoisin = strtok_r(reste, SEP_VOISIN, &savePtrVoisin);
        
        while (blocVoisin != NULL)
        {
            char *nomArrivee = strtok(blocVoisin, SEP_VOISIN_COUT);
            char *poidsStr = strtok(NULL, SEP_VOISIN_COUT);
            
            if (nomArrivee && poidsStr)
            {
                Sommet *sArr = trouverSommet(graphe, nomArrivee);
                int cout = atoi(poidsStr);
                
                if (sArr != NULL)
                {
                    Arc *nouvelArc = (Arc*) malloc(sizeof(Arc));
                    nouvelArc->sDep = sDep;
                    nouvelArc->sArr = sArr;
                    nouvelArc->cout = cout;
                    
                    graphe->arcs[*indexArc] = nouvelArc;
                    (*indexArc)++;
                }
            }
            blocVoisin = strtok_r(NULL, SEP_VOISIN, &savePtrVoisin);
        }
    }
    free(copieLigne);
}

Graphe* creerGraphe( char *txt )
{
    char *position;
    int indice = 0;
    int nbArcs = 0;
    char *texte = strdup( txt );
    int tailleGraphe = getTailleGraphe( texte );
    char *ligne = strtok_r( texte, SEP_LIGNE, &position );

    Graphe *graphe = (Graphe*) malloc(sizeof(Graphe));
    graphe->nbElements = tailleGraphe;
    graphe->sommets = (Sommet**) malloc( graphe->nbElements * sizeof(Sommet*) );

    while ( ligne != NULL )
    {
        if ( strlen(ligne) > 0 )
        {
            graphe->sommets[indice++] = creerSommet( ligne );
        }
        nbArcs += compterNbChar( ligne, SEP_VOISIN_COUT[0] );
        ligne = strtok_r( NULL, SEP_LIGNE, &position );
    }

    graphe->nbArcs = nbArcs;
    graphe->arcs = (Arc**) malloc(graphe->nbArcs * sizeof(Arc*) );
    if ( graphe->arcs == NULL ) { return NULL; }

    free(texte);
    texte = strdup( txt );
    ligne = strtok_r( texte, SEP_LIGNE, &position );

    int arcCourant = 0;
    while ( ligne != NULL )
    {
        creerArcs( ligne, graphe, &arcCourant ); 
        ligne = strtok_r( NULL, SEP_LIGNE, &position );
    }

    free( texte );
    return graphe;
}

// MAIN

int main( int argc, char *argv[] )
{
    if ( argc != 2 ) { return -1; }

    char *texte = getContenuFichier( argv[1] );
    if ( texte == NULL ) { return -1; }

    Graphe *graphe = creerGraphe( texte );

    free(texte);
    detruireGraphe(graphe);
    
    return 0;
}