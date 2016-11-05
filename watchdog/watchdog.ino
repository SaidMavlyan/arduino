#include <avr/wdt.h>

void setup ()
  {
  Serial.begin (9600);
  Serial.println();
  Serial.println(F("Code starting ..."));
  }  // end of setup

void loop()
{
//   wdt_reset();// make sure this gets called at least once every 8 seconds!
  wdt_enable(WDTO_8S);
  while(1){}
}
