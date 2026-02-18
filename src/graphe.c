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
    if ( g == NULL ) return;

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

VoisinPoids* getListeAretesDansLigne(char* l, int* nbAretes)
{
    if ( l == NULL || nbAretes == NULL ) return NULL;
    
    *nbAretes = 0;
    char* copie = strdup(l);
    
    int nbPointsVirgules = compterNbChar(';', copie);
    if (nbPointsVirgules == 0) {
        free(copie);
        return NULL;
    }
    
    VoisinPoids* listeAretes = malloc(nbPointsVirgules * sizeof(VoisinPoids));
    if (listeAretes == NULL) {
        free(copie);
        return NULL;
    }
    
    char* saveptr;
    char* token = strtok_r(copie, SEP_PROPRIETE, &saveptr);
    
    token = strtok_r(NULL, SEP_PROPRIETE, &saveptr);
    
    int index = 0;
    while (token != NULL)
    {
        while (*token && isspace((unsigned char)*token)) token++;
        
        if (*token == '\0') {
            break;
        }
        
        char* virgule = strchr(token, ',');
        if (virgule != NULL)
        {
            *virgule = '\0';
            char* nomVoisin = token;
            char* poidsStr = virgule + 1;
            
            listeAretes[index].nom = strdup(nomVoisin);
            listeAretes[index].poids = atoi(poidsStr);
            index++;
        }
        
        token = strtok_r(NULL, SEP_PROPRIETE, &saveptr);
    }
    
    *nbAretes = index;
    free(copie);
    
    if (index == 0) {
        free(listeAretes);
        return NULL;
    }
    
    return listeAretes;
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
          int nbAretes = 0;
            VoisinPoids* listeAretes = getListeAretesDansLigne(ligne, &nbAretes);
            
            if ( listeAretes != NULL )
            {
                sommet->nbSuivants = nbAretes;
                sommet->suivants = malloc(nbAretes * sizeof(Arete)); 
                
                for (int i = 0; i < nbAretes; i++)
                {
                    Sommet* voisin = getSommetDepuisNom(listeAretes[i].nom, graphe);
                    
                    if (voisin == NULL)
                    {
                        printf("Voisin inconnu: %s\n", listeAretes[i].nom);
                    }
                    else
                    {
                        sommet->suivants[i].destination = voisin; 
                        sommet->suivants[i].poids = listeAretes[i].poids; 
                    }
                    
                    free(listeAretes[i].nom);
                }
                
                free(listeAretes);
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
            Arete* arete = &s->suivants[j]; 
            
            if (arete->destination != NULL) 
            {
                printf("%s(%d) ", arete->destination->nom, arete->poids); 
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