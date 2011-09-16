// dinamyxel ax-12 servo test for Arduino 0017

//#include <SoftwareSerial.h>
#include <ax12.h>

//SoftwareSerial swSerial (2, 3);

AX12 motor[2] = {AX12 (), AX12 ()};                 // define 2 motores AX12


void setup()   {                
  
  pinMode(13, OUTPUT);
  Serial.begin (57600);                             // inicializa el SoftSerial a 115,2 Kb/s
  
  AX12::init (1000000);                             // inicializa los AX12 a 1 Mb/s
  //AX12::init (1000000);                             // inicializa los AX12 a 1 Mb/s
  int detect[2];                                    // array para detectar automáticamente las ID de 2 motores
  AX12::autoDetect (detect, 2);                     // detección de IDs
  for (byte i=0; i<2; i++) {
    motor[i].id = detect[i];                        // asigna las ID detectadas a los motores definidos previamente
  }
  motor[0].inverse = true;                          // asigna la propiedad "inverse" a uno de los motores
  motor_init ();  
  
}

void loop()   {
  
  int a = motor[0].writeInfo (MOVING_SPEED, random(1024));
  
  Serial.print ("internal error ");
  Serial.println(a, DEC);
  Serial.print ("AX12 error ");
  Serial.println(motor[0].status_error, DEC);
  
  a = motor[1].writeInfo (MOVING_SPEED, random(1024));
  
  Serial.print ("internal error ");
  Serial.println(a, DEC);
  Serial.print ("AX12 error ");
  Serial.println(motor[0].status_error, DEC);
  
  //movimiento (2, 500);  

  delay (500);
}


void motor_init () {
  
  for (int i=0; i<2; i++) {
    motor[i].writeInfo (TORQUE_ENABLE, 1);               // habilita el torque
    motor[i].setEndlessTurnMode(true);                   // lo pone en modo de rotación continua
   // setear la inercia y todo eso
    motor[i].writeInfo (CW_COMPLIANCE_MARGIN, 0);
    motor[i].writeInfo (CCW_COMPLIANCE_MARGIN, 0);
    motor[i].writeInfo (CW_COMPLIANCE_SLOPE, 95);
    motor[i].writeInfo (CCW_COMPLIANCE_SLOPE, 95);
    motor[i].writeInfo (PUNCH, 150);
    motor[i].writeInfo (MAX_TORQUE, 1023);
    motor[i].writeInfo (LIMIT_TEMPERATURE, 85);
    motor[i].writeInfo (DOWN_LIMIT_VOLTAGE, 60);
    motor[i].writeInfo (DOWN_LIMIT_VOLTAGE, 190);
    motor[i].writeInfo (RETURN_DELAY_TIME, 150);
   // test motor
    motor[i].endlessTurn (500); delay (500); 
    motor[i].endlessTurn (-500); delay (500);
    motor[i].endlessTurn (0);
  }
  
}

void movimiento (byte direccion, int velocidad) {
  motor[0].endlessTurn (-velocidad*bin2sign(direccion&1));
  motor[1].endlessTurn (-velocidad*bin2sign(direccion>>1));      
} 
