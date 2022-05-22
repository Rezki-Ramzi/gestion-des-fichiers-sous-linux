#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <regex.h> 
#include <ctype.h> 

int verification_nom_fichier(char * option){
int bit=1;
if(strcmp(option,"-m")!=0 && strcmp(option,"-s")!=0 && strcmp(option,"-d")!=0 && strcmp(option,"-t")!=0 && strcmp(option,"-p")!=0 && strcmp(option,"-a")!=0 && strcmp(option," ")!=0)
   bit=0;
  return bit;
}
int verification_option(char * option){
int bit=0;
if(strcmp(option,"-m")==0 || strcmp(option,"-s")==0 || strcmp(option,"-d")==0 || strcmp(option,"-t")==0 || strcmp(option,"-p")==0 || strcmp(option,"-a")==0)
   bit=1;
  return bit;
}

void affichage_date_consultation_modification(struct stat s) {
printf("----->Date de la consultation du fichier: %s \n",asctime(localtime(&s.st_atime)));
}

void affichage_date_derniere_modification(struct stat s){
printf("----->Date de la dernière modification: %s \n",asctime(localtime(&s.st_mtime)));
}

void affichage_taille_fichier(struct stat s){
printf("---->La taille de fichier : %ld octetes\n",s.st_size);}

void affichage_type_fichier(struct stat s){
switch (s.st_mode & S_IFMT){
 case S_IFREG :
 printf("---->Le type c est un fichier régulier\n") ; break;
 case S_IFDIR :
 printf("---->Le type c est un répertoire\n") ; break;
case S_IFCHR : 
printf("---->Le type c est un spécial caractere \n") ; break;
 case S_IFBLK : 
 printf("---->Le type c est un special bloc \n ") ; break;
case S_IFIFO : 
printf ("----> Le type c est un FIFO\n") ; break;

}
}
//////////////////////////////////////////
void affichage_protection_fichier(struct stat s){
printf("---->la protection  du fichier: %o \n",s.st_mode & 0777);
}

void afficher_tout(struct stat s){
affichage_date_consultation_modification(s);affichage_date_derniere_modification(s);affichage_taille_fichier(s);
affichage_type_fichier(s);affichage_protection_fichier(s);
}

void afficher_res(char *chemin, char *nom,char *argv[],int argc) {

    struct stat buf;int trouve=0;
    char chaine[PATH_MAX];
    strcpy(chaine, chemin);
    strcat(chaine, "/");
    strcat(chaine, nom);
    stat(chaine, &buf);
      printf("------------------le chemin du fichier est: ");
    printf("%s/", chemin);
    printf("%s\n", nom);
     for (int k=1;k<argc-1;k++){
					if(strcmp("-a",argv[k])==0 && verification_option(argv[k]))  {
					trouve=1;
					afficher_tout(buf);break;
					}
					
						
					  
					 }
					 if(trouve==0){  
					 for (int k=1;k<argc-1;k++){
						if (strcmp("-d",argv[k])==0 && verification_option(argv[k]) )   affichage_date_derniere_modification(buf);  
						if (strcmp("-m",argv[k])==0 && verification_option(argv[k]))   affichage_date_consultation_modification(buf); 
						if(strcmp("-s",argv[k])==0 && verification_option(argv[k]))    affichage_taille_fichier(buf);
						if(strcmp("-p",argv[k])==0 && verification_option(argv[k]))    affichage_protection_fichier(buf);
						if(strcmp("-t",argv[k])==0 && verification_option(argv[k]))    affichage_type_fichier(buf);
						}
						}
    
}

void rechercher_dans_rep(char *chemin, char *nom, int niveau,int argc,char*argv[])
{
     
    if (niveau >= 0)
    {
        struct dirent *entree;
        DIR *d = opendir(chemin);
        char chaine[1000]; // pour traiter les chemins de grande taille 
       regex_t regex;int return_value;
     return_value = regcomp(&regex,nom, REG_NOSUB | REG_EXTENDED);
     
        while (1)
        {  
            entree = readdir(d);
            if (entree == NULL)
                break;
            if (regexec(&regex,entree->d_name,0, NULL, 0)==0 && strcmp(entree->d_name, ".") && strcmp(entree->d_name,"..")) 
            {
                afficher_res(chemin, entree->d_name,argv,argc);
                 
            }

            if (entree->d_type == DT_DIR && strcmp(entree->d_name, ".") && strcmp(entree->d_name,".."))
            {
                strcpy(chaine,chemin);
                 if (strcmp(chemin, "/")) // pour ne pas afficher le / 2 fois
                    strcat(chaine, "/");
                strcat(chaine,entree->d_name);
                rechercher_dans_rep(chaine, nom, niveau-1,argc,argv);
            }   
        }
    }
}

void rechercher_dans_rep_sans_niveau(char *chemin, char *nom,int argc,char*argv[])
{
        struct dirent *entree;
        DIR *d = opendir(chemin);
        char chaine[1000]; // pour traiter les chemins de grande taille 
       regex_t regex;int return_value;
     return_value = regcomp(&regex,nom, REG_NOSUB | REG_EXTENDED);
     
        while (1)
        {  
            entree = readdir(d);
            if (entree == NULL)
                break;
            if (regexec(&regex,entree->d_name,0, NULL, 0)==0 && strcmp(entree->d_name, ".") && strcmp(entree->d_name,"..")) 
            {
                afficher_res(chemin, entree->d_name,argv,argc);
                 
            }

            if (entree->d_type == DT_DIR && strcmp(entree->d_name, ".") && strcmp(entree->d_name,".."))
            {
                strcpy(chaine,chemin);
                 if (strcmp(chemin, "/")) // pour ne pas afficher le / 2 fois
                    strcat(chaine, "/");
                strcat(chaine,entree->d_name);
                rechercher_dans_rep_sans_niveau(chaine,nom,argc,argv);
            }   
        }
    
}

////////////////////////
int main(int argc, char *argv[]){
 DIR *repertoire;
   char nom_fichier[30];
   char rep[30];
   char exp[300]; int test_niv=0;
        char c[3]; strcpy(exp,"^");
        char niveau[20];
    
       
      
   
           if(((strcmp(argv[1]," ")!=0 && strcmp(argv[2],"-n")==0) || (strcmp(argv[1],"-n")==0))&& verification_nom_fichier(argv[argc-1])==0) {
           
                if(strcmp(argv[2],"-n")==0) { 
         
         if((argc-1)>3 && !verification_option(argv[3]))
         strcpy(niveau,argv[3]);
         else 
         test_niv=1;
         }
         else{ 
         if(strcmp(argv[1],"-n")==0) { 
         
          if((argc-1)!=3 && !verification_option(argv[2]))
         strcpy(niveau,argv[2]);
         else   
          test_niv=1;
         }
         
         }
             //printf("le test niv=%d \n",test_niv);
            strcpy(nom_fichier,argv[argc-1]);
            printf("le nom de fichier %s \n",nom_fichier);
            if(strcmp(argv[2],"-n")==0) { strcpy(rep,argv[1]);}
            if(strcmp(argv[1],"-n")==0) { strcpy(rep,".");}
            repertoire = opendir(rep);
           printf("le nom de rep: %s \n",rep);
             for (int i = 0; i < strlen(nom_fichier); i++)
    {
        switch (nom_fichier[i])
        {
            case '*':
                strcat(exp, "[[:print:]]*");
                break;
            case '?':
                strcat(exp, "[[:print:]]");
                break;
            case '+':
                strcat(exp, "\\");
                strcat(exp, "+");
                break;
            case '(':
                strcat(exp, "\\");
                strcat(exp, "(");
                break;
            case ')':
                strcat(exp, "\\");
                strcat(exp, ")");
                break;
            case '[':
                strcat(exp, "\\");
                strcat(exp, "[");
                break;
            case ']':
                strcat(exp, "\\");
                strcat(exp, "]");
                break;
            case '.':
                strcat(exp, "\\");
                strcat(exp, ".");
                break;
            case '^':
                strcat(exp, "\\");
                strcat(exp, "^");
                break;
            case '|':
                strcat(exp, "\\");
                strcat(exp, "|");
                break;
            case '$':
                strcat(exp, "\\");
                strcat(exp, "$");
                break;
            case '\\':
                strcat(exp, "\\");
                strcat(exp, "\\");
                break;
            default:
                c[0] = nom_fichier[i];
                c[1] = '\0';
                strcat(exp, c);
                break;
        }
    }
        strcat(exp,"$");
           printf("l\'expresssion reguliere est :%s\n",exp);
       if(test_niv==0)
            rechercher_dans_rep(rep,exp,atoi(niveau),argc,argv); // atoi : convert une chaine à un entier
      else 
            rechercher_dans_rep_sans_niveau(rep,exp,argc,argv);
            
             
           }
           else
           {
           printf("la commande est non valide !!,Merci de suivre le format : ./search -r [repertoire] -n [niveau] [options] nom_fichier\n");
           }
          
        

return 0;
				
}

/*
gcc -o search.o -c TP.c
ramzi@ramzi-VirtualBox:~/Bureau$ gcc -o search search.o
*/

