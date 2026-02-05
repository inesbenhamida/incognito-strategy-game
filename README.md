# incognito-strategy-game
🕵️ Incognito - Jeu de Stratégie


Incognito est un jeu de plateau tactique développé en langage C où deux joueurs s'affrontent pour capturer l'espion adverse ou infiltrer le château ennemi.

## 👤 Auteure


Inès Benhamida 

## Règles du jeu


Objectif : Faire entrer son espion dans le château adverse ou identifier l'espion de l'autre joueur par une interrogation.


Déplacements : Les pions se déplacent orthogonalement ou diagonalement sur un plateau de 5x5.

Interrogations : Permettent de démasquer l'espion. Si l'interrogateur se trompe, il est retiré du plateau.

##  Points Techniques


Structures de données : Utilisation de structures complexes pour gérer les pions, les cases et l'état global du jeu.


Persistance des données : Système de sauvegarde et de chargement des parties via des fichiers .inco pour reprendre une partie en cours.


Validation de mouvement : Implémentation d'une logique de vérification stricte (obstacles, limites du plateau, règles spécifiques aux châteaux).

## Installation et Lancement Pour compiler le projet :

clang -std=c17 -Wall BENHAMIDA_DAABAK_-Incognito.c -o incognito

Pour lancer une partie avec affichage ASCII et sauvegarde :

./incognito -a -s sauvegarde.inco
