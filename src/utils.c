#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

int compterNbChar( char* ligne, char c )
{
    if ( !ligne ) { return 0; }

    int nbr = 0;
    int len = strlen(ligne);

    for (int i=0; i < len; i++)
    {
        if ( ligne[i] == c )
        {
            nbr++;
        }
    }

    return nbr;
}

// Lecture de fichier

char* getContenuFichier( char* fichier )
{
    FILE *file = fopen(fichier, "r");

    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier: %s\n", fichier);
        exit(-1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *texte = malloc(size * sizeof(char) + 1);
    if (texte == NULL)
    {
        fclose(file);
        exit(-1);
    }

    int tailleLus = fread(texte, 1, size, file);
    texte[tailleLus] = '\0';

    fclose(file);
    return texte;
}