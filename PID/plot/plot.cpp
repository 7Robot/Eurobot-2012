#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rs232.h"
#include <iostream>
#include <time.h>

using namespace std;

#define buffSize 64
#define arduino 15
#define pic 16
#define nMes 100


int mesure(FILE *,int tsec,int pps);
FILE * creerFichier(FILE *);
float getFloat(unsigned char * buff);


int main()
{
	FILE * data = NULL;
	
	/* Création du fichier de données. */
	data = creerFichier(data);
	
	mesure(data,10,1);
	
	
	fclose(data);
	return 0;
}


FILE * creerFichier(FILE * data)
{
	FILE * temp;
	time_t rawtime; /* Pour le nom du fichier */
    struct tm instant;
    char filename[42];
	char datestr[25];
    
    time(&rawtime);
	instant = *localtime(&rawtime);
	
    strcpy(filename,"\0");
	strcpy(datestr,"\0");
   
    sprintf(datestr,"%d-%d-%d@%dh%dm%ds",instant.tm_mday,instant.tm_mon+1,instant.tm_year+1900,instant.tm_hour,instant.tm_min,instant.tm_sec);
	strncat(filename,"./mesures/",16);
	strncat(filename,"aquisition",16);
    strncat(filename,datestr,20);
    strncat(filename,".gp",4);
    
    printf ( "Fichier de données : %s \n" , filename);
	temp = fopen(filename,"w");
	
	fprintf(temp,"#%s \n",filename);
    fprintf(temp,"# Fichier de données à tracer avec GNUplot \n#Commentaires : \n#\n#\n\n\n");
    
    return temp;
}


int mesure(FILE * data,int tsec,int pps)
{
	unsigned char buff[buffSize] ; /* PollComport impose unsigned. */
	unsigned char * pBuff = buff;
	int nbytes=0;
	

	/* Ouverture du port série. */
	if(OpenComport(pic,9600)){printf("Erreur d'ouverture du port. \n"); return -1;}
	else printf("Port serie ouvert.\n");
	PollComport(pic, buff, buffSize); /* On vide le buffer en prévention. */
	
	SendByte(pic, 'e'); /* Echelon de test. */
	usleep(1000); /* Temps de l'envoi. */
	
	for(int t=1;t<=tsec*pps;t++)
	{
		pBuff = buff; /* Pointeur en début de chaine. */
		SendByte(pic, 'v'); /* Demande information. */
		
		usleep(1000000/pps); /* Pause en µs. */
		
		nbytes = PollComport(pic, buff, buffSize); /* nbytes ont étés récupérés. */
		buff[nbytes]='\0'; /* On met le caractère de fin de chaine. */
		
		if(!nbytes) printf("Warning : buffer vide\n");
		
		else 
		{
		    printf("%d bytes : %s\n",nbytes,buff);
			fprintf(data, "%d   %s\n",t*1000/pps,buff);
		}
	}
	fprintf(data, "\n#### Fin du fichier ####\n");

	return 0;
}

float getFloat(unsigned char * buff)
{
	/* Convertir 4 bytes en un float */
	float * pf;
	pf = (float*) buff;
    return *pf;
}
