#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <time.h>

/* attention compare a des coordonnees classiques qui commencent en bas a gauche
les coordonnees ici commencent en haut a gauche et se comportent comme pour une matrice 

/* le nom de la structure pour les case et cas parce que apparement il existe une fonction c qui s'appelle case*/

struct cas {
unsigned int N;
unsigned int S;
unsigned int E;
unsigned int O;
unsigned int etat;
unsigned int x;
unsigned int y;
};

struct labyrinthe {
unsigned int hauteur;
unsigned int longueur;
struct cas** mat;
};

struct stack {
unsigned int size ; //taille de la pile
unsigned int sp ;  //nombre d'element empil�s
struct cas* data ;  //tableau des valeurs de la pile dernier élément = dessus de la pile
};



struct cas pop ( struct stack* P)
{
if ((*P).sp == 0) {
printf("pile vide!") ;
}
(*P).sp-- ;
return ((*P).data[(*P).sp]);
}

void push ( struct stack *P , struct cas val ) {
if (P -> sp == P->size) {
/* Pile pleine . */
P->size *= 2 ;
P->data =  (struct cas*)realloc (P->data, P->size) ;
}
P -> data [P -> sp ] = val ;
P -> sp ++ ;
}

struct labyrinthe new_lab(int n,int m) {
    struct cas** matrice= (struct cas**)malloc(n*sizeof(struct cas*));
    int i;
    int j;
    for(i=0;i<n;i++) {
        matrice[i]= (struct cas*)malloc(m*sizeof(struct cas));
        for(j=0;j<m;j++) {
            struct cas new_case;
            new_case.N=1;
            new_case.S=1;
            new_case.O=1;
            new_case.E=1;
            new_case.etat=0;
            new_case.x=i;
            new_case.y=j;
            matrice[i][j]=new_case;
        }

    }
    struct labyrinthe new_laby;
    new_laby.hauteur=n;
    new_laby.longueur=m;
    new_laby.mat=matrice;
    return new_laby;
}
/* 
On veut crer un labyrinthe parfait. C'est à dire que pour tout couple de cases on a a un unique chemin qui les relis.
Construction du labyrinthe:
-On intialise un labyrinthe avec que des murs (quadriage)
-On prend une case au hazard que l'on commence à etudier.
    -de cette case on en prend une adjacente possible au hazard
    -on casse le mur entre la nouvelle case et celle que l'on etudie
        - puis on etudie la nouvelle case de la meme maniere 
            - et ainsi de suite ...
                -quand la nouvelle case à toute ses cases adjacentes déja traitées on traite de nouveau la case 
                d'avant jusqu'à que toutes les cases soit traités

 */






struct labyrinthe construction(int n, int m) {

    // Initialisation du labirinthe avec que des cases d'etat 0(non traité)
    struct labyrinthe laby=new_lab(n,m);

    /* On cree une pile des cases du laby à traiter elle contiend au plus toute les cases 
    et elle est initialisée avec une case au hazard (base du tronc) */
    struct stack pile;
    pile.size=n*m;
    pile.sp=0;
    struct stack* p_pile=&pile;
    pile.data= (struct cas*) malloc(n*m*sizeof(struct cas));
    srand(time(NULL));
    int i=rand()%n;
    int j=rand()%m;
    laby.mat[i][j].etat=1;  //signifie que la case est traitee
    push(p_pile,laby.mat[i][j]);

    //traitement de toutes les cases
    while (pile.sp!=0) {
        struct cas visit=pile.data[pile.sp-1];
        int acc=0;
        struct cas tab[4];
        int pos_x=visit.x;
        int pos_y=visit.y;
        int k;
        // On modifie les cases adjacente à la case qu'on traite si elles sont dans le labyrinthe et non traités (etat = 0)
        for(k=-1;k<2;k=k+2) {
                    if(pos_x+k>=0 && pos_x+k<n){
                        struct cas case_test=laby.mat[pos_x+k][pos_y];
                        if (case_test.etat==0) {
                            tab[acc]=case_test;
                            acc++;
                        }
                    }
                    if(pos_y+k>=0 && pos_y+k<m) {
                        struct cas case_test=laby.mat[pos_x][pos_y+k];
                        if (case_test.etat==0) {
                            tab[acc]=case_test;
                            acc++;

                         }

                     }
        }
        if(acc==0) {
            pop(p_pile);
/*             Une case peut etre retiré si toutes les cases autours sont traitées. 
            A chaque fois que cette operation est effectuée c'est la fin d'une 
            branche est le début d'une nouvelle. */
            
        }
        else {

            // On casse le chemin entre la case traitée et une case adajacente
            int rng=rand()%acc;
            int pos_x1=tab[rng].x;
            int pos_y1=tab[rng].y;
            int dif_x=pos_x1-pos_x;
            int dif_y=pos_y1-pos_y;
            if(dif_x==1) {
                laby.mat[pos_x][pos_y].E=0;
                laby.mat[pos_x1][pos_y1].O=0;
            }
            if(dif_x==-1) {
                laby.mat[pos_x][pos_y].O=0;
                laby.mat[pos_x1][pos_y1].E=0;
            }
            if(dif_y==1) {
                laby.mat[pos_x][pos_y].S=0;
                laby.mat[pos_x1][pos_y1].N=0;
            }
            if(dif_y==-1) {
                laby.mat[pos_x][pos_y].N=0;
                laby.mat[pos_x1][pos_y1].S=0;
            }
            laby.mat[pos_x1][pos_y1].etat=1;
            push(p_pile,laby.mat[pos_x1][pos_y1]);
        }
    }
return laby;
}

struct cas* search(struct cas case1, struct cas case2,struct labyrinthe laby, int n, int m){
    
    struct stack P;
    P.size = n*m;
    P.sp = 1;
    P.data = (struct cas*) malloc(sizeof(struct cas)*n*m);
    P.data[0] = case1;
    laby.mat[case1.x][case1.y].etat = 0;


    while((P.data[P.sp - 1].x != case2.x || P.data[P.sp - 1].y != case2.y )){
        struct cas visit = P.data[P.sp - 1]; 
        int acc=0;
        int pos_x=visit.x;
        int pos_y=visit.y;
        int k;
        // On modifie les cases adjacente a la case qu'on traite si elles sont dans le labyrinthe et non traités (etat = 0)
    
        for(k=-1;k<2;k=k+2) {
                    if(pos_x+k>=0 && pos_x+k<n && ((k == -1 && laby.mat[pos_x][pos_y].O == 0)
                    || (k == 1 && laby.mat[pos_x][pos_y].E == 0))){
                        struct cas case_test=laby.mat[pos_x+k][pos_y];
                        if (case_test.etat==1) {
                            laby.mat[pos_x+k][pos_y].etat = 0;
                            push(&P, case_test);
                            acc++;
                            break;
                        }
                    }
                    if(pos_y+k>=0 && pos_y+k<m && ((k == -1 && laby.mat[pos_x][pos_y].N == 0)
                    || (k == 1 && laby.mat[pos_x][pos_y].S == 0))) {
                        struct cas case_test=laby.mat[pos_x][pos_y+k];
                        if (case_test.etat==1) {
                            laby.mat[pos_x][pos_y+k].etat = 0;
                            push(&P, case_test);
                            acc++;
                            break;
                         }

                     }
        }
        if(acc==0) {
            pop(&P);
        }
        
    }
    return P.data;
}


int main(int argc, char *argv[])
{
    //D�marrage de SDL
    if (SDL_Init(SDL_INIT_VIDEO) == -1)    //D�marrage de SDL et v�rification du succ�s de l'initialisation.
    {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());     //Ecriture de l'erreur dans le fichcier d'erreur.
        exit(EXIT_FAILURE);     //Sortie du programme.
    }

    //Cr�ation du labyrinthe
    unsigned int longueur = 50, hauteur = 50;
    struct labyrinthe laby = construction(longueur,hauteur);




    //Cr�ation de l'�cran
    unsigned int wc = 800/longueur, hc = 700/hauteur;
    Uint32 w = wc*longueur, h = hc*hauteur;
    SDL_Surface *ecran = NULL;
    ecran = SDL_SetVideoMode(w,h,32,SDL_HWSURFACE );

    //V�rification de la cr�ation
    if (ecran == NULL)
    {
        fprintf(stderr, "Impossible de charger le mode vid�o : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Event event;
    int continuer = 1, i, j;

    SDL_Surface *mur= NULL;
    SDL_Rect position;
    SDL_WM_SetCaption("Labyrinthe",NULL);

    do
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        mur = SDL_CreateRGBSurface(SDL_HWSURFACE, w, 3, 32, 0, 0, 0, 0);
        SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 0, 0));
        position.x = 0;
        position.y = 0;
        SDL_BlitSurface(mur,NULL,ecran, &position);

        position.x = 0;
        position.y = h - 3;
        SDL_BlitSurface(mur,NULL,ecran, &position);

        mur = SDL_CreateRGBSurface(SDL_HWSURFACE, 3, h, 32, 0, 0, 0, 0);
        SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 0, 0));
        position.x = 0;
        position.y = 0;
        SDL_BlitSurface(mur,NULL,ecran, &position);

        position.x = w - 3;
        position.y = 0;
        SDL_BlitSurface(mur,NULL,ecran, &position);


//----------------Affichage des murs----------------------------------------
        for(i=0;i<longueur;i++)
        {
            for(j=0;j<hauteur;j++)
            {
                if(laby.mat[i][j].N==1)
                {
                    mur = SDL_CreateRGBSurface(SDL_HWSURFACE, wc, 1, 32, 0, 0, 0, 0);
                    SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 0, 0));
                    position.x = wc*i;
                    position.y = hc*j;
                    SDL_BlitSurface(mur,NULL,ecran, &position);
                }

                if(laby.mat[i][j].O==1)
                {
                    mur = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, hc, 32, 0, 0, 0, 0);
                    SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 0, 0));
                    position.x = wc*i;
                    position.y = hc*j;
                    SDL_BlitSurface(mur,NULL,ecran, &position);
                }
            }
        }
//------------------------------------------------------------------------------------

//--------------------------Determination du chemin entre case1 et case2--------------------

/* C'est une méthode qui s'inspire du fil d'arianne, 
-On enpile la première case du chemin et on dit qu'elle est visitee
-On repete l'etape suivante: On cherche une case adjacente possible parmis les non-visitees et on l'ajoute à la pile en indiquant qu'elle est visitee. Sinon (pas de case adjacente non visitéé) on depile la case. */

    struct cas case1 = laby.mat[0][0];
    struct cas case2 = laby.mat[longueur-1][hauteur-1];
    struct stack P;
    P.size = longueur*hauteur;
    P.sp = 1;
    P.data = (struct cas*) malloc(sizeof(struct cas)*longueur*hauteur);
    P.data[0] = case1;
    laby.mat[case1.x][case1.y].etat = 0;

    while((P.data[P.sp - 1].x != case2.x || P.data[P.sp - 1].y != case2.y )){ // tant que la derniere case est pas la fin

        mur = SDL_CreateRGBSurface(SDL_HWSURFACE, wc/3, hc/3, 32, 0, 0, 0, 0);
        SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 255, 255));
        position.x = wc*case1.x+wc/2-wc/6;
        position.y = hc*case1.y+hc/2-wc/6;
        SDL_BlitSurface(mur,NULL,ecran, &position);

        mur = SDL_CreateRGBSurface(SDL_HWSURFACE, wc/3, hc/3, 32, 0, 0, 0, 0);
        SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 255, 255, 0));
        position.x = wc*case2.x+wc/2-wc/6;
        position.y = hc*case2.y+hc/2-wc/6;
        SDL_BlitSurface(mur,NULL,ecran, &position);
        
        struct cas visit = P.data[P.sp - 1]; 
        int acc=0;
        int pos_x=visit.x;
        int pos_y=visit.y;
        int k;
        // On modifie les cases adjacente a la case qu'on traite si elles sont dans le labyrinthe et non traités (etat = 0)
    
        for(k=-1;k<2;k=k+2) {
            if(pos_x+k>=0 && pos_x+k<longueur && ((k == -1 && laby.mat[pos_x][pos_y].O == 0)
            || (k == 1 && laby.mat[pos_x][pos_y].E == 0))){
            struct cas case_test=laby.mat[pos_x+k][pos_y];
                if (case_test.etat==1) {
                    laby.mat[pos_x+k][pos_y].etat = 0;
                    push(&P, case_test);
                    mur = SDL_CreateRGBSurface(SDL_HWSURFACE, wc/3, hc/3, 32, 0, 0, 0, 0);
                    SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 255, 0));
                    position.x = wc*P.data[P.sp -1].x+wc/2-wc/6;
                    position.y = hc*P.data[P.sp -1].y+hc/2-wc/6;
                    SDL_BlitSurface(mur,NULL,ecran, &position);

                    acc++;
                    break; //on quitte la boucle car on veut selectionner une seule case
                }
            }
            if(pos_y+k>=0 && pos_y+k<hauteur && ((k == -1 && laby.mat[pos_x][pos_y].N == 0)
            || (k == 1 && laby.mat[pos_x][pos_y].S == 0))) {
                struct cas case_test=laby.mat[pos_x][pos_y+k];
                if (case_test.etat==1) {
                    laby.mat[pos_x][pos_y+k].etat = 0;
                    push(&P, case_test);
                    mur = SDL_CreateRGBSurface(SDL_HWSURFACE, wc/3, hc/3, 32, 0, 0, 0, 0);
                    SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 0, 255, 0));
                    position.x = wc*P.data[P.sp -1].x+wc/2-wc/6;
                    position.y = hc*P.data[P.sp -1].y+hc/2-wc/6;
                    SDL_BlitSurface(mur,NULL,ecran, &position);

                    acc++;
                    break; //on quitte la boucle car on veut selectionner une seule case
                }
            }
        }

        // pas de cases visitees
        if(acc==0) {
            mur = SDL_CreateRGBSurface(SDL_HWSURFACE, wc/3, hc/3, 32, 0, 0, 0, 0);
            SDL_FillRect(mur,NULL,SDL_MapRGB(ecran->format, 255, 0, 0));
            position.x = wc*P.data[P.sp -1].x+wc/2-wc/6;
            position.y = hc*P.data[P.sp -1].y+hc/2-wc/6;
            SDL_BlitSurface(mur,NULL,ecran, &position);
            pop(&P);
        }
        SDL_Flip(ecran);
        SDL_Delay(100);

    }
        



//pour quitter la fenetre

        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT: continuer = 0; break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: continuer = 0; break;
                }
        }
    } while(continuer);



    SDL_Quit();
    return EXIT_SUCCESS;
}


