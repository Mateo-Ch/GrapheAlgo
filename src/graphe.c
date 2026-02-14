#include "graphe.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#define SEP_NOEUD      "\n"
#define SEP_PROPRIETE  ";"
#define SEP_VOISIN     ","

// FICHIER

size_t getTailleFichier( FILE *file )
{
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

char* getContenuFichier( char* fichier )
{
    FILE *file;
    size_t size;
    char *texte;
    
    file = fopen(fichier, "r");

    if (file == NULL)
    {
        printf("Allo mec tu joues à quoi concentres toi tia pas ouvert le fichier, tié dyslexiquue le sang de ta mère ?");
        exit(-1);
    }

    size = getTailleFichier( file );
    texte = malloc(size * sizeof(char) + 1);
    if (texte == NULL) exit(-1);

    fread(texte, 1, size, file);
    texte[size] = '\0';
    fclose(file);
    
    return texte;
}

// GRAPHE

int* genererTableauCouts( Graphe *graphe, Sommet source )
{
    int *d;
    
    d = malloc( graphe->taille * sizeof(int) );
    if ( d == NULL ) { return NULL; }

    for ( int i = 0; i < graphe->taille; i++ )
    {
        if ( strcmp( graphe->sommets[i].nom, source.nom ) == 0 )
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

// La aussi j'ai chiéééé

// int getNombreChar( char* texte, char c )
// {
//     int n = 0;

//     while ( texte != NULL && *texte != '\0' )
//     {
//         if ( *texte == c ) { n++; }
//     }
    
//     return n;
// }

Sommet* getSommetDepuisNom( char* nom, Graphe* graphe )
{
    for (size_t i = 0; i < graphe->taille; i++)
    {
        if ( strcmp(nom, graphe->sommets[i].nom) == 0 )
        {
            return &(graphe->sommets[i]);
        }
    }

    return NULL;
}

int getTailleGraphe( char* texte )
{
    int taille = 0;
    char* position;
    char* copie = strdup(texte);
    char* token = strtok_r(copie, SEP_NOEUD, &position);

    while (token != NULL)
    {
        if (strlen(token) > 0) { taille++; }
        token = strtok_r(NULL, SEP_NOEUD, &position);
    }
    
    free(copie);

    return taille;
}

void creerSommets( char* texte, Graphe* graphe )
{
    char* ligne;
    char* pos;
    int indice = 0;

    ligne = strtok_r(texte, SEP_NOEUD, &pos);
    
    while ( ligne != NULL )
    {
        Sommet sommet = {0};
        char *token;

        token = strtok(ligne, SEP_PROPRIETE);
        if ( token == NULL ) { exit(-1); }

        // NOM
        sommet.nom = strdup( token );

        // COÛT
        token = strtok(NULL, SEP_PROPRIETE);
        if ( token != NULL )
        {
            sommet.cout = atoi(token);
        }
        else
        {
            sommet.cout = 0;
        }

        sommet.nbSuivants = 0;
        sommet.suivants = NULL;

        graphe->sommets[indice] = sommet;

        // Sommet suivant
        indice++;
        ligne = strtok_r(NULL, SEP_NOEUD, &pos);
    }
}

void lierSommets(char* texte, Graphe* graphe)
{
    // J'ai chié dans la fonction là


    // char* ligne;
    // char* save_ligne;
    
    // ligne = strtok_r(texte, SEP_NOEUD, &save_ligne);
    
    // while (ligne != NULL)
    // {
    //     char* txt = strrchr(ligne, ';');

    //     if (txt != NULL)
    //     {
    //         txt++;

    //         if (*txt != '\0')
    //         {
    //             char* suivant;
    //             char* save_sommet;

    //             suivant = strtok_r(txt, SEP_VOISIN, &save_sommet);

    //             printf("%d\n", getNombreChar(txt, ',') + 1);

    //             while (suivant != NULL)
    //             {
    //                 printf("voisin = %s\n", suivant);

    //                 suivant = strtok_r(NULL, SEP_VOISIN, &save_sommet);
    //             }
    //         }
    //     }

    //     ligne = strtok_r(NULL, SEP_NOEUD, &save_ligne);
    // }
}

Graphe creerGrapheDepuisFichier( char* texte )
{
    Graphe graphe = {0};
    char* copie;

    // Taille du graphe
    graphe.taille = getTailleGraphe( texte );
    graphe.sommets = malloc( graphe.taille * sizeof(Sommet) );

    // Création des sommets (sans les suivants)
    copie = strdup( texte );
    creerSommets( copie, &graphe );
    free(copie);

    // Création des liens
    copie = strdup( texte );
    lierSommets( copie, &graphe );
    free(copie);

    free(texte);

    return graphe;
}

int main()
{
    Graphe g = creerGrapheDepuisFichier( getContenuFichier( "./res/test.gph" ) );

    for (int i = 0; i < g.taille; i++)
    {
        Sommet* s = &g.sommets[i];

        printf("%s -> ", s->nom);

        for (int j = 0; j < s->nbSuivants; j++)
        {
            printf("%s ", s->suivants[j]->nom);
        }

        printf("\n");
    }
    
    return 0;
}