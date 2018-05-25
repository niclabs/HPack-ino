#line 2 "test_indexed_header.ino"
#include <ArduinoUnit.h>
#include "HPack.h"

test(IndexedHeaderFieldStatic){
  Serial.println(F("Test IndexedHeaderFieldNoHuffmanStatic"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
 HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  //char * name_string = (char*)"holasd\0";
  //char * value_string = (char*)"value1234567890\0";
  hp = hbs->indexedHeaderField(1);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0", hp1->name);
  assertEqual((char*)"", hp1->value);
  delete(hp1);
  
  delete(hpack_sender);delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


test(IndexedHeaderFieldNoHuffmanDynamic){
  Serial.println(F("Test IndexedHeaderFieldNoHuffmanDynamic"));
  int initial_memory = freeMemory();
  HPackData *hp1, *hp2, *hpd1, *hpd2; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";

  hp1 = hbs->literalHeaderFieldWithIncrementalIndex(false, name_string, false,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  
  hp2 = hbs->indexedHeaderField(62);
  EncodedData* ed2 = hbs->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  hbr->receiveHeaderBlock(ed2->encoded_data, ed2->length);
  delete[](ed2->encoded_data);
  delete(ed2);
    
  
  hpd1 = hbr->getNext();
  delete(hpd1);
  hpd2 = hbr->getNext();
  HeaderPair* hpair2 = hbr->getHeaderPair(hpd2); 
  delete(hpd2);
  assertEqual(name_string, hpair2->name);
  assertEqual(value_string, hpair2->value);
  delete(hpair2);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


test(IndexedHeaderFieldHuffmanDynamic){
  Serial.println(F("Test IndexedHeaderFieldNoHuffmanDynamic"));
  int initial_memory = freeMemory();
  HPackData *hp1, *hp2, *hpd1, *hpd2; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";

  hp1 = hbs->literalHeaderFieldWithIncrementalIndex(true, name_string, true,value_string);
  
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  
  
  hp2 = hbs->indexedHeaderField(62);

  EncodedData* ed2 = hbs->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  hbr->receiveHeaderBlock(ed2->encoded_data, ed2->length);
  delete[](ed2->encoded_data);
  delete(ed2);

  
  hpd1 = hbr->getNext();
  delete(hpd1);
  hpd2 = hbr->getNext();
  HeaderPair* hpair = hbr->getHeaderPair(hpd2);
  delete(hpd2);
  assertEqual(name_string, hpair->name);
  assertEqual(value_string, hpair->value);
  delete(hpair);
  
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
