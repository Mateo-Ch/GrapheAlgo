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

char* getContenuFichier( char* fichier )
{
    FILE *file = fopen(fichier, "r");

    if (file == NULL)
    {
        printf("Allo mec tu joues à quoi concentres toi tia pas ouvert le fichier, tié dyslexiquue le sang de ta mère ?");
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

    fread(texte, 1, size, file);
    for (long i = 0; i < size; i++)
    {
        if (texte[i] == '\r') texte[i] = '\n';
    }
    texte[size] = '\0';

    fclose(file);
    return texte;
}

// GRAPHE

void detruireGraphe( Graphe* g )
{
    for (int i = 0; i < g->taille; i++)
    {
        free(g->sommets[i].nom);
        free(g->sommets[i].suivants);
    }
    
    free(g->sommets);
}

int compterNbChar( char c, char* ligne )
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

char* getNomSommetDansLigne( char* l )
{
    if ( l == NULL || strlen(l) == 0 ) { return NULL; }
    
    char* copie = strdup( l );

    char* tmp = strtok( copie, SEP_PROPRIETE );
    if ( tmp == NULL )
    {
        free(copie);
        return NULL;
    }

    char* nom = strdup( tmp );
    
    free(copie);
    return nom;
}

int getCoutSommetDansLigne( char* l )
{
    if ( l == NULL || strlen(l) == 0 ) { return 0; }

    char* copie = strdup( l );
    strtok( copie, SEP_PROPRIETE );

    char* tmp = strtok( NULL, SEP_PROPRIETE );
    if ( tmp == NULL )
    {
        free(copie);
        return 0;
    }

    int cout = atoi( tmp );

    free(copie);
    return cout;
}

char** getListeNomSuivantDansLigne( char* l )
{
    if ( l == NULL ) return NULL;
    char* copie = strdup( l );
    char* dernierPointVirgule = strrchr(copie, ';');
    
    if ( dernierPointVirgule == NULL || strlen(dernierPointVirgule + 1) == 0 )
    {
        free(copie);
        return NULL;
    }

    char* fin = dernierPointVirgule + 1;

    while (*fin && isspace((unsigned char)*fin)) fin++;

    if (*fin == '\0') {
        free(copie);
        return NULL;
    }

    printf("LIGNE : %s / FIN : %s / TAILLE : %lld\n", copie, fin, strlen(fin));

    int nbVoisins = compterNbChar(',', fin) + 1;

    char** liste = malloc( (nbVoisins + 1) * sizeof(char*) );
    if ( liste == NULL )
    {
        free(copie);
        return NULL;
    }

    int i = 0;
    char* token = strtok(fin, ",");

    while ( token != NULL )
    {
        liste[i++] = strdup(token);
        token = strtok(NULL, ",");
    }

    liste[i] = NULL;
    
    free(copie);
    return liste;
}

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

void creerSommets( char* txt, Graphe* graphe )
{
    char* texte = strdup(txt);
    char* ligne;
    char* position;
    int indice = 0;

    ligne = strtok_r(texte, SEP_NOEUD, &position);
    
    while ( ligne != NULL )
    {
        Sommet sommet = {0};

        sommet.nom = getNomSommetDansLigne( ligne );
        sommet.cout = getCoutSommetDansLigne( ligne );
        
        sommet.nbSuivants = 0;
        sommet.suivants = NULL;

        graphe->sommets[indice] = sommet;

        // Sommet suivant
        indice++;
        ligne = strtok_r(NULL, SEP_NOEUD, &position);
    }

    free(texte);
}

void relierSommets(char* txt, Graphe* graphe)
{
    char* texte = strdup(txt);
    char* save_ligne;
    char* ligne = strtok_r(texte, SEP_NOEUD, &save_ligne);
    
    while ( ligne != NULL )
    {
        char* nomSommet = getNomSommetDansLigne(ligne);
        Sommet* sommet = getSommetDepuisNom(nomSommet, graphe);
        free(nomSommet);

        if ( sommet != NULL )
        {
            char** listeSuivant = getListeNomSuivantDansLigne(ligne);
            if ( listeSuivant != NULL )
            {
                int nbSommets = 0;
                while ( listeSuivant[nbSommets] != NULL ) { nbSommets++; }

                sommet->nbSuivants = nbSommets;
                sommet->suivants = malloc(nbSommets * sizeof(Sommet*));

                for (int i = 0; i < nbSommets; i++)
                {
                    Sommet* voisin = getSommetDepuisNom(listeSuivant[i], graphe);
                    if (voisin == NULL)
                    {
                        printf("Voisin inconnu: %s\n", listeSuivant[i]);
                    }
                    else
                    {
                        sommet->suivants[i] = voisin;
                    }

                    free(listeSuivant[i]);
                }

                free(listeSuivant);
            }
        }

        ligne = strtok_r(NULL, SEP_NOEUD, &save_ligne);
    }

    free(texte);
}

Graphe creerGrapheDepuisFichier( char* texte )
{
    Graphe graphe = {0};

    // Taille du graphe
    graphe.taille = getTailleGraphe( texte );
    graphe.sommets = malloc( graphe.taille * sizeof(Sommet) );
    
    creerSommets( texte, &graphe ); // Création des sommets (sans les suivants)
    relierSommets( texte, &graphe ); // Création des liens

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
            if (s->suivants[j] != NULL) 
            {
                printf("%s ", s->suivants[j]->nom);
            }
            else
            {
                printf("?!");
            }
        }

        printf("\n");
    }

    detruireGraphe( &g );

    return 0;
}