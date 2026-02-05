#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAILLE 5

typedef enum { BLANC, NOIR } Couleur;
typedef enum { CHEVALIER, ESPION } Type;

typedef struct _pion {
    Type type;
    Couleur couleur;
} Pion;

typedef struct _jeu {
    Pion* plateau[TAILLE][TAILLE];
    Couleur joueur;
} Jeu;

typedef struct {
    int x, y;
} Case;

/**
 Case_position : convertit une case en format texte pour la sauvegarde
 * Paramètres : 
 * c la case à convertir 
 * pos Chaîne de caractères pour stocker la position (exemple : "a3")
 */

void case_position(Case c, char* pos) {
    pos[0] = 'a' + c.y;
    pos[1] = '1' + (TAILLE - 1 - c.x);
    pos[2] = '\0';
}

/**
 Sauvegarder_coup : sauvegarde un coup dans le fichier de sauvegarde.
 * Paramètres : 
 * f Pointeur vers le fichier de sauvegarde.
 * type Le type de coup ('D' pour déplacement, 'I' pour interrogation).
 * depart La case de départ du coup.
 * arrivee La case d'arrivée du coup.
 */
void sauvegarder_coup(FILE* f, char type, Case depart, Case arrivee) {
    char pos_dep[3], pos_arr[3];
    case_position(depart, pos_dep);
    case_position(arrivee, pos_arr);
    fprintf(f, "%c %s->%s\n", type, pos_dep, pos_arr);
}

/**
 Initialiser_plateau : Initialise le plateau de jeu avec 5 pions pour chaque joueur (blanc et noir),
 *  en plaçant les pions sur des positions fixes et en sélectionnant un espion
 *  au hasard dans chaque équipe.
 * 
 * Parmètre:
 * Jeu : un pointeur vers une structure `Jeu` où le plateau sera initialisé avec
 * les pions, et le joueur qui commence sera sélectionné au hasard.
 */
void initialiser_plateau(Jeu* jeu) {
    static Pion tous_les_pions[10];
    srand(time(NULL));

    int positions_blancs[5][2] = {{4, 1}, {4, 2}, {3, 0}, {3, 1}, {2, 0}};
    int positions_noirs[5][2] = {{0, 2}, {0, 3}, {1, 3}, {1, 4}, {2, 4}};

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            jeu->plateau[i][j] = NULL;
        }
    }

    int indice_espion_blanc = rand() % 5;
    int indice_espion_noir = rand() % 5;

    for (int i = 0; i < 5; i++) {
        tous_les_pions[i].couleur = BLANC;
        tous_les_pions[i].type = (i == indice_espion_blanc) ? ESPION : CHEVALIER;
        jeu->plateau[positions_blancs[i][0]][positions_blancs[i][1]] = &tous_les_pions[i];
    }

    for (int i = 0; i < 5; i++) {
        tous_les_pions[i + 5].couleur = NOIR;
        tous_les_pions[i + 5].type = (i == indice_espion_noir) ? ESPION : CHEVALIER;
        jeu->plateau[positions_noirs[i][0]][positions_noirs[i][1]] = &tous_les_pions[i + 5];
    }

    jeu->joueur = (rand() % 2 == 0) ? BLANC : NOIR;
}

/**
 * Afficher_plateau : Affiche l'état actuel du plateau de jeu dans la console. 
 *        Les cases vides sont représentées par '.', les pions blancs par 'B' et les noirs par 'N'.
 * Paramètre :
 * Jeu Un pointeur vers la structure `Jeu` contenant le plateau à afficher.
 */
void afficher_plateau(Jeu* jeu) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (jeu->plateau[i][j] == NULL) {
                printf(". ");
            } else {
                printf("%c ", jeu->plateau[i][j]->couleur == BLANC ? 'B' : 'N');
            }
        }
        printf("\n");
    }
}

/**
 * Deplacement_valide : Vérifie si un déplacement est valide en fonction des règles du jeu.
 * Paramètres :
 * depart La case de départ du pion.
 * arrivee La case d'arrivée du pion.
 * jeu Un pointeur vers la structure `Jeu` contenant le plateau.
 * 
 * Return : int Code de validité du déplacement :
 *              - 1 : Déplacement valide
 *              - 2 : Hors des limites
 *              - 4 : Case déjà occupée
 *              - -2 : Château propre interdit
 *              - -3 : Entrée dans le château adverse non orthogonale
 *              - -1 : Passage au-dessus d'un pion impossible
 */

int deplacement_valide(Case depart, Case arrivee, Jeu* jeu) {
    // Vérifie si la case d'arrivée est hors des limites
    if (arrivee.x < 0 || arrivee.x >= TAILLE || arrivee.y < 0 || arrivee.y >= TAILLE) {
        return 2; 
    }

    // Vérifie si la case d'arrivée est occupée
    if (jeu->plateau[arrivee.x][arrivee.y] != NULL) {
        return 4; // Case déjà occupée
    }

    // Vérifie si le pion essaie d'entrer dans son propre château
    Pion* pion = jeu->plateau[depart.x][depart.y];
    if ((pion->couleur == BLANC && arrivee.x == 4 && arrivee.y == 0) ||
        (pion->couleur == NOIR && arrivee.x == 0 && arrivee.y == 4)) {
        return -2; // Code spécifique pour château propre
    }
    
    // Vérifie si le pion essaie d'entrer dans le château adverse
    if ((pion->couleur == BLANC && arrivee.x == 0 && arrivee.y == 4) ||
        (pion->couleur == NOIR && arrivee.x == 4 && arrivee.y == 0)) {
        // Vérifie si le déplacement est orthogonal (dx ou dy doit être 0, mais pas les deux)
        int dx = abs(arrivee.x - depart.x);
        int dy = abs(arrivee.y - depart.y);
        if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1))) {
            return -3;  // Déplacement non valide pour entrer dans le château
        }
    }


    // Calcul des distances en x et y
    int dx = arrivee.x - depart.x, dy = arrivee.y - depart.y;
    
    // Vérifie que le mouvement est en ligne droite ou en diagonale
    if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) {
        return 0; // Déplacement non valide
    }

    int step_x = (dx != 0) ? dx / abs(dx) : 0;
    int step_y = (dy != 0) ? dy / abs(dy) : 0;

    // Vérifie chaque case intermédiaire pour s'assurer qu'elle est vide
    int x = depart.x + step_x, y = depart.y + step_y;
    while (x != arrivee.x || y != arrivee.y) {
        if (jeu->plateau[x][y] != NULL) {
            return -1;  // Passage au-dessus d'un pion impossible
        }
        x += step_x;
        y += step_y;
    }

    return 1; // Déplacement valide
}

/**
 * Deplacer pion : Déplace un pion du joueur actuel de la case de départ à la case d'arrivée,
 * selon les règles de validité.
 * Paramètres :
 * depart Case de départ du pion.
 * arrivee Case d'arrivée du pion.
 * jeu Pointeur vers la structure `Jeu` contenant le plateau.
 */
void deplacer_pion(Case depart, Case arrivee, Jeu* jeu) {
    if (jeu->plateau[depart.x][depart.y] == NULL) {
        printf("Erreur : Il n'y a pas de pion à la case de départ.\n");
        return;
    }

    if (jeu->plateau[depart.x][depart.y]->couleur != jeu->joueur) {
        printf("Erreur : Vous ne pouvez déplacer que vos propres pions.\n");
        return;
    }

    int validite = deplacement_valide(depart, arrivee, jeu);
    if (validite == 1) {
        Pion* pion = jeu->plateau[depart.x][depart.y];

         // Vérifie si le pion entre dans le château adverse
        if ((pion->couleur == BLANC && arrivee.x == 0 && arrivee.y == 4) ||
            (pion->couleur == NOIR && arrivee.x == 4 && arrivee.y == 0)) {
            if (pion->type == ESPION) {
                printf("Victoire ! Le joueur %s a gagné en entrant dans le château adverse.\n", 
                       jeu->joueur == BLANC ? "Blanc" : "Noir");
                exit(0);  // Fin de la partie
            } else {
                printf("Le pion est perdu en entrant dans le château adverse.\n");
                jeu->plateau[depart.x][depart.y] = NULL;  // Supprime le pion
                return;  // Arrête la fonction ici, le pion est perdu
            }
        }

        jeu->plateau[arrivee.x][arrivee.y] = jeu->plateau[depart.x][depart.y];
        jeu->plateau[depart.x][depart.y] = NULL;
    } else if (validite == -2) {
        printf("Erreur : Vous ne pouvez pas entrer dans votre propre château.\n");
    } else if (validite == -3) {
        printf("Le pion est perdu en entrant dans le château adverse.\n");
        jeu->plateau[depart.x][depart.y] = NULL;  // Supprime le pion qui est perdu
    } else if (validite == -4) {
        printf("Erreur : Entrée dans le château adverse possible uniquement de manière orthogonale.\n");
    } else if (validite == 2) {
        printf("Erreur : La case d'arrivée est hors des limites du plateau.\n");
    } else if (validite == 4) {
        printf("Erreur : La case d'arrivée est déjà occupée.\n");
    } else if (validite == -1) {
        printf("Erreur : Passage au-dessus d'un pion impossible.\n");
    } else {
        printf("Déplacement non valide.\n");
    }
}

/**
 * Interroger_pion : Interroge un pion adverse pour vérifier s'il est un espion.
 * Paramètres :
 * interrogateur Case de l'interrogateur.
 * interroge Case du pion interrogé.
 * jeu Pointeur vers la structure `Jeu`.
 */
void interroger_pion(Case interrogateur, Case interroge, Jeu* jeu) {
    Pion* pionInterroge = jeu->plateau[interroge.x][interroge.y];

    if (pionInterroge->type == ESPION) {
        printf("Espion adverse trouvé ! Le joueur %s a gagné.\n", jeu->joueur == BLANC ? "Blanc" : "Noir");
        exit(0);
    } else {
        printf("Le pion interrogé n'est pas un espion. Le pion interrogateur est perdu.\n");
        jeu->plateau[interrogateur.x][interrogateur.y] = NULL;
    }
}

/**
 Sauvergarder_partie : sauvegarde l'état actuel de la partie dans un fichier.
 * 
 * Enregistre la position initiale des espions, le joueur en cours, et chaque coup (déplacement
 * ou interrogation). La partie est clôturée et le fichier est fermé quand un coup final (type 'F') 
 * est reçu.
 * 
 * Paramètres : 
 * jeu Pointeur vers la structure `Jeu` contenant l'état de la partie.
 * fichier Nom du fichier de sauvegarde.
 * type_coup Type de coup ('D' pour déplacement, 'I' pour interrogation, 'F' pour fin de partie).
 * depart Case de départ pour le coup.
 * arrivee Case d'arrivée pour le coup.
 * 
 * Exemple : 
 * sauvegarder_partie(&jeu, "sauvegarde.inco", 'D', (Case){2, 3}, (Case){2, 4});
 */

void sauvegarder_partie(Jeu* jeu, const char* fichier, char type_coup, Case depart, Case arrivee) {
    static FILE* f = NULL;
    if (!f) {
        f = fopen(fichier, "a");
        if (!f) {
            perror("Erreur d'ouverture du fichier de sauvegarde");
            return;
        }

        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                Pion* p = jeu->plateau[i][j];
                if (p && p->type == ESPION) {
                    char pos[3];
                    case_position((Case){i, j}, pos);
                    fprintf(f, "%c %s\n", (p->couleur == BLANC) ? 'B' : 'N', pos);
                }
            }
        }
        fprintf(f, "%c\n", (jeu->joueur == BLANC) ? 'B' : 'N');
    }

    if (type_coup == 'D' || type_coup == 'I') {
        sauvegarder_coup(f, type_coup, depart, arrivee);
    }
    if (type_coup == 'F') {
        fprintf(f, "%c\n", (jeu->joueur == BLANC) ? 'B' : 'N');
        fclose(f);
        f = NULL;
    }
}



/**
 Charger_partie  Charge une partie depuis un fichier de sauvegarde.
 * 
 * Restaure le plateau et le joueur en cours à partir du fichier de sauvegarde.
 * 
 * jeu Pointeur vers la structure `Jeu` pour stocker l'état restauré.
 * fichier Nom du fichier de sauvegarde.
 * 
 * Exemple : 
 * charger_partie(&jeu, "sauvegarde.inco");
 */

void charger_partie(Jeu* jeu, const char* fichier) {
    FILE* f = fopen(fichier, "r");
    if (!f) {
        perror("Erreur d'ouverture du fichier de sauvegarde");
        return;
    }

    // Initialise le plateau vide
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            jeu->plateau[i][j] = NULL;
        }
    }

    char ligne[10];
    while (fgets(ligne, sizeof(ligne), f)) {
        if (ligne[0] == 'J') {
            jeu->joueur = (ligne[7] == 'B') ? BLANC : NOIR;
        } else {
            Couleur couleur = (ligne[0] == 'B') ? BLANC : NOIR;
            Type type = (ligne[2] == 'E') ? ESPION : CHEVALIER;
            int x = TAILLE - (ligne[5] - '1') - 1;
            int y = ligne[4] - 'a';

            jeu->plateau[x][y] = malloc(sizeof(Pion));
            jeu->plateau[x][y]->type = type;
            jeu->plateau[x][y]->couleur = couleur;
        }
    }

    fclose(f);
    printf("Partie chargée depuis le fichier %s\n", fichier);
}



/**
 * Main :  Fonction principale du jeu Incognito.
 * 
 * Initialise une nouvelle partie ou charge une sauvegarde, puis lance la boucle de jeu.
 * Les joueurs peuvent déplacer un pion ou interroger un pion adverse, et chaque action est
 * enregistrée dans un fichier de sauvegarde.
 * 
 * Arguments :
 * - `-c <fichier>` : Charge une partie depuis le fichier spécifié.
 * - `-s <fichier>` : Spécifie le fichier de sauvegarde pour enregistrer les coups.
 * 
 * Paramètres :
 * argc Nombre d'arguments de la ligne de commande.
 * argv Tableau des arguments de la ligne de commande.
 * 
 * @return 0 en cas de succès, 1 si le fichier de sauvegarde est manquant.
 * 
 * Exemple d'utilisation :
 * `./inco -c sauvegarde.inco -s nouvelle_sauvegarde.inco`
 */

// Fonction principale
int main(int argc, char *argv[]) {
    Jeu jeu;
    char* fichier_sauvegarde = NULL;

    if (argc > 2 && strcmp(argv[1], "-c") == 0) {
        charger_partie(&jeu, argv[2]);
    } else {
        initialiser_plateau(&jeu);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            fichier_sauvegarde = argv[i + 1];
        }
    }

    if (fichier_sauvegarde == NULL) {
        printf("Erreur : fichier de sauvegarde manquant.\n");
        return 1;
    }

    printf("Bienvenue dans Incognito !\n");
    afficher_plateau(&jeu);

    while (1) {
        char choix;
        Case depart, arrivee;
        printf("Joueur %s, déplacement (d) ou interrogation (i) ? ", jeu.joueur == BLANC ? "Blanc" : "Noir");
        scanf(" %c", &choix);
        if (choix == 'd') {
            printf("Déplacement (x1 y1 -> x2 y2) : ");
            scanf("%d %d %d %d", &depart.x, &depart.y, &arrivee.x, &arrivee.y);
            deplacer_pion(depart, arrivee, &jeu);
            sauvegarder_partie(&jeu, fichier_sauvegarde, 'D', depart, arrivee);
        } else if (choix == 'i') {
            printf("Interrogation (x1 y1 -> x2 y2) : ");
            scanf("%d %d %d %d", &depart.x, &depart.y, &arrivee.x, &arrivee.y);
            interroger_pion(depart, arrivee, &jeu);
            sauvegarder_partie(&jeu, fichier_sauvegarde, 'I', depart, arrivee);
        }
        jeu.joueur = (jeu.joueur == BLANC) ? NOIR : BLANC;
        afficher_plateau(&jeu);
    }

    return 0;
}
