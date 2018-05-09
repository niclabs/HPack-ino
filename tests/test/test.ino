#line 2 "test.ino"
#include <ArduinoUnit.h>
#include "HPack.h"

test(ok){
  assertNotEqual(2,3);//,F("notequal!!"));
}

void setup(){
  Serial.begin(9600);
   while(!Serial) {}
   //Serial.println("Start!");
}


void loop()
{
  Test::run();
};
