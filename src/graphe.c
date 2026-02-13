#include "graphe.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#define SEPARATEUR_LIGNE "\n"
#define SEPARATEUR_SOMMETS ";"
#define SEPARATEUR_DATA ","

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
    char *text_array;
    
    file = fopen(fichier, "r");

    if (file == NULL)
    {
        printf("Allo mec tu joues à quoi concentres toi tia pas ouvert le fichier, tié dyslexiquue le sang de ta mère ?");
        exit(-1);
    }

    size = getTailleFichier( file );
    
    text_array = malloc(size * sizeof(char) + 1);
    fread(text_array, 1, size, file);
    text_array[size] = '\0';
    fclose(file);
    
    return text_array;
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

Sommet creerSommet( char* texte )
{
    Sommet sommet = {0};

    char *position;
    char *token = strtok_r(texte, SEPARATEUR_SOMMETS, &position);
    if ( token == NULL ) { return sommet; }

    // NOM
    sommet.nom = malloc(strlen(token) + 1);
    if ( sommet.nom == NULL ) { return sommet; }

    strcpy(sommet.nom, token);

    // COÛT
    token = strtok_r(NULL, SEPARATEUR_SOMMETS, &position);
    if ( token == NULL ) { return sommet; }

    sommet.cout = atoi(token);

    return sommet;
}

Graphe creerGraphe( char* texte )
{
    Graphe graphe = {0};
    char* position;
    char* token = strtok_r(texte, SEPARATEUR_LIGNE, &position);

    while ( token != NULL )
    {
        graphe.taille++;

        // rien fonctionne vraiment mais en gros
        /*
            - tu dois recuppe la taille du graphe
            - malloc( taille * sizeof(Sommets) + 1 );
            - tous les foutre a la suite dans le pointeur puis ATTENTION ajouter a la fin \0
            - Maintenant qu'ils sont creer (les sommets) il faut faire les liens );
        */

        token = strtok_r(NULL, SEPARATEUR_LIGNE, &position);
    }

    free(texte);

    return graphe;
}


int main()
{
    creerGraphe( getContenuFichier( "../res/test.gph" ) );
    return 0;
}