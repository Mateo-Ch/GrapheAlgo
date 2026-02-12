#include "graphe.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

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

Graphe creerGraphe( char* texte )
{
    Graphe graphe =           {0};
    int indice    =           (0);
    int tailleText= strlen(texte);

    while ( texte[indice] == '\0')
    {
        if ( texte[indice] == '\n' ) { graphe.taille++; }
        char *token = strtok();

    }
}

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
    text_array = (char*) malloc(size * sizeof(char) + 1);
    
    fread(text_array, 1, size, file);
    text_array[size] = '\0';
    fclose(file);
    
    return text_array;
}

int main()
{

    return 0;
}