#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define nb_sportif 10 

void menu(char nom[nb_sportif][25]);
void h_athlete(char nom[nb_sportif][25]);
void statistique(char nom[nb_sportif][25]);
void performance(char nom[nb_sportif][25]);
void maj_athlete(char nom[nb_sportif][25], int athlete, int epreuve, int mois,
 int jour, float temps, int nb_entrainement, int cas);


//structure liant sportif à sa moyenne
typedef struct {
    float moyenne;
    int index;
} MoyenneSportif;

//permet d'empêcher une boucle infinie d'erreur et vide le buffer
void clear(){int w;
  while ((w = getchar()) != '\n' && w != EOF) {
  }
}

//fonction pour choisir le temps de l'athlete
int choixtemps(int cas){
char tabcas [2][20] = {"du relais", "de la course"};
int temps;
int testtemps = 1;
  while (testtemps == 1) {
    testtemps = 0;
    printf(
        "\n-------------------------------\n\nTemps %s en secondes : ", tabcas[cas]);
    scanf("%d", &temps);
    if (temps < 0 || temps > 100000) {
      printf("\n-------------------------------\n\n\033[0;31m# Temps incorrect\033[0m\n");
      clear();
      testtemps = 1;
    }
  }
  return temps;
}

//fonction pour choisir le sportif
int choixsportif(char nom[nb_sportif][25], int cas){
  char tabcas[2][50] = {"Choississez un sportif", "Regardez les performances de quel sportif ?"};
  int choix, verif;
    do {
        verif = 0;
        printf("\n-------------------------------\n\n%s\n\n", tabcas[cas]);
          for (int i = 0; i < nb_sportif; i++) {
            printf("%d : %s\n", i + 1, nom[i]);
          }
        printf("\n%d : Retour\n", nb_sportif + 1);
        printf("\nChoix : ");
        scanf("%d", &choix);
          if (choix == nb_sportif + 1) {
            menu(nom);
          }
          if (choix < 1 || choix > nb_sportif + 1) {
            verif = 1;
            printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
            clear();
        }
    } while (verif > 0);
    
  return choix;
}

//fonction pour choisir l'épreuve'
int choixepreuve(){
  int testchoix = 1;
  int choix;
    while (testchoix == 1) {
      testchoix = 0;
      printf("\n-------------------------------\n\nChoix de l'épreuve : \n\n1 : "
             "100m\n2 : 400m\n3 : 5000m\n4 : "
             "Marathon\n\nChoix : ");
      scanf("%d", &choix);
        if (choix < 1 || choix > 4) {
          printf("\n\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
          clear();
          testchoix = 1;
        }
    }
  return choix;
}

//procédure pour choisir la date de l'entrainement
void choixdate(int * mois, int * jour){//choisit et vériie la date de l'entrainement
  int testdate = 1;
    while (testdate == 1) {
      testdate = 0;
      printf("\nEntrer la date mois/jour sous le format MM/JJ : ");
      scanf("%d/%d", &*mois, &*jour);
        if ((*mois < 1 || *mois > 12) || (*jour < 1 || *jour > 31)) {
          printf("\n-------------------------------\n\n\033[0;31m# Date incorrect\033[0m\n\n-------------------------------\n");
          clear();//évite boucle infinie
          testdate = 1;
        }
    }

}

//fonction qui vérifie que l'on ne peut pas faire plusieurs relais le meme jour
int verif_date(int jour, int mois) {
char ligne[100];
int mois2,jour2;
FILE *fichier = fopen("relais", "r");

  if (fichier == NULL) {
      perror("\n\n\033[\n-------------------------------\n\n0;31m# Erreur lors de l'ouverture du fichier\033[0m\n\n");
      exit(1);
  }
  while (fgets(ligne, sizeof(ligne), fichier)) {
    sscanf(ligne, "24/%d/%d", &mois2, &jour2);
      if (mois2==mois && jour2==jour) {
        printf("\n-------------------------------\n\n\033[0;31m# Vous ne pouvez pas faire plusieurs entrainements de relais le même jour\033[0m\n\n-------------------------------\n");
        fclose(fichier);
        return 0;
      }
  }
  fclose(fichier);
  return 1;
}

//fonction qui renvoie la moyenne des valeurs d'un tableau
float t_moyenne(float * tab_temps, int taille) {//calcule la moyenne des temps
  if (taille == 0) return 0;
    float somme = 0;
    for (int i = 0; i < taille; i++) {
        somme += tab_temps[i];
  }
  return somme / taille;
}

//procédure supplémentaire a tri_tableau
void echanger(MoyenneSportif * a, MoyenneSportif * b) {
  MoyenneSportif temp = *a;
  *a = *b;
  *b = temp;
}

//calcul du plus petit au plus grand un tableau
MoyenneSportif * tri_tableau(MoyenneSportif tab[], int taille) {
  MoyenneSportif *tableau_trie = malloc(taille * sizeof(MoyenneSportif));
    if (tableau_trie == NULL) {
        printf("\n-------------------------------\n\n\033[0;31m# Erreur  d'allocation de mémoire\033[0m\n");
        exit(1);
    }
      for (int i = 0; i < taille; i++) {
          tableau_trie[i] = tab[i];
      }
      for (int i = 0; i < taille - 1; i++) {
          int min_index = i;
          for (int j = i + 1; j < taille; j++) {
              if (tableau_trie[j].moyenne < tableau_trie[min_index].moyenne) {
                  min_index = j;
              }
          }
      echanger(&tableau_trie[min_index], &tableau_trie[i]);
    }
  return tableau_trie;
}

//met dans un tableau toute les temps selon l'épreuve choisis d'un athlete
float *tris_stats(char nom[nb_sportif][25], int athlete, int epreuve, int *taille_tab) {
  char tabepreuve[5][10] = {"100m", "400m", "5000m", "Marathon", "Relais"};
  int maxligne = 200;
  int maxcharacter = 25;
  char mot1[maxcharacter];
  char mot2[maxcharacter];
  float temps;
  float *tab_valeur = malloc(sizeof(float) * maxligne);
  int nb_sportif_lignes = 0;
  FILE *fichier = fopen(nom[athlete], "r");
    if (fichier == NULL) {
        perror("\n\n\033[0;31m# Erreur lors de l'ouverture du fichier\033[0m\n\n");
        exit(1);
    }
    while (fscanf(fichier, "%s %s %f", mot1, mot2, &temps) != EOF) {
      if (strcmp(mot2, tabepreuve[epreuve]) == 0) {
          tab_valeur[nb_sportif_lignes] = temps;
          nb_sportif_lignes++;
        }
    }
    fclose(fichier);
    tab_valeur = realloc(tab_valeur, sizeof(float) * nb_sportif_lignes);
    *taille_tab = nb_sportif_lignes;
    return tab_valeur;
}

//met dans un tableau toute les temps selon la date choisis d'un athlete
float *tris_stats_date(char nom[nb_sportif][25], int athlete, int *taille_tab,
                       int mois, int jour, int cas, int epreuve) {
  char tabepreuve[5][10] = {"100m", "400m", "5000m", "Marathon", "Relais"};
  int mois_temp, jour_temp;
  int maxligne = 100;
  char ligne[100];
  char mot[25];
  float temps;
  float *tab_valeur = malloc(sizeof(float) * maxligne);
  int nb_sportif_lignes = 0;
  FILE *fichier = fopen(nom[athlete], "r");

    if (fichier == NULL) {
        perror("\n\n\033[\n-------------------------------\n\n0;31m# Erreur lors de l'ouverture du fichier\033[0m\n\n");
        exit(1);
    }

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (sscanf(ligne, "24/%d/%d %s %f", &mois_temp, &jour_temp, mot, &temps) == 4) {
            if (mois_temp == mois && jour_temp == jour) {
                if (cas == 1) {
                    if (strcmp(mot, tabepreuve[epreuve]) == 0) {
                        tab_valeur[nb_sportif_lignes] = temps;
                        nb_sportif_lignes++;
                    }
                } else {
                    tab_valeur[nb_sportif_lignes] = temps;
                    nb_sportif_lignes++;
                }
            }
        }
    }
    fclose(fichier);
    tab_valeur = realloc(tab_valeur, sizeof(float) * nb_sportif_lignes);
  if (tab_valeur == NULL && nb_sportif_lignes > 0) {
      perror("Erreur de réallocation de mémoire pour tab_valeur");
      exit(1);
  }
    *taille_tab = nb_sportif_lignes;
    return tab_valeur;
}

//permet de voir la progression d'un athelte entre 2 dates dans une epreuve précise
void progression(char nom[nb_sportif][25], int athlete, int epreuve) {
    char tabepreuve[5][10] = {"100m", "400m", "5000m", "Marathon", "Relais"};
    int mois, mois2, jour, jour2;
    int taille_tab1 = 0;
    int taille_tab2 = 0;

    printf("\n-------------------------------\n");

    choixdate(&mois, &jour);
    choixdate(&mois2, &jour2);

    float *tab1 = tris_stats_date(nom, athlete, &taille_tab1, mois, jour, 1, epreuve);
    float *tab2 = tris_stats_date(nom, athlete, &taille_tab2, mois2, jour2, 1, epreuve);

    if (taille_tab1 == 0 || taille_tab2 == 0) {
        printf("\n-------------------------------\n\n\033[0;31m# Pas de temps trouvé par rapport au critéres donnés\033[0m\n");
        performance(nom);
    } else {
        float moyenne1 = t_moyenne(tab1, taille_tab1);
        printf("\n%.2f", moyenne1);
        float moyenne2 = t_moyenne(tab2, taille_tab2);
        printf("\n%.2f", moyenne2);

        char signe;
        if (moyenne2 - moyenne1 > 0) {
            signe = '+';
        } else {
            signe = '\0';
        }

        printf("\n-------------------------------\n\nDifférence de temps de %s entre le %d/%d et le %d/%d au %s : %c%.2fs\n", nom[athlete], mois, jour, mois2, jour2, tabepreuve[epreuve], signe, moyenne2 - moyenne1);

        free(tab1);
        free(tab2);
    }
}

//affiche les détailles des performances des sportifs
void performance(char nom[nb_sportif][25]) {
  int choix, choix2, choix3, choix4, mois, jour, i;
  int taille_max = 0;
  int *taille_tab = &taille_max;
    do {
        i = 0;

        choix = choixsportif(nom, 1);

        FILE *fichier = fopen(nom[choix - 1], "r");
        if (fichier == NULL) {
            perror("\n\n\033[0;31m# Erreur lors de l'ouverture du fichier\033[0m\n\n");
            exit(1);
        }
        fseek(fichier, 0, SEEK_END); // Se déplace à la fin du fichier
        long taille = ftell(fichier); // Vérifie si le fichier est vide
        if (taille == 0) {
            printf("\n-------------------------------\n\n\033[0;31m# Pas d'entrainement en mémoire\033[0m\n");
            fclose(fichier);
            menu(nom);
        }
        fclose(fichier);
    } while (i > 0);

  printf("\n-------------------------------\n\n1 : Statistiques global\n2 : Progression de l'athlete\n\nChoix : ");
  scanf("%d", &choix4);
  
if(choix4==2){
  printf("\n-------------------------------\n\nCette option vous permet de voir la différence de temps (positive ou négative) d'un athlete dans une épreuve entre 2 dates\n");
  int epreuve = choixepreuve();
  progression(nom, choix-1, epreuve-1);
}

else if(choix4==1){
    printf("\n-------------------------------\n\n1 : Par épreuve\n2 : Par date\n\nChoix : ");
    scanf("%d", &choix3);
    if (choix3 == 1) {
        int testchoix = 1;
        while (testchoix == 1) {
            testchoix = 0;
            printf("\n-------------------------------\n\nPerfomance de quel epreuve "
                   ": \n\n1 : 100m\n2 : 400m\n3 : 5000m\n4 : "
                   "Marathon\n5 : Relais\n\nChoix : ");
            scanf("%d", &choix2);
            if (choix2 < 1 || choix2 > 5) {
                printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n\n");
                clear();
                testchoix = 1;
            }
        }

        char tabepreuve[5][10] = {"100m", "400m", "5000m", "Marathon", "Relais"};
        float *stats = tris_stats(nom, choix - 1, choix2 - 1, taille_tab);
        if (taille_max > 0) {
          printf("\n-------------------------------\n\nStatistique de %s au %s : \n",
             nom[choix - 1], tabepreuve[choix2 - 1]);
            printf("\n\nMeilleur temps : %.2fs\nPire temps : %.2fs\nTemps moyen : %.2fs\n",
                   stats[0], stats[taille_max - 1], t_moyenne(stats, taille_max));
            free(stats);
            menu(nom);
        } else {
            printf("\n-------------------------------\n\n\033[0;31m# Aucun temps trouvé pour l'épreuve spécifiée.\033[0m\n");
            free(stats);
            menu(nom);
        }
    } else if (choix3 == 2) {
        int verif;
        do {
            verif = 0;
            printf("\n-------------------------------\n\nVeuillez écrire la date "
                   "(mois/jour) : ");
            scanf("%d/%d", &mois, &jour);
            if ((mois > 0 && mois <= 12) && (jour > 0 && jour <= 31)) {
                float *stats = tris_stats_date(nom, choix - 1, taille_tab, mois, jour, 0, 0);
                if (taille_max > 0) {
                    printf("\n-------------------------------\n\nStatistique de %s le "
                           "24/%d/%d : \n",
                           nom[choix - 1], mois, jour);
                    printf("\n\nMeilleur temps : %.2fs\nPire temps : %.2fs\nTemps moyen : "
                           "%.2fs\n",
                           stats[0], stats[taille_max - 1], t_moyenne(stats, taille_max));
                    free(stats);
                    menu(nom);
                } else {
                    printf("\n-------------------------------\n\n\033[0;31m# Aucun temps trouvé la date spécifiée.\033[0m\n");
                    free(stats);
                    menu(nom);
                }
            } else {
                printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie. Veuillez entrer une date valide.\033[0m\n");
                clear();
                verif = 1;
            }
        } while (verif == 1);
    }
  }
  
  else{
    clear();
    printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
    statistique(nom);
  }
}

//selectionne les meilleurs sportifs par rapport à leurs performances
void selec_jo(char nom[nb_sportif][25]) {
  char tabepreuve[5][10] = {"100m", "400m", "5000m", "Marathon", "Relais"};
  printf("\n");
  int choix;
  int taille_max;
  int *taille_tab = &taille_max;
  MoyenneSportif tab_moyenne[nb_sportif];
  int testchoix = 1;
  
    while (testchoix == 1) {
        testchoix = 0;
        printf("\n-------------------------------\n\nChoix de l'épreuve : \n\n1 : 100m\n2 : 400m\n3 : 5000m\n4 : Marathon\n5 : Relais\n\nChoix : ");
        scanf("%d", &choix);
        if (choix < 1 || choix > 5) {
            printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
            testchoix = 1;
        }
    }
    for (int i = 0; i < nb_sportif; i++) {
        float *stats = tris_stats(nom, i, choix - 1, taille_tab);
        if (taille_max > 0) {
            tab_moyenne[i].moyenne = t_moyenne(stats, taille_max);
        } else {
            tab_moyenne[i].moyenne = -1;
        }
        tab_moyenne[i].index = i;
        free(stats);
    }

    MoyenneSportif *tab_moyenne_trie = tri_tableau(tab_moyenne, nb_sportif);

    int taille_top = 0;
    for (int i = 0; i < nb_sportif; i++) {
      printf("\n%.2f", tab_moyenne[i].moyenne);
        if (tab_moyenne_trie[i].moyenne != -1) {
            taille_top++;
        }
    }

    if (taille_top < 3) {
        printf("\n-------------------------------\n\n\033[0;31m# Pas assez d'athlete assigné à cette épreuve pour y établir un top 3\033[0m\n");
    } else {
      
        int i = 0;
        int j = 0;
        printf("\n-------------------------------\n\n");
        printf("Top 3 du %s :\n", tabepreuve[choix-1]);
      
        while (i < 3) {
          if(tab_moyenne_trie[j].moyenne != -1){
            printf("\n%d : %s avec une moyenne de %.2fs\n", i + 1, nom[tab_moyenne_trie[j].index], tab_moyenne_trie[j].moyenne);
          
            i++;
          }
          j++;
        }
    }

    free(tab_moyenne_trie);
}

//affiche les options de statistique d'un athlete
void statistique(char nom[nb_sportif][25]) {
  int choix, verifvaleur;
    do {
      verifvaleur = 0;
      printf("\n-------------------------------\n\n1 : Perfomances\n2 : "
             "Selection JO\n\n3 : Retour\n\nChoix : ");
      scanf("%d", &choix);
      switch (choix) {
      case 1:
        performance(nom);
        break;
      case 2:
        selec_jo(nom);
        break;
      case 3:
        menu(nom);
        break;
      default:
        printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
        clear();
        verifvaleur = 1;
      }
    } while (verifvaleur == 1);
}

//affiche l'historique d'un athlete
void h_athlete(char nom[nb_sportif][25]) {
  int choix, i;
  choix = choixsportif(nom,0);
  FILE *fichier = fopen(nom[choix - 1], "r");//verifie si le fichier existe
  
    if (fichier == NULL) {
    printf("\n-------------------------------\n\n\033[0;31m# Erreur : Impossible d'ouvrir le fichier.\033[0m\n");
    return;
  }
  fseek(fichier, 0, SEEK_END);  // se déplacer à la fin du fichier
  long taille = ftell(fichier); // obtenir la position du curseur
  if (taille == 0) {
    printf("\n-------------------------------\n\n\033[0;31m# Historique vide\033[0m\n");
    fclose(fichier);
    h_athlete(nom);
  } else {
    printf("\n-------------------------------\n\nHistorique d'entrainement de "
           "%s :\n\n",
           nom[choix - 1]);
    
    // Aller à la fin du fichier
    fseek(fichier, 0, SEEK_END);
    // Récupérer la position actuelle du curseur
    long int position = ftell(fichier);
    // Parcourir le fichier de la fin vers le début
    while (position >= 0) {
      // Déplacer le curseur à la position actuelle
      fseek(fichier, position, SEEK_SET);
      // Lire le caractère à la position actuelle
      int caractere = fgetc(fichier);
      // Si le caractère est un retour à la ligne ou le début du fichier
      if (caractere == '\n' || position == 0) {
        printf("\n");
        // Aller à la prochaine ligne (ou début du fichier)
        long int next_pos = position == 0 ? 0 : position + 1;
        fseek(fichier, next_pos, SEEK_SET);
        // Lire et afficher la ligne
        char *ligne = NULL;
        size_t taille = 0;
        ssize_t taille_lue = getline(&ligne, &taille, fichier);
        if (taille_lue != -1) {
          printf("%s", ligne);
        }
        free(ligne);
      }
      position--;
    }
    fclose(fichier);
    printf("\n----------------------\n\n");
  }
  menu(nom);
}

//crée un fichier relais et le remplis des informations données
void relais(char nom[nb_sportif][25]) {
  int mois, jour, temps;
  int testdate = 1;
  int prenom;
  int verif = 0;
  printf("\n-------------------------------\n\nConfiguration d'entrainement du Relais\n");

  int * pjour = &jour;
  int * pmois = &mois;

  while(verif==0){
  choixdate(pmois,pjour);
  verif = verif_date(jour, mois);
  }

  temps = choixtemps(0);
  
  FILE *fichier = NULL;
  fichier = fopen("relais", "a");//crée un fichier en mode ajout
  if (fichier == NULL) {
    perror("\033[0;31m# Erreur lors de l'ouverture du fichier\033[0m");
    return;
  }
  printf("\n-------------------------------\n\n");
  for (int i = 0; i < nb_sportif; i++) {
    printf("%d : %s\n", i + 1, nom[i]);//affiche les sportifs
  }
  
  int verifdiff = 0;
  int listerelais[4];
  
  for (int i = 0; i < 4; i++) {
    do {
      verifdiff = 0;
      printf("\n-------------------------------\n\nVeuillez notifier le joueur "
             "en position %d : ",//verifie si il y a bien un joueur en position i
             i + 1);
      scanf("%d", &prenom);

      if(prenom<1 || prenom >10){
        verifdiff = 1;
        printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
      }
      
      prenom--;
      listerelais[i] = prenom;
      
      for (int j = 0; j < i; j++) {
        if (listerelais[j] == listerelais[i]) {//compare les athlète
          printf("\n-------------------------------\n\n\033[0;31m# Athlete déjà séléctionné, choississez-en un différent\033[0m\n");
          verifdiff = 1;
        }
      }
    } while (verifdiff == 1);
    maj_athlete(nom, listerelais[i], 5, mois, jour, temps, 1, 1);//affiche les détailles dans le fichier relais
    fprintf(fichier, "24/%d/%d %ds %s %d\n", mois, jour, temps,
            nom[listerelais[i]], i + 1);//affiche les détailles dans le fichier relais
  }
  fprintf(fichier, "----------------------------------------\n");
  fclose(fichier);
  printf("\n-------------------------------\n\n\033[0;32mL'entrainement à été ajouté avec succès\033[0m\n");//confirmation au près de l'utilisateur
  menu(nom);
}

//met à jour les fichier des joueurs avec de nouveaux entrainement
void maj_athlete(char nom[nb_sportif][25], int athlete, int epreuve, int mois,
                 int jour, float temps, int nb_entrainement, int cas) {
  char tabepreuve[5][10] = {"100m    ", "400m    ", "5000m   ", "Marathon", "Relais  "};
  FILE *fichier = NULL;
  fichier = fopen(nom[athlete], "a");
  if (fichier == NULL) {
    perror("\033[0;31m# Erreur lors de l'ouverture du fichier\033[0m");
    return;
  }
  fprintf(fichier, "24/%d/%d %s %.2fs \n----------------------\n", mois, jour,
          tabepreuve[epreuve - 1], temps);
  fclose(fichier);

  if(cas==0){
  if(nb_entrainement == 1){
    printf("\n-------------------------------\n\n\033[0;32mL'entrainement à été ajouté avec succès\033[0m\n");
    menu(nom);
  }
  }
  
}

//configure les entrainement
void entrainement2(char nom[nb_sportif][25], int athlete, int simple_rapide) {
  int choix2, mois, jour;
  float temps;

  printf("\n-------------------------------\n\n");
  printf("Configurer l'entrainement de %s\n\n", nom[athlete]);

  int * pjour = &jour;
  int * pmois = &mois;

  choixdate(pjour,pmois);
  choix2 = choixepreuve();
  if (simple_rapide == 1) {  
    temps = choixtemps(1);  
    maj_athlete(nom, athlete, choix2, mois, jour, temps, 1, 0);

  } else if (simple_rapide == 2) {

    int nb_entrainement;
    float min;
    float max;
    int verif;
    do {
      verif = 0;
      printf("\n-------------------------------\n\nCombien d'entrainement : ");
      scanf("%d", &nb_entrainement);
      if (nb_entrainement <= 0) {
        printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
        clear();
        verif = 1;
      }
    } while (verif == 1);
    do {
      verif = 0;
      printf("\n-------------------------------\n\nTemps minimum : ");
      scanf("%f", &min);
      if (min < 0) {
        printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
        clear();
        verif = 1;
      }
    } while (verif == 1);
    do {
      verif = 0;
      printf("\n-------------------------------\n\nTemps maximum : ");
      scanf("%f", &max);
      if (max < min) {
        printf("\n-------------------------------\n\n\033[0;31m# Le temps maximum doit être supérieur au temps minimum\033[0m\n");
        verif = 1;
      }
    } while (verif == 1);

    for (int i = 0; i < nb_entrainement; i++) {
      float scale = (float)rand() / (float)RAND_MAX;
      temps = min + scale * (max - min);
      maj_athlete(nom, athlete, choix2, mois, jour, temps, nb_entrainement - i, 0);
    }
  }
}

//affiche les possibilités d'entrainements (simple ou rapide)
void entrainement(char nom[nb_sportif][25]) {
  int choix, choix2, verif;
  choix = choixsportif(nom,0);
  do {
    verif = 0;
    printf("\n-------------------------------\n\n1 : Entrainement simple\n2 : "
           "Entrainement rapide\n\nChoix : ");
    scanf("%d", &choix2);
    if (choix2 == 1 || choix2 == 2) {
      entrainement2(nom, choix - 1, choix2);
    } else {
      printf("\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
      clear();
      verif = 1;
    }
  } while (verif == 1);
}

//menu principal
void menu(char nom[nb_sportif][25]) {
int verif = 1;
while (verif) {
    int choix;
    printf("\n-------------------------------\n\nMenu principal\n\n1 : Entrainement\n2 : Relais\n3 : Historique\n4 : Statistique\n\nChoix : ");

    if (scanf("%d", &choix) != 1) {
        printf("\n\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
        clear();
      menu(nom);
    }
        switch (choix) {
        case 1:
            entrainement(nom);
            break;
        case 2:
            relais(nom);
            break;
        case 3:
            h_athlete(nom);
            break;
        case 4:
            statistique(nom);
            break;
        default:
            printf("\n\n-------------------------------\n\n\033[0;31m# Erreur de saisie\033[0m\n");
            clear();
            break;
        }
    }
}

//crée un fichier pour chaque athlète
void creafichier() {
  char nom[nb_sportif][25] = {
      "Antonin Leneuille", "Cékoa Teta", "Diego Polla", "Elioth Pelioth",
      "Eské Jilétu", "Igrèg Bar", "Kylian Mobutu", "Mathieu Golf5",
      "Maxence Alycia", "Oujelen Térici"};
  FILE *adresse[nb_sportif];
  for (int i = 0; i < nb_sportif; i++) {
    adresse[i] = fopen(nom[i], "w");
    if (adresse[i] == NULL) {
      printf("\n-------------------------------\n\n\033[0;31m# Erreur lors de la création du fichier %s\033[0m\n",
             nom[i]);
      return;
    }
  }
  for (int i = 0; i < nb_sportif; i++) {
    fclose(adresse[i]);
  }
  FILE *adresserelais = fopen("relais", "w");
  if (adresserelais == NULL) {
    printf("\n-------------------------------\n\n\033[0;31m# Erreur lors de la création du fichier relais\033[0m\n");
    return;
  }
  fclose(adresserelais);
  menu(nom);
}

//affichage d'acceuil
int main() {
  srand(time(NULL));
  printf("\n\033[0;37m------------------------------------------\n");
  printf("\033[0;34m██████████████\033[0;37m██████████████\033[0;31m██████████████\n\n");
  printf("\033[0;34m CCC    \033[0;33mEEEEE  \033[0;30mRRRR            \033[0;32mJJJ   \033[0;31mOOO  \n");
  printf("\033[0;34mC   C   \033[0;33mE      \033[0;30mR   R  \033[0;34m2\033[0;37m02\033[0;31m4      \033[0;32mJ   \033[0;31mO   O \n");
  printf("\033[0;34mC       \033[0;33mEEEEE  \033[0;30mRRRR  ------     \033[0;32mJ   \033[0;31mO   O \n");
  printf("\033[0;34mC   C   \033[0;33mE      \033[0;30mR R          \033[0;32mJ   J   \033[0;31mO   O \n");
  printf("\033[0;34m CCC    \033[0;33mEEEEE  \033[0;30mR  RR         \033[0;32mJJJ     \033[0;31mOOO   \n\n");
printf("\033[0;34m██████████████\033[0;37m██████████████\033[0;31m██████████████");
printf("\n\033[0;37m------------------------------------------\n");
  creafichier();
  return 0;
}
