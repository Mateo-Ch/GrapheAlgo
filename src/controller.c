#include <stdio.h>
#include <stdlib.h>

#include "graphe.h"
#include "gui.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <fichier_graphe>\n", argv[0]);
        return 1;
    }

    char *texte = lireFichier(argv[1]);
    if (!texte) return 1;

    Graphe *graphe = creerGraphe(texte);
    free(texte);

    creerGUI(graphe);

    detruireGraphe(graphe);
    return 0;
}
