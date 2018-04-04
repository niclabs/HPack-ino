#line 2 "test.ino"
#include <ArduinoUnit.h>
#include "HPack.h"


test(EncodeDecodeInteger){
  int initial_memory = freeMemory();
  uint32_t integers[] = {0,1,5,30,31,32,127,128,129,130,131,132,133,134,135,18000}; 
  int sints = (sizeof(integers)/sizeof(uint32_t));
  for(uint8_t prefix = 1; prefix < 8; prefix++){
      for(int i = 0; i < sints; i++){
        //Serial.print(F("integer:"));
        //Serial.println(integers[i]);
        //Serial.print(F("prefix:"));
        //Serial.println(prefix);
        
        byte * encoded_integer = HPackCodec::encodeInteger(integers[i],prefix);
        uint32_t octets_length = getOctetsLength(integers[i],prefix);
        //Serial.print(F("encoded length: "));
        //Serial.println(octets_length);
        //for(int k =0; k<octets_length; k++){
          //Serial.println(encoded_integer[k],BIN);
        //}
        
        uint32_t decoded = HPackCodec::decodeInteger(encoded_integer,prefix, octets_length,0);  
        delete[](encoded_integer);       
        //Serial.print(F("decoded:"));
        //Serial.println(decoded);
        assertEqual(freeMemory(),initial_memory);     
      }
   }
}

test(EncodeDecodeString){
  
}

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
}