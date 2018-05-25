#line 2 "test_lit_without.ino"
#include <ArduinoUnit.h>
#include "HPack.h"

test(LiteralHeaderFieldWithoutIndexingWithIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldWithoutIndexingWithIndexHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
 HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalWithoutIndexing(1, true,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0",hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  
 delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);    
  
  
}

test(LiteralHeaderFieldWithoutIndexingWithIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldWithoutIndexingWithIndexNOHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalWithoutIndexing(1, false,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0",hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


test(LiteralHeaderFieldWithoutIndexingWithoutIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldWithoutIndexingWithoutIndexHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalWithoutIndexing(true, name_string, true,value_string);
 EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual(name_string,hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);     
}


test(LiteralHeaderFieldWithoutIndexingWithoutIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldWithoutIndexingWithoutIndexNoHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";

  
  hp = hbs->literalWithoutIndexing(false, name_string, false,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual(name_string,hp1->name);
  assertEqual(value_string,hp1->value);
  delete(hp1);
  delete(hpack_sender);delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);    
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
