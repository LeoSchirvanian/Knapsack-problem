#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TAILLE_MAX 1000
#define TIME 1.0

//Groupe : SCHIRVANIAN Léo, NEVEROV Cyprien, BEYE Papa, KNIFASS Chihab

int main(int argc, char *argv[]){

    //INITIALISATION DES VALEURS
    int n = 0;
    getSizeVector(argv[1], &n); //On récupère la taille des vecteurs (argv[1] : premier paramètre de l'éxécutable)

    int c[n];
    int b = 0;
    int a[n];
    readFile(argv[1], &n, &b, &c, &a); //On recupère les vecteurs (argv[1] : premier paramètre de l'éxécutable)

    double ratio[n];
    for(int i =0; i< n; i++){
        ratio[i] = (double)c[i] / (double)a[i];
    }

    ////On trie à bulle le tableau ratio par ordre décroissant et on ordonne les tableaux a et c selon le tri de ratio
    bubbleSort(ratio, n, a, c);

    //1) Borne supérieure par une implémentation simplifiée de l’algorithme du simplexe à ce problème.
    double borneSup = 0.0;
    methodeSimplexe(a, c, n, b, &borneSup);
    printf("Borne sup : %f   ", borneSup);

    //2) Premier minorant par l’heuristique gloutonne du meilleur ratio qualité/prix.
    double borneInf0 = 0.0;
    algoGlouton(a, c, n, b, &borneInf0);
    printf("Borne inf0 : %f   ", borneInf0);

	
    //3) Implémenter une première énumération naïve et donner un deuxième minorant obtenu par recherche exhaustive limitée à 60 secondes CPU.
    double borneInf1 = 0.0;
    enumNaive(a, c, &n, b, &borneInf1);
    printf("Borne inf1 : %f   ", borneInf1);
	
	
    //4) Implémenter la programmation dynamique qui s’applique parfaitement à ce problème et afficher les valeurs optimales de chacune des instances
    int opt = 0;
    dynamique(a, c, n, b, &opt);
    printf("OPT : %d   \n", opt);
	
    return 0;
}


//Prend un fichier et donne et indique la taille des vecteurs via un pointeur sur n
void getSizeVector(const char *filename, int* n){
    // *** INITIALISATION : RÉCUPÉRATION DES DONNÉES DU FICHIER ***
    FILE* fichier = NULL;

    //Ouverture du fichier
    fichier = fopen(filename, "r");

    //Si l'ouverture s'est bien déroulée
    if(fichier != NULL){

        //On place le curseur au début du fichier
        fseek(fichier, 0, SEEK_SET);

        printf("%s   ", filename);
        //Taille des données
        //On récupère la taille des données
        fscanf(fichier, "%d", n);

        fclose(fichier);

    }

    //Si l'ouverture a échoué, on renvoie une erreur
    else{
        //Erreur
        printf("Erreur ouverture du fichier");
    }
}


//Prend un fichier dont on connait la taille des vecteurs et on recupere les differentes données via des pointeurs
void readFile(const char *filename, int* n, int* b, int* c, int* a){

    // *** INITIALISATION : RÉCUPÉRATION DES DONNÉES DU FICHIER ***
    FILE* fichier = NULL;

    //Ouverture du fichier
    fichier = fopen(filename, "r");

    //Si l'ouverture s'est bien déroulée
    if(fichier != NULL){

        //On place le curseur au début du fichier
        fseek(fichier, 0, SEEK_SET);

        //Taille des données
        //On récupère la taille des données
        fscanf(fichier, "%d", n);

        // c = Valeur des objets
        //On récupère les valeurs des objets dans un tableau de int
        for(int i =0; i<*n; i++){
            fscanf(fichier, "%d ", c+i);
        }

        // b = Capacité max
        //On récupère la capacité max
        fscanf(fichier, "%d", b);

        // Volume des objets
        //On récupère le volume des objets dans un tableau d'int
        for(int i =0; i< *n; i++){
            fscanf(fichier, "%d ", a+i);
        }

        fclose(fichier);

    }

    //Si l'ouverture a échoué, on renvoie une erreur
    else{
        //Erreur
        printf("Erreur ouverture du fichier");
    }
}


//tri à bulle un tableau par ordre décroissant selon le ratio et on y associe ce tri par les valeurs de a et c correspondantes
void bubbleSort(double ratio[], int n, int c[], int a[]){
   for (int i=0; i < n-1; i++){
       for (int j=0; j < n-i-1; j++){
           if (ratio[j] < ratio[j+1]){
              swapDouble(&ratio[j], &ratio[j+1]);
              swapInt(&c[j], &c[j+1]);
              swapInt(&a[j], &a[j+1]);
            }
        }
    }
}


//méthode du simplexe qui permet d'avoir la borne superieure
void methodeSimplexe(int a[], int c[], int n, int b, double* borneSup){

    int compt = 0;
    int i = 0;

    do{
        *borneSup += (double)c[i];
        compt += a[i];
        i++;
    }while(compt < b);
    i--;

    compt -= a[i];
    *borneSup -= c[i];

    //Calcul fraction restante
    int reste = b - compt;
    double frac = (double)reste / (double)a[i];

    *borneSup += ( (double)c[i] * frac );
}


//Algorithme glouton qui permet d'avoir une borne inférieure
void algoGlouton(int a[], int c[], int n, int b, double* borneInf0){

    int compt = 0;
    int i = 0;

    do{
        *borneInf0 += (double)c[i];
        compt += a[i];
        i++;
    }while(compt < b);

    i--;
    compt -= a[i];
    *borneInf0 -= c[i];

    int reste = b - compt;
    double frac = (double)reste / (double)a[n-1];

    *borneInf0 += ( (double)c[n-1] * frac );

}

//Calcul toutes les possibilités de manière naive jusqu'à 60 secondes CPU et renvoie le maximum qu'elle a pu calculer durant ce temps de calul CPU
void enumNaive(int a[], int c[], int* n, int b, double* borneInf1){

    //Calcul temps CPU
    clock_t start, end;
    double cpu_time_used = 0.0;

    //Sortie de boucle, au cas où on calcul toutes les possibilités avant les 60 secondes CPU
    double inv = 0;

    //création et initialisation d'un tableau de char de taille n : vecteur de décision
    char x[TAILLE_MAX+1] = ""; //tableau de taille 1000 au max (pas de variable dans initialisation de tableau de char)
    x[*n] = "\0";              //on place la fin de la table à l'indice n
    for(int i = 0; i<*n; i++){ //on place des 0 de l'indice à 0 à n-2
        x[i] = '0';
        if(i == *n-1){         //et un 1 à l'indice n-1
            x[i] = '1';
        }
    }


    //On limite le temps de calcul à 60 secondes CPU ou à 2*10^n calcul (toutes les possibilités)
    do{
        start = clock();

        int compt = 0;
        int volum = 0;
        incremeteBinaire(x, *n);  //crée un a un tous les nombres binaires de taille n dans notre tableau de char : tous les vecteurs de décisions possibles
        for(int i = 0; i<*n; i++){
            if(x[i] == '1'){      //s'il y a un 1
                compt += c[i];     //on ajoute le c correspondant dans notre compteur de valeur
                volum += a[i];     //on ajoute le a correspondant dans notre compteur de volume
            }
        }

        if(volum <= b){             //si la contrainte de volume est respectée
            if(compt > *borneInf1){ //si le compteur de valeur est le plus grand qu'on ai jamais calculé
                *borneInf1 = compt; //alors il devient notre nouvelle borne inférieure
            }
        }
        inv++;

        end = clock();
        cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC; //incrémentation du calcul de temps CPU à chaque boucle

    }while( (cpu_time_used < TIME) && (inv < pow(2,*n)) );

    //printf("\ncpu_time_used : %f s  inv : %f\n", cpu_time_used, inv); //On limite le temps de calcul à 60 secondes CPU ou à 2*10^n calcul (toutes les possibilités)
}


//algorithme dynamique
void dynamique(int a[], int c[], int n, int b, int* opt){
    //Initialisation de la matrice 
    int i, j;
    int** mat = (int**) malloc(n*sizeof (int*) );
    for (i=0; i<n; i++){
    	mat[i] = (int*) malloc (b*sizeof (int) );
    }

    //Initialisation de la première ligne de la matrice
    for(j=0; j<b; j++){
	if(j < a[0]){
		mat[0][j] = 0;
	}
	else{
		mat[0][j] = c[0];
	}
    }

    //Remplissage de la matrice
    for(i = 1; i<n; i++){
    	for(j=0; j<b; j++){
		if(j >= a[i]){
			if(mat[i-1][j] > mat[i-1][j-a[i]] + c[i]){
				mat[i][j] = mat[i-1][j];
			}
			else{
				mat[i][j] = mat[i-1][j-a[i]] + c[i];
			}
		}
		else
		{
			mat[i][j] = mat[i-1][j];
		}
	}
    }
    *opt = mat[n-1][b-1];

    //Libération de la mémoire
    for (i=0; i<n; i++){
	free (mat[i]);
    }
    free(mat);

}


//échange les valeurs de deux pointeur int en entrée
void swapInt(int *xp, int *yp){
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


//échange les valeurs de deux pointeur double en entrée
void swapDouble(double *xp, double *yp){
    double temp = *xp;
    *xp = *yp;
    *yp = temp;
}


//Crée un a un tous les nombres binaires de taille n dans notre tableau de char = tous les vecteurs de décisions possibles de taille n
void incremeteBinaire(char x[TAILLE_MAX], int n){
    //Initialisation
    int queUn = 0;
    int queZero = 0;
    //On parcourt le tableau de char de taille n
    for(int i = 0 ; i<n; i++){
        if(x[i]=='1'){  // si on détecte un 1
            queUn = 1;   //detecte s'il y a que des 1 après
            queZero = 1; //détecte s'il y a que des 0 après
            for(int j = i+1; j<n; j++){ //On parcourt tous le tableau de char après ce 1 détecté
                if(x[j] == '0'){
                    queUn = 0;   //si on détecte un 0 dans la suite il n'y a pas que des 1
                }
                if(x[j] == '1'){
                    queZero = 0; //si on détecte un 1 dans la suite il n'y a pas que des 0
                }
            }
            if(queUn == 1){ //si il n'y a que des 1 dans la suite, on les mets tous à 0 y compris le 1 détecté, et celui avant ce dernier est mis à 1
                for(int k = i; k<n; k++){
                    x[k] = '0';
                }
                x[i-1] = '1';
                break;  //On sort de la boucle
            }
            if(queZero == 1){ //s'il n'y a que des 0 on mets un 1 au dernier caractère
                x[n-1] = '1';
                break; //On sort de la boucle
            }
        }
    }
    //Cas particulier
    if(queUn == 0 && queZero == 0){
            x[n-1] = 1;
    }
}


/*              
                 RÉSULTATS POUR LES 25 FICHIERS DE DONNÉES

dataset      |   UB            |     LB0        |     LB1        |   OPT
----------------------------------------------------------------------------------
P15_0.dat    |   386.829       |     362        |     364        |   364
P15_1.dat    |   418.647       |     400        |     394        |   394 
P15_2.dat    |   429.118       |     399        |     408        |   408 
P15_3.dat    |   444.909       |     436        |     444        |   444
----------------------------------------------------------------------------------
P56_0.dat    |   33 319.212    |     33 257     |     15 651     |   33 187
P56_1.dat    |   33 623.981    |     33 256     |     15 250     |   33 199
P56_2.dat    |   32 689.773    |     32 448     |     16 498     |   32 467
P56_3.dat    |   32 713.121    |     32 417     |     15 521     |   32 478
----------------------------------------------------------------------------------
P100_0.dat   |   39 121.077    |     38 955     |     11 042     |   39 109
P100_1.dat   |   34 476.770    |     34 208     |     12 079     |   34 406
P100_2.dat   |   39 662.969    |     39 388     |     11 028     |   39 558
P100_3.dat   |   36 425.390    |     36 231     |     12 017     |   36 329
----------------------------------------------------------------------------------
P250_0.dat   |   92 647.936    |     92 566     |     12 336     |   92 597
P250_1.dat   |   88 058.524    |     87 579     |     12 036     |   88 021
P250_2.dat   |   85 973.460    |     85 932     |     12 336     |   85 939
P250_3.dat   |   88 309.790    |     87 908     |     13 513     |   88 286
----------------------------------------------------------------------------------
P500_0.dat   |   183 556.473   |     183 175    |     12 422     |   183 541
P500_1.dat   |   178 052.371   |     177 982    |     12 683     |   178 032
P500_2.dat   |   186 866.004   |     186 450    |     12 739     |   186 831
P500_3.dat   |   178 877.398   |     178 367    |     11 680     |   178 851
----------------------------------------------------------------------------------
p1000_1.dat  |   456 387.384   |     455 994    |     488        |   456 379
p1000_2.dat  |   460 862.079   |     460 830    |     192        |   460 846
p1000_3.dat  |   399 438.504   |     399 194    |     319        |   399 432
p1000_4.dat  |   393 428.728   |     393 371    |     226        |   393 420
p1000_5.dat  |   410 045.842   |     409 570    |     271        |   410 033
----------------------------------------------------------------------------------
*/
