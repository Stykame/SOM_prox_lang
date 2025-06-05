#include <stdio.h>  // printf, fopen, fclose, fgets, fprintf
#include <stdlib.h> // exit, malloc, free, rand
#include <string.h> // strcpy, strcmp
#include <time.h>   // pour le random
#include <dirent.h> // pour compter les fichiers dans un dossier
#include <math.h>   // sqrt, pow
#include <ctype.h>  // pour toupper (hash)

#define TYPE_FICHIER_REG 8                           // Type de fichier régulier
#define DOSSIER_TEXTES "Textes/"                     // Dossier contenant les textes à traiter
#define FICHIER_LOG "fichier_journal"                // Fichier journal contenant les résultats de l'apprentissage
#define LONGUEUR_CARTE 7                             // Nombre de lignes de la carte
#define LARGEUR_CARTE 7                              // Nombre de colonnes de la carte
#define TAILLE_VECTEUR 5000                          // Taille des vecteurs mémoires
#define TAILLE_MAX_NOM_FICHIER 256                   // Taille maximale d'un nom de fichier
#define VALEUR_INIT_MIN 0.000                        // Valeur minimale pour l'initialisation des attracteurs
#define VALEUR_INIT_MAX 0.002                        // Valeur maximale pour l'initialisation des attracteurs
#define NB_LETTRES 3                                 // Nombre de lettres à prendre en compte pour le hash
#define NB_ATTRACTEURS LONGUEUR_CARTE *LARGEUR_CARTE // Nombre d'attracteurs sur la carte

int NOMBRE_ITERATIONS = 200;    // Nombre d'itérations pour l'apprentissage
int iteration_en_cours = 0;     // pour le calcul du voisinage et du facteur d'apprentissage
int TAILLE_VOISINAGE_INITIALE;  // Taille du voisinage initiale (dépend de la taille de la carte)
int TAILLE_VOISINAGE;           // Taille du voisinage pour l'itération en cours (dépend de l'itération)
float ALPHA_INIT = 0.7;         // Facteur d'apprentissage initial
float ALPHA;                    // Facteur d'apprentissage pour l'itération en cours
int ECRIRE_FICHIER_JOURNAL = 0; // Permet de savoir si on doit écrire dans le fichier journal ou non
FILE *fichier_journal;          // Fichier journal contenant les résultats de l'apprentissage

typedef float Attracteur[TAILLE_VECTEUR]; // attracteur modélisé comme un vecteur de float
typedef float Vecteur[TAILLE_VECTEUR];    // vecteur modélisé comme un vecteur de float
typedef struct Position                   // position sur la carte
{
    int x; // ligne
    int y; // colonne
} Position;

Attracteur carte_attracteurs[LONGUEUR_CARTE][LARGEUR_CARTE]; // Carte des attracteurs

int comparer_noms(const void *a, const void *b);                // Fonction de comparaison pour le tri des noms de fichiers
float nombre_aleatoire(float min, float max);                   // Génère un nombre aléatoire entre min et max
void afficher_erreur(char *message);                            // Affiche un message d'erreur et quitte le programme
int compter_fichiers(const char *dossier);                      // Compte le nombre de fichiers dans un dossier
int traiter_texte(const char *nomFichier);                      // Traite un texte
int obtenir_valeur_ascii(char c);                               // Retourne la valeur ascii d'un caractère
int fonction_de_hash(int groupe[NB_LETTRES]);                   // Retourne la valeur de hash d'un groupe de lettres
double calculer_norme(float vecteur[TAILLE_VECTEUR]);           // Calcule la norme d'un vecteur
float calculer_distance(Vecteur entree, Attracteur attracteur); // Calcule la distance entre un vecteur et un attracteur
int obtenir_entier_aleatoire(int min, int max);                 // Retourne un entier aléatoire entre min et max