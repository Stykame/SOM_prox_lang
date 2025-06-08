#include "projet.h"

// Fonction principale
int main()
{
    int i, y, z, nb_fichiers = 0; // compteurs
    srand(time(0));               // initialisation du random

    // Initialisation des attracteurs
    for (i = 0; i < LONGUEUR_CARTE; i++)
        for (y = 0; y < LARGEUR_CARTE; y++)
            for (z = 0; z < TAILLE_VECTEUR; z++)
                carte_attracteurs[i][y][z] = nombre_aleatoire(VALEUR_INIT_MIN, VALEUR_INIT_MAX); // On initialise les attracteurs avec des valeurs aléatoires

    nb_fichiers = compter_fichiers(DOSSIER_TEXTES);          // On compte le nombre de fichiers dans le dossier Textes
    char noms_fichiers[nb_fichiers][TAILLE_MAX_NOM_FICHIER]; // Tableau contenant les noms de fichiers
    DIR *d;                                                  // Pointeur vers le dossier
    struct dirent *dir;                                      // Structure contenant les informations sur le fichier
    i = 0;                                                   // Réinitialisation du compteur
    d = opendir(DOSSIER_TEXTES);                             // Ouverture du dossier

// On récupère les noms des fichiers
if (d) // Si le dossier existe
{
    while ((dir = readdir(d)) != NULL) // Tant qu'il y a des fichiers dans le dossier
    {
        char chemin_complet[PATH_MAX];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", DOSSIER_TEXTES, dir->d_name);

        struct stat st;
        if (stat(chemin_complet, &st) == 0 && S_ISREG(st.st_mode)) // Si c'est un fichier régulier
        {
            strcpy((char *)&noms_fichiers[i++], dir->d_name); // On copie le nom du fichier dans le tableau
        }
    }
    closedir(d); // Fermeture du dossier
}

    TAILLE_VOISINAGE_INITIALE = sqrt(pow(LONGUEUR_CARTE, 2) + pow(LARGEUR_CARTE, 2)); // Calcul de la taille du voisinage initiale
    qsort(noms_fichiers, nb_fichiers, TAILLE_MAX_NOM_FICHIER, comparer_noms);         // On trie les noms de fichiers par ordre alphabétique

    // Boucle de traitement des textes
    for (iteration_en_cours = 0; iteration_en_cours < NOMBRE_ITERATIONS; iteration_en_cours++) // Pour chaque itération
    {
        TAILLE_VOISINAGE = TAILLE_VOISINAGE_INITIALE * (1 - ((double)iteration_en_cours) / NOMBRE_ITERATIONS); // Calcul de la taille du voisinage pour l'itération en cours
        ALPHA = ALPHA_INIT * (1 - ((double)iteration_en_cours) / NOMBRE_ITERATIONS);                           // Calcul du facteur d'apprentissage pour l'itération en cours
        traiter_texte(noms_fichiers[obtenir_entier_aleatoire(0, nb_fichiers - 1)]);                            // On traite un texte aléatoire
    }

    // Écriture du fichier journal
    fichier_journal = fopen(FICHIER_LOG, "w"); // Ouverture du fichier journal
    if (fichier_journal == NULL)               // Si le fichier n'a pas pu être ouvert
    {
        printf("Erreur d'ouverture du fichier journal\n"); // On affiche un message d'erreur
        exit(1);                                           // On quitte le programme
    }
    fprintf(fichier_journal, "*** Distance des vecteurs pour chaque entrée ***\n\n"); // On écrit l'entête du fichier journal
    ECRIRE_FICHIER_JOURNAL = 1;                                                       // On active l'écriture dans le fichier journal
    for (i = 0; i < nb_fichiers; i++)                                                 // Pour chaque fichier
        traiter_texte(noms_fichiers[i]);                                              // On traite le texte
    fclose(fichier_journal);                                                          // Fermeture du fichier journal

    return 0; // Fin du programme
}

// Fonction de comparaison pour le tri des noms de fichiers
int comparer_noms(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b); // On compare les deux noms de fichiers
}

// Retourne un nombre aléatoire entre min et max
float nombre_aleatoire(float min, float max)
{
    return (float)rand() / (float)RAND_MAX * (max - min) + min; // Retourne un nombre aléatoire entre min et max
}

// Affiche un message d'erreur et quitte le programme
void afficher_erreur(char *message)
{
    printf("%s\n", message); // On affiche le message d'erreur
    exit(1);                 // On quitte le programme
}

// Compte le nombre de fichiers dans un dossier
int compter_fichiers(const char *dossier)
{
    int nb_fichiers = 0;             // Compteur
    DIR *d;                          // Pointeur vers le dossier
    struct dirent *dir;             // Structure contenant les infos du fichier
    d = opendir(dossier);           // Ouverture du dossier
    char message[100];              // Message d'erreur

    if (d) // Si le dossier existe
    {
        while ((dir = readdir(d)) != NULL) // Tant qu'il y a des fichiers dans le dossier
        {
            char chemin_complet[PATH_MAX];
            snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", dossier, dir->d_name);

            struct stat st;
            if (stat(chemin_complet, &st) == 0 && S_ISREG(st.st_mode)) // Si c'est un fichier régulier
            {
                nb_fichiers++; // On incrémente le compteur
            }
        }
        closedir(d); // Fermeture du dossier
    }
    else // Si le dossier n'existe pas
    {
        snprintf(message, sizeof(message), "Erreur d'ouverture du dossier '%s'", dossier);
        afficher_erreur(message); // On affiche un message d'erreur
    }

    return nb_fichiers; // On retourne le nombre de fichiers
}

// Traitements à effectuer sur un texte
int traiter_texte(const char *nomFichier)
{
    // On ouvre le fichier
    char *chemin = malloc(TAILLE_MAX_NOM_FICHIER * 2); // Chemin du fichier
    strcpy(chemin, DOSSIER_TEXTES);                    // On copie le chemin du dossier
    strcat(chemin, nomFichier);                        // On ajoute le nom du fichier
    FILE *fichier = fopen(chemin, "r");                // Ouverture du fichier
    if (fichier == NULL)                               // Si le fichier n'a pas pu être ouvert
    {
        printf("Erreur lors de l'ouverture du fichier %s\n", chemin);
        exit(1); // On quitte le programme
    }

    // On lit et stocke des fréquences dans le vecteur de fréquences du texte
    long occurrences[TAILLE_VECTEUR] = {0}; // Tableau des occurrences
    int nb_groupe = 0;                      // Nombre de groupes de lettres
    int groupe[NB_LETTRES];                 // Groupe de lettres
    int c = 'a';                            // Caractère lu
    while (c != EOF)                        // Tant qu'on n'est pas à la fin du fichier
    {
        for (int i = 0; i < NB_LETTRES && c != EOF; i++) // Pour chaque groupe de lettres
        {
            groupe[i] = c = fgetc(fichier);     // On lit un caractère
            if (c == '\n' || c == '\t')         // Si le caractère est un retour à la ligne ou une tabulation
                c = groupe[i] = fgetc(fichier); // On lit un autre caractère
        }
        nb_groupe++;                                 // On incrémente le nombre de groupes de lettres
        if (c != EOF)                                // Si on n'est pas à la fin du fichier
            occurrences[fonction_de_hash(groupe)]++; // On incrémente le nombre d'occurrences du groupe de lettres
    }

    // Calcul du vecteur de fréquences
    float frequences[TAILLE_VECTEUR] = {0};  // Vecteur de fréquences
    for (int i = 0; i < TAILLE_VECTEUR; i++) // Pour chaque groupe de lettres
    {
        frequences[i] = ((float)occurrences[i]) / ((float)nb_groupe); // On calcule la fréquence
    }

    // On normalise le vecteur de fréquences pour obtenir un vecteur d'entrée
    Vecteur entree;                                 // Vecteur d'entrée
    double norme = calculer_norme(frequences);      // On calcule la norme du vecteur de fréquences
    for (int i = 0; i < TAILLE_VECTEUR; i++)        // Pour chaque groupe de lettres
        entree[i] = ((float)frequences[i]) / norme; // On normalise la fréquence

    fclose(fichier); // Fermeture du fichier

    // Détermination de la position de l'attracteur le plus proche
    Position pos_gagnante; // Position de l'attracteur le plus proche
    pos_gagnante.x = -1;   // Initialisation de la position
    pos_gagnante.y = -1;
    float distance_min, d;                                                               // Distance minimale et distance courante
    distance_min = TAILLE_VECTEUR;                                                       // Initialisation de la distance minimale
    for (int i = 0; i < LONGUEUR_CARTE; i++)                                             // Pour chaque ligne de la carte
        for (int y = 0; y < LARGEUR_CARTE; y++)                                          // Pour chaque colonne de la carte
            if (distance_min > (d = calculer_distance(entree, carte_attracteurs[i][y]))) // Si la distance est inférieure à la distance minimale
            {
                pos_gagnante.x = i; // On met à jour la position de l'attracteur le plus proche
                pos_gagnante.y = y;
                distance_min = d; // On met à jour la distance minimale
            }

    if (ECRIRE_FICHIER_JOURNAL) // Si le drapeau est à 1
        goto journal;           // On écrit dans le fichier journal

    // On parcourt la carte à une distance < TAILLE_VOISINAGE de l'attracteur gagnant et on modifie les vecteurs d'attracteurs en fonction de la formule de Kohonen
    for (int i = 0; i < LONGUEUR_CARTE; i++)    // Pour chaque ligne de la carte
        for (int y = 0; y < LARGEUR_CARTE; y++) // Pour chaque colonne
        {
            int dist = sqrt(pow(i - pos_gagnante.x, 2) + pow(y - pos_gagnante.y, 2)); // On calcule la distance entre l'attracteur et la position courante
            if (dist < TAILLE_VOISINAGE)                                              // Si la distance est inférieure à la taille du voisinage
            {
                for (int z = 0; z < TAILLE_VECTEUR; z++) // Pour chaque groupe de lettres
                    carte_attracteurs[i][y][z] += ALPHA * (entree[z] - carte_attracteurs[i][y][z]) *
                                                  (1 - ((float)dist) / (2 * TAILLE_VOISINAGE)); // On modifie le vecteur de l'attracteur
            }
        }

    return 0;

journal:                                          // On écrit dans le fichier journal si le drapeau est à 1
    printf("%s\n", nomFichier);                   // On affiche le nom du fichier
    fprintf(fichier_journal, "%s\n", nomFichier); // On écrit le nom du fichier

    for (int i = 0; i < LONGUEUR_CARTE; i++) // Pour chaque ligne de la carte
    {
        for (int y = 0; y < LARGEUR_CARTE; y++) // Pour chaque colonne de la carte
        {
            // On encadre l'attrateur gagnant de "!" et les autres de "[]" pour les différencier
            if (i != pos_gagnante.x || y != pos_gagnante.y) // Si l'attracteur n'est pas le gagnant
                fprintf(fichier_journal, "[%.2f] ", calculer_distance(entree, carte_attracteurs[i][y]));
            else // Si l'attracteur est le gagnant
                fprintf(fichier_journal, "!%.2f! ", calculer_distance(entree, carte_attracteurs[i][y]));
        }
        fprintf(fichier_journal, "\n"); // On saute une ligne
    }
    fprintf(fichier_journal, "\n");

    return 0;
}

// On utilise la méthode de Horner pour calculer le hash en modulant par la taille du vecteur les groupes de lettres
int fonction_de_hash(int groupe[NB_LETTRES])
{
    int res = 0;                         // Résultat
    int c;                               // Caractère courant
    for (int i = 0; i < NB_LETTRES; i++) // Pour chaque groupe de lettres
    {
        c = obtenir_valeur_ascii(groupe[i]);     // On récupère la valeur numérique du caractère
        for (int y = 1; y < NB_LETTRES - i; y++) // Pour chaque caractère à droite du caractère courant
            c *= 26;                             // Valeur numérique du caractère
        res += c;                                // On ajoute la valeur numérique du caractère au résultat
    }
    return res % TAILLE_VECTEUR; // On module le résultat par la taille du vecteur
}

// Fonction qui renvoie la valeur numérique d'un caractère
int obtenir_valeur_ascii(char c)
{
    return toupper(c) - 'A';
}

// Fonction qui renvoie la norme d'un vecteur
double calculer_norme(float vecteur[TAILLE_VECTEUR])
{
    int i;                                   // Compteur
    double norme = 0;                        // Norme du vecteur
    for (i = 0; i < TAILLE_VECTEUR; i++)     // Pour chaque élément du vecteur
        norme += pow((double)vecteur[i], 2); // On ajoute le carré de l'élément à la norme
    norme = sqrt(norme);                     // On calcule la racine carrée de la norme

    return norme; // On renvoie la norme
}

// Fonction qui calcule la distance euclidienne entre un vecteur d'entrée et un attracteur
float calculer_distance(Vecteur entree, Attracteur attracteur)
{
    float distance = 0;                      // La distance euclidienne est la racine carrée de la somme des carrés des différences entre les éléments des vecteurs
    for (int i = 0; i < TAILLE_VECTEUR; i++) // Pour chaque élément des vecteurs

        distance += (entree[i] - attracteur[i]) * (entree[i] - attracteur[i]); // On ajoute le carré de la différence entre les éléments des vecteurs
    distance = sqrt(distance);                                                 // On calcule la racine carrée de la distance

    return distance; // On renvoie la distance
}

// Renvoie un entier aléatoire compris entre min et max inclus
int obtenir_entier_aleatoire(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}
