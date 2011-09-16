/*
  ax12_blink_id.pde - Utility to visualize dynamixel IDs with status LEDs.
  Created by Martin d'Allens, September 6, 2011.
  Released into the public domain.
*/

#include <ax12.h>

#define ARDUINO_LED 13
#define BLINK_TIME  200 // ms

void setup()
{
   AX12::init(1000000); // Factory default of 1Mb/s.
   pinMode(ARDUINO_LED, OUTPUT);
}

void loop()
{
   AX12 ax(0); // We will use only one instance for all servos.
   
   boolean id_zero_active = (ax.writeInfo(AX_LED, 0) == 0); // Turn off the LED for ID 0 and check the success. Variable unused.
   
   for(int pulse = 1; pulse <= 253; pulse++)
   {
      boolean far_enough = true;
      int start_millis = millis();
      
      digitalWrite(ARDUINO_LED, HIGH);
      for(int id = pulse; id <= 253; id++) // Try all IDs that should blink for this pulse.
      {
         ax.id = id;
         if(ax.writeInfo(AX_LED, 1) == 0) // We found an active ID.
         {
            if(id > pulse) // This ID justifies another pulse.
               far_enough = false; 
         }
      }
      int elapsed_millis = millis() - start_millis;
      if(elapsed_millis < BLINK_TIME)
         delay(BLINK_TIME - elapsed_millis);
      
      digitalWrite(ARDUINO_LED, LOW);
      ax.id = 254; // Broadcast ID, to turn all LEDs off.
      ax.writeInfo(AX_LED, 0);
      delay(BLINK_TIME);
      
      if(far_enough)
         break;
   }
   
   delay(750);
}
