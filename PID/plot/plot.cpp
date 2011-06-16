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


int main()
{
	FILE * data = NULL;
	
	/* Création du fichier de données. */
	data = creerFichier(data);
	
	mesure(data,1,100);
	
	
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
	if(OpenComport(pic,57600)){printf("Erreur d'ouverture du port. \n"); return -1;}
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

/*


int mesures(int pitch,int nb)
{
	int l=0,i=0;
	unsigned char buff[buffSize];
	unsigned char * p= &buff[0];
	
	
	FILE * mesures;
	FILE * plot;
	FILE * gp=popen("gnuplot","w");
	
	
    
	
	
	

    OpenComport(arduino,9600); // sudo ln -sf ttyACM0 ttyS15 ** ou changer ds rs232.c
	PollComport(arduino, buff, buffSize);
	SendByte(arduino, 'Z'); // RAZ
	
	printf("port serie ouvert \n");


	
	fprintf(plot,"# Fichier de tracé des données contenues dans mesures.gp \n \n \n");
	fprintf(plot,"set title 'Courbes' \n");
	fprintf(plot,"set xlabel 'Temps (secondes)' \n");
	fprintf(plot,"set ylabel 'Tension (V)' \n");
	fprintf(plot,"set y2label 'Courant (A)' \n");
	fprintf(plot,"set y2tics 1 \n");
	fprintf(plot,"set autoscale \n");
	fprintf(plot,"Kt = 1.0/10 \nK0 = 1.0/28.24 \nK1= 1.0/28.24 \nK2 = 5.0/(1023*0.18)\nK3 = 1/20.0 \nK4 = 5.0/1023 \nK5 = 5.0/50 \n");
	fprintf(plot,"plot './%s' using (Kt*($1+$8)/2):($2*K0) axis x1y1 with lp title 'Vbat' ,\\\n",filename);
	fprintf(plot,"     './%s' using (Kt*($1+$8)/2):(($3-$2)*K0/0.18) axis x1y2 with lp title 'Ibat' ,\\\n",filename);
	fprintf(plot,"     './%s' using (Kt*($1+$8)/2):($4*K2) axis x1y2 with lp title 'Iexitation' ,\\\n",filename);
	fprintf(plot,"     './%s' using (Kt*($1+$8)/2):(($5-$2)*K1) axis x1y1 with lp title 'Vdiode' ,\\\n",filename);
	fprintf(plot,"     './%s' using (Kt*($1+$8)/2):($7*K3) axis x1y1 with lp title 'Vitesse'",filename);
     

     
	
	fclose(plot);
	
	
	mesures = fopen(filename,"w");
	
	fprintf(mesures,"#Mesures effectuees le %s \n", datestr);
	fprintf(mesures,"#Commentaires : \n#\n#\n#\n#\n#\n#\n\n\n\n");
	fprintf(mesures,"#ti       Voie0   Voie 1    Voie 2   Voie 3   Voie 4   Voie 6   tf\n");
	
	
	while(nb >= 0)
	{
		usleep(1000*pitch);
		i = PollComport(arduino, buff, buffSize);
		if(i>=10)
		{
			p = &buff[0];
			while(*p != '[') p++; // *p vaut [
			p++;
			l=0;
			while(*p != ']')
			{
				fprintf(mesures,"%c",*p);
				printf("%c",*p);
				l++;
				p++;
				if(*p == ' ')
				{
					while(l < 9)
					{
						fprintf(mesures," ");
						printf(" ");
						l++;
					}
					l=0;
				}
			}
			fprintf(mesures,"\n");
			printf("\n");
			
					}
	nb--;
	SendByte(arduino, 'l');
	}
	
	fclose(mesures);
	CloseComport(arduino);
	

	return 0;
}*/





