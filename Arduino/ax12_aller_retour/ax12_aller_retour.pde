#include <ax12.h>

AX12 moteur(3); // Adapter l'adresse.

void setup()
{
   AX12::init(1000000); // Vitesse réglée en usine, 1Mb/s.
   
   moteur.writeInfo(TORQUE_ENABLE, 1);
   moteur.writeInfo(MAX_TORQUE, 500);

   //moteur.setEndlessTurnMode(true);
   //moteur.endlessTurn(500);
}

int t = 0; // Temps.
int dir = 1; // Direction du déplacement.

void loop()
{
   t += dir;
   
   if(t <= -90 || t >= 90)
     dir = -dir;
   
   angle(moteur, t);

   delay(10);
}

byte angle(AX12 ax, int a)
{
   return ax.writeInfo(GOAL_POSITION, map(a, -150, 150, 0, 1023));
}
