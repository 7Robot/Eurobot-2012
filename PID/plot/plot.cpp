#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rs232.h"
#include <iostream>
#include <time.h>

using namespace std;

#define buffSize 64
#define arduino 15
#define nMes 100


int mesures(int pitch,int nb);


int main()
{
	int nmes;
	int pitch;
	cout << "Pitch ? (mesures par secondes)" ;
	cin >> pitch;
	pitch = 1000/pitch;
	
	cout << "Temps d'aquisition ? (secondes)";
	cin >> nmes ;
	nmes =  (nmes*1000)/pitch;
	
	mesures(pitch,nmes);
		
    return 0;
}

int mesures(int pitch,int nb)
{
	int l=0,i=0;
	unsigned char buff[buffSize];
	unsigned char * p= &buff[0];
	char filename[42];
	char filenameplot[45];
	char datestr[25];
	
	FILE * mesures;
	FILE * plot;
	FILE * gp=popen("gnuplot","w");
	
	time_t rawtime;
    struct tm instant;
	

    OpenComport(arduino,9600); // sudo ln -sf ttyACM0 ttyS15 ** ou changer ds rs232.c
	PollComport(arduino, buff, buffSize);
	SendByte(arduino, 'Z'); // RAZ
	
	printf("port serie ouvert \n");

	time(&rawtime);
	instant = *localtime(&rawtime);
    
	strcpy(filename,"\0");
	strcpy(datestr,"\0");
	strcpy(filenameplot,"\0");
   
    sprintf(datestr,"%d-%d-%d@%dh%dm%ds",instant.tm_mday,instant.tm_mon+1,instant.tm_year+1900,instant.tm_hour,instant.tm_min,instant.tm_sec);
	strncat(filename,"eole",16);
    strncat(filename,datestr,20);
    strcpy(filenameplot, filename);
    strncat(filename,".gp",4);
    strncat(filenameplot,".plot",6);
    
    printf ( "Fichier de données : %s \n" , filename);
	printf ( "Fichier GNUplot : %s \n" , filenameplot);
	
	plot = fopen(filenameplot,"w"); // ecriture du fichier de tracage
	
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
}





