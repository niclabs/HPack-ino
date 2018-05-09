#line 2 "test_dyn_update.ino"
#include <ArduinoUnit.h>
#include "HPack.h"

test(DynamicTableSizeUpdateDecrease){
  Serial.println(F("Test DynamicTableSizeUpdateDecrease"));
  int initial_memory = freeMemory();
  HPackData *hp1, *hp2, *hp3, *hpd1, *hpd2, *hpd3; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  
  hp2 = hbs->literalHeaderFieldWithIncrementalIndex(true, name_string, true,value_string);

  EncodedData* ed2 = hbs->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  hbr->receiveHeaderBlock(ed2->encoded_data, ed2->length);
  delete[](ed2->encoded_data);
  delete(ed2);

  uint32_t new_size = 55;
  hp1 = hbs->dynamicTableSizeUpdate(new_size);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);

  hp3 = hbs->indexedHeaderField(62);

  EncodedData* ed3 = hbs->createHeaderBlock(&(HPackData*){hp3},1);
  delete(hp3);
  hbr->receiveHeaderBlock(ed3->encoded_data, ed3->length);
  delete[](ed3->encoded_data);
  delete(ed3);
  
  hpd2 = hbr->getNext();
  delete(hpd2);
  hpd1 = hbr->getNext();
  delete(hpd1);
  hpd3 = hbr->getNext(); 
  HeaderPair* hpair = hbr->getHeaderPair(hpd3);
  delete(hpd3);
  
  assertEqual(name_string, hpair->name);
  assertEqual(value_string, hpair->value);
  delete(hpair);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      

}


test(DynamicTableSizeUpdateIncrease){
  Serial.println(F("Test DynamicTableSizeUpdateIncrease"));
  int initial_memory = freeMemory();
  HPackData *hp1, *hp2, *hp3, *hpd1, *hpd2, *hpd3; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 55;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  
  hp2 = hbs->literalHeaderFieldWithIncrementalIndex(true, name_string, true,value_string);

  EncodedData* ed2 = hbs->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  hbr->receiveHeaderBlock(ed2->encoded_data, ed2->length);
  delete[](ed2->encoded_data);
  delete(ed2);

  uint32_t new_size = 100;
  hp1 = hbs->dynamicTableSizeUpdate(new_size);

  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);

  hp3 = hbs->indexedHeaderField(62);
  
  EncodedData* ed3 = hbs->createHeaderBlock(&(HPackData*){hp3},1);
  delete(hp3);
  hbr->receiveHeaderBlock(ed3->encoded_data, ed3->length);
  delete[](ed3->encoded_data);
  delete(ed3);
  
  
  hpd2 = hbr->getNext();
  delete(hpd2);
  hpd1 = hbr->getNext();
  delete(hpd1);
  hpd3 = hbr->getNext(); 
  HeaderPair* hpair = hbr->getHeaderPair(hpd3);
  delete(hpd3);
  
  assertEqual(name_string, hpair->name);
  assertEqual(value_string, hpair->value);
  delete(hpair);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      

}


test(DynamicTableSizeUpdateDecreaseDeletingEntry){
  Serial.println(F("Test DynamicTableSizeUpdateDecreaseDeletingEntry"));
  int initial_memory = freeMemory();
  HPackData *hp1, *hp2, *hp3, *hpd1, *hpd2, *hpd3; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  
  hp2 = hbs->literalHeaderFieldWithIncrementalIndex(true, name_string, true,value_string);
  EncodedData* ed2 = hbs->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  hbr->receiveHeaderBlock(ed2->encoded_data, ed2->length);
  delete[](ed2->encoded_data);
  delete(ed2);

  uint32_t new_size = 32;
  hp1 = hbs->dynamicTableSizeUpdate(new_size);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);

  //hp3 = hb->indexedHeaderField(62);
  //size_added = hb->addData(hp3);
  //delete(hp3);
  
  hpd2 = hbr->getNext();
  delete(hpd2);
  hpd1 = hbr->getNext();
  delete(hpd1);
  //hpd3 = hb->getNext(); 
  //HeaderPair* hpair = hb->getHeaderPair(hpd3);
  //delete(hpd3);
  
  //assertEqual(name_string, hpair->name);
  //assertEqual(value_string, hpair->value);
  //delete(hpair);
  

  assertEqual(hbs->dyn_table->length(),0);
  assertEqual(hbr->dyn_table->length(),0);
  
  delete(hpack_sender);delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      

}


test(DynamicTableSizeUpdateOverflow){
  Serial.println(F("Test DynamicTableSizeUpdateOverflow"));
  int initial_memory = freeMemory();
  HPackData *hp1, *hp2, *hp3, *hpd1, *hpd2, *hpd3; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 114;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  
  char * name1_string = (char*)"this is a name name name 123!!1928309#$$%%&/(/()?\0";
  char * value1_string = (char*)"valueeeeeeeeevalue123456789asd0!!\0";
  
  hp1 = hbs->literalHeaderFieldWithIncrementalIndex(true, name1_string, true,value1_string);
  
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);

  Serial.println(strlen(name1_string));
  Serial.println(strlen(value1_string));
  assertEqual(hbs->dyn_table->length(),1);
  assertEqual(hbs->dyn_table->size(),strlen(name1_string)+strlen(value1_string)+32);

  hpd1 = hbr->getNext();

  assertEqual(hbr->dyn_table->length(),1);
  assertEqual(hbr->dyn_table->size(),strlen(name1_string)+strlen(value1_string)+32);
  
  HeaderPair* hpair1 = hbr->getHeaderPair(hpd1);
  delete(hpd1);
  assertEqual(name1_string, hpair1->name);
  assertEqual(value1_string, hpair1->value);
  delete(hpair1);
  
  char * name2_string = (char*)"holasd\0";
  char * value2_string = (char*)"value1234567890\0";
  
  hp2 = hbs->literalHeaderFieldWithIncrementalIndex(true, name2_string, true,value2_string);

  EncodedData* ed2 = hbs->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  hbr->receiveHeaderBlock(ed2->encoded_data, ed2->length);
  delete[](ed2->encoded_data);
  delete(ed2);

  assertEqual(hbs->dyn_table->length(),1);
  assertEqual(hbs->dyn_table->size(),strlen(name2_string)+strlen(value2_string)+32);

  hpd2 = hbr->getNext();

  assertEqual(hbr->dyn_table->length(),1);
  assertEqual(hbr->dyn_table->size(),strlen(name2_string)+strlen(value2_string)+32);


  HeaderPair* hpair2 = hbr->getHeaderPair(hpd2);
  delete(hpd2);
  assertEqual(name2_string, hpair2->name);
  assertEqual(value2_string, hpair2->value);
  delete(hpair2);
 
  char * name3_string = (char*)"name2tooverloadData\0";
  char * value3_string = (char*)"value2overflowtable...!!1234567890\0";
  
  hp3 = hbs->literalHeaderFieldWithIncrementalIndex(true, name3_string, true,value3_string);
  
  EncodedData* ed3 = hbs->createHeaderBlock(&(HPackData*){hp3},1);
  delete(hp3);
  hbr->receiveHeaderBlock(ed3->encoded_data, ed3->length);
  delete[](ed3->encoded_data);
  delete(ed3);

  assertEqual(hbs->dyn_table->length(),1);
  assertEqual(hbs->dyn_table->size(),strlen(name3_string)+strlen(value3_string)+32);
  
  hpd3 = hbr->getNext();

  assertEqual(hbr->dyn_table->length(),1);
  assertEqual(hbr->dyn_table->size(),strlen(name3_string)+strlen(value3_string)+32);
  
  HeaderPair* hpair3 = hbr->getHeaderPair(hpd3);
  delete(hpd3);
  assertEqual(name3_string, hpair3->name);
  assertEqual(value3_string, hpair3->value);
  delete(hpair3);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      

}



/*test(ok){
  assertNotEqual(2,3);//,F("notequal!!"));
}*/

void setup(){
  Serial.begin(9600);
   while(!Serial) {}
   //Serial.println("Start!");
}


void loop()
{
  Test::run();
};
