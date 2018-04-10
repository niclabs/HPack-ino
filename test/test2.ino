#line 2 "test2.ino"
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

test(EncodeDecodeStringNoHuffman){
  int initial_memory = freeMemory();
  char* string;
  bool huffman;

  string = "Holakasjhflaskjhdflsadkjfhlasdjhasldfhajklsdhflajshlasjhdflsadjfhañlsdhflasdjfhlahfdlasfflajsdfhlajsdfh!\0";
  huffman = false;
  byte * encoded_string = HPackCodec::encodeString(string,huffman);

  //Serial.println((char*)encoded_string);
  //Serial.println(strlen((char*)encoded_string));
  char* string_decoded = HPackCodec::decodeString(encoded_string, huffman, strlen((char*)encoded_string),0);

  //Serial.println(strlen(string));
  //Serial.println(strlen(string_decoded));
  //Serial.println(string_decoded[5],BIN);
  
  assertEqual(string, string_decoded);
  delete[](encoded_string);
  delete[](string_decoded);
  assertEqual(freeMemory(),initial_memory);     
}

test(EncodeDecodeStringHuffman){
  int initial_memory = freeMemory();
  char* string;
  bool huffman;

  string = "Holaasdasdaksjdhaskjdhkjahdkjasd!|1\0";
  huffman = true;
  byte * encoded_string = HPackCodec::encodeString(string,huffman);

  //Serial.println((char*)encoded_string);
  //Serial.println(strlen((char*)encoded_string));
  char* string_decoded = HPackCodec::decodeString(encoded_string, huffman, strlen((char*)encoded_string), 0);

  //Serial.println(strlen(string));
  //Serial.println(strlen(string_decoded));
  //Serial.println(string_decoded[5],BIN);
  
  assertEqual(string, string_decoded);
  delete[](encoded_string);
  delete[](string_decoded);
  assertEqual(freeMemory(),initial_memory);     
}


test(LiteralHeaderFieldNeverIndexedWithoutIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithoutIndexHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  hp = hb->literalNeverIndexed(true, name_string, true,value_string);
  int size_added = hb->addData(hp);
  delete(hp);
  
  hpd = hb->getNext();
  HeaderPair* hp1 = hb->getHeaderPair(hpd);
  delete(hpd);
  assertEqual(name_string,hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  delete(hpack);
  assertEqual(freeMemory(),initial_memory);     
}

test(LiteralHeaderFieldNeverIndexedWithoutIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithoutIndexNoHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;
  
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";

  
  hp = hb->literalNeverIndexed(false, name_string, false,value_string);
  int size_added = hb->addData(hp);
  delete(hp);
  
  hpd = hb->getNext();
  HeaderPair* hp1 = hb->getHeaderPair(hpd);
  delete(hpd);
  assertEqual(name_string,hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  delete(hpack);
  assertEqual(freeMemory(),initial_memory);    
}


test(LiteralHeaderFieldNeverIndexedWithIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithIndexHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;
  char * value_string = (char*)"value1234567890\0";
  hp = hb->literalNeverIndexed(1, true,value_string);
  int size_added = hb->addData(hp);
  delete(hp);
  hpd = hb->getNext();
  HeaderPair* hp1 = hb->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0",hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  
  delete(hpack);
  assertEqual(freeMemory(),initial_memory);    
  
  
}


test(LiteralHeaderFieldNeverIndexedWithIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithIndexNOHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;
  char * value_string = (char*)"value1234567890\0";
  hp = hb->literalNeverIndexed(1, false,value_string);
  int size_added = hb->addData(hp);
  delete(hp);
  hpd = hb->getNext();
  HeaderPair* hp1 = hb->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0",hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  
  delete(hpack);
  assertEqual(freeMemory(),initial_memory);      
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
