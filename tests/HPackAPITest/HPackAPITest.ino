#line 2 "HPachAPITest.ino"
#include <ArduinoUnit.h>
#include "HPack.h"


test(search_header_in_static_table){
  Serial.println(F("Test search_header_in_static_table"));
  int initial_memory = freeMemory();
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  
  //Test values

  //unknown name and value
  char * static_name_string_0 = (char*)"holasd\0";
  char * static_value_string_0 = (char*)"value1234567890\0";
  uint32_t index0 = hpack->search_header_in_static_table(static_name_string_0, static_value_string_0);
  assertEqual(index0,0);//TODO look for it.

  //known name and value (value empty)
  char * static_name_string_1 = (char*)":authority\0";
  char * static_value_string_1 = (char*)"\0";
  uint32_t index1 = hpack->search_header_in_static_table(static_name_string_1, static_value_string_1);
  assertEqual(index1,(uint32_t)1);//TODO look for it.

  //known name and value (value not empty)
  char * static_name_string_2 = (char*)":method\0";
  char * static_value_string_2 = (char*)"GET\0";
  uint32_t index2 = hpack->search_header_in_static_table(static_name_string_2, static_value_string_2);
  assertEqual(index2,(uint32_t)2);//TODO look for it.

  //known name and unknown value (value empty)
  char * static_name_string_3 = (char*)":method\0";
  char * static_value_string_3 = (char*)"\0";
  uint32_t index3 = hpack->search_header_in_static_table(static_name_string_3, static_value_string_3);
  assertEqual(index3,(uint32_t)0);//TODO look for it.

  //known name and value (value not empty)
  char * static_name_string_4 = (char*)":method\0";
  char * static_value_string_4 = (char*)"hola\0";  
  uint32_t index4 = hpack->search_header_in_static_table(static_name_string_4, static_value_string_4);
  assertEqual(index4,(uint32_t)0);//TODO look for it.

  //similar unknown known name and known value
  char * static_name_string_5 = (char*)":methodologia\0";
  char * static_value_string_5 = (char*)"GET\0";    
  uint32_t index5 = hpack->search_header_in_static_table(static_name_string_5, static_value_string_5);
  assertEqual(index5,(uint32_t)0);//TODO look for it.
 
  //known name and unknown value (value known empty)
  char * static_name_string_6 = (char*)":authority\0";
  char * static_value_string_6 = (char*)"hola\0";
  uint32_t index6 = hpack->search_header_in_static_table(static_name_string_6, static_value_string_6);
  assertEqual(index6,(uint32_t) 1);//TODO look for it.

  //símilar unknown name and value (name below the first)
  char * static_name_string_7 = (char*)":authorita\0";
  char * static_value_string_7 = (char*)"\0";
  uint32_t index7 = hpack->search_header_in_static_table(static_name_string_7, static_value_string_7);
  assertEqual(index7,(uint32_t)0);//TODO look for it.

  //símilar unknown name and value (name beyond the last)
  char * static_name_string_8 = (char*)"www-authenticato\0";
  char * static_value_string_8 = (char*)"\0";
  uint32_t index8 = hpack->search_header_in_static_table(static_name_string_8, static_value_string_8);
  assertEqual(index8,(uint32_t)0);//TODO look for it.

  //known name and unknown value (value beyond the first)
  char * static_name_string_9 = (char*)":authority\0";
  char * static_value_string_9 = (char*)"0\0";
  uint32_t index9 = hpack->search_header_in_static_table(static_name_string_9, static_value_string_9);
  assertEqual(index9,(uint32_t)1);//TODO look for it.

  //símilar unknown name and value (name beyond the last)
  char * static_name_string_10 = (char*)"www-authenticate\0";
  char * static_value_string_10 = (char*)"z\0";
  uint32_t index10 = hpack->search_header_in_static_table(static_name_string_10, static_value_string_10);
  assertEqual(index10,(uint32_t)61);//TODO look for it.

  
  //known name and value (last header)
  char * static_name_string_11 = (char*)"www-authenticate\0";
  char * static_value_string_11 = (char*)"\0";
  uint32_t index11 = hpack->search_header_in_static_table(static_name_string_11, static_value_string_11);
  assertEqual(index11,(uint32_t)61);//TODO look for it.

  delete(hpack);
  
  assertEqual(freeMemory(),initial_memory);     
}

test(search_header_in_dyn_table){
  Serial.println(F("Test search_header_in_dyn_table"));
  int initial_memory = freeMemory();

  uint32_t incomming_buffer_max_size = 300;
  uint32_t max_table_size= 300;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;
  
  
  
  //search in empty dyn table
  char * dyn_name_string_0 = (char*)"holasd\0";
  char * dyn_value_string_0 = (char*)"value1234567890\0";
  uint32_t index0 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_0);
  assertEqual(index0,(uint32_t)0);

  //adding first dynamic entry
  HPackData *hp0 = hb->literalHeaderFieldWithIncrementalIndex(false, dyn_name_string_0, false,dyn_value_string_0);
  EncodedData* ed = hb->createHeaderBlock(&(HPackData*){hp0},1);
  delete(hp0);
  //hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  //HPackData *hpd0 = hb->getNext();
  //delete(hpd0);

  
  //search unknown header pair in not empty dyn table
  char * dyn_name_string_1 = (char*)"chaos\0";
  char * dyn_value_string_1 = (char*)"asd\0";
  uint32_t index1 = hpack->search_header_in_dyn_table(dyn_name_string_1, dyn_value_string_1);
  assertEqual(index1,(uint32_t)0);

   
  //search known header in dyn table with one header(same header)
  uint32_t index2 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_0);
  assertEqual(index2,(uint32_t)62);
  
  //search known name, unknown value in dyn talble with one entry(below)
  char * dyn_value_string_2 = (char*)"value123456780\0";
  uint32_t index3 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_2);
  assertEqual(index3,(uint32_t)0);
  
  //search known name, unknown value in dyn talble with one entry(beyond)
  char * dyn_value_string_3 = (char*)"value12345678-\0";
  uint32_t index4 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_3);
  assertEqual(index4,(uint32_t)0);
  
  //search unknown name, known value in dyn table with one entry(below name)
  char * dyn_name_string_2 = (char*)"chaor\0";
  uint32_t index5 = hpack->search_header_in_dyn_table(dyn_name_string_2, dyn_value_string_1);
  assertEqual(index5,(uint32_t)0);

  //search unknown name, known value in dyn table with one entry(beyond name)
  char * dyn_name_string_3 = (char*)"chaot\0";
  uint32_t index6 = hpack->search_header_in_dyn_table(dyn_name_string_3, dyn_value_string_1);
  assertEqual(index6,(uint32_t)0);


  //adding second entry
  HPackData *hp1 = hb->literalHeaderFieldWithIncrementalIndex(false, dyn_name_string_1, true,dyn_value_string_1);
  EncodedData* ed1 = hb->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  delete[](ed1->encoded_data);
  delete(ed1);

  
  //search known header in dyn table with two header(first header)
  uint32_t index7 = hpack->search_header_in_dyn_table(dyn_name_string_1, dyn_value_string_1);
  assertEqual(index7,(uint32_t)62);

  
  //search known name, unknown value in dyn table with two entry (first header)
  uint32_t index8 = hpack->search_header_in_dyn_table(dyn_name_string_1, dyn_value_string_3);
  assertEqual(index8,(uint32_t)0);
  
  //search unknown name, known value in dyn table with two entry (first header)
  uint32_t index9 = hpack->search_header_in_dyn_table(dyn_name_string_2, dyn_value_string_1);
  assertEqual(index9,(uint32_t)0);
   
  //search known header in dyn table with two header(second header)
   uint32_t index10 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_0);
  assertEqual(index10,(uint32_t)63);

  
  //search known name, unknown value in dyn table with two entry(below second header)
  uint32_t index11 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_2);
  assertEqual(index11,(uint32_t)0);
  //search known name, unknown value in dyn table with two entry(beyond second header)
  uint32_t index12 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_3);
  assertEqual(index12,(uint32_t)0);


  //adding third entry
  HPackData *hp2 = hb->literalHeaderFieldWithIncrementalIndex(false, dyn_name_string_2, true,dyn_value_string_2);
  EncodedData* ed2 = hb->createHeaderBlock(&(HPackData*){hp2},1);
  delete(hp2);
  delete[](ed2->encoded_data);
  delete(ed2);

  //search known header in dyn table with three header
  uint32_t index13 = hpack->search_header_in_dyn_table(dyn_name_string_2, dyn_value_string_2);
  assertEqual(index13,(uint32_t)62);

  uint32_t index14 = hpack->search_header_in_dyn_table(dyn_name_string_1, dyn_value_string_1);
  assertEqual(index14,(uint32_t)63);

  uint32_t index15 = hpack->search_header_in_dyn_table(dyn_name_string_0, dyn_value_string_0);
  assertEqual(index15,(uint32_t)64);
  
  delete(hpack);
  
  assertEqual(freeMemory(),initial_memory);     
  
}

test(search_header){
  Serial.println(F("Test search_header"));
  int initial_memory = freeMemory();
  //HPackData *hp1, *hp2, *hpd1, *hpd2; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;


  char * static_name_string_1 = (char*)":authority\0";
  char * static_value_string_1 = (char*)"\0";
  uint32_t index1 = hpack->search_header(static_name_string_1, static_value_string_1);
  assertEqual(index1,(uint32_t)1);


   //search in empty dyn table
  char * dyn_name_string_1 = (char*)"holasd\0";
  char * dyn_value_string_1 = (char*)"value1234567890\0";
  uint32_t index2 = hpack->search_header(dyn_name_string_1, dyn_value_string_1);
  assertEqual(index2,(uint32_t)0);

  //adding first dynamic entry
  HPackData *hp1 = hb->literalHeaderFieldWithIncrementalIndex(false, dyn_name_string_1, false,dyn_value_string_1);
  EncodedData* ed1 = hb->createHeaderBlock(&(HPackData*){hp1},1);
  delete(hp1);
  delete[](ed1->encoded_data);
  delete(ed1);
  
  //search known header pair in not empty dyn table
  uint32_t index3 = hpack->search_header(dyn_name_string_1, dyn_value_string_1);
  assertEqual(index3,(uint32_t)62);

  delete(hpack);
  
  assertEqual(freeMemory(),initial_memory);     

}



test(EncodeDecodeStatic){
  Serial.println(F("Test EncodeDecode Static"));
  int initial_memory = freeMemory();
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);

  char * static_name_string_1 = (char*)":method\0";
  char * static_value_string_1 = (char*)"GET\0";
  EncodedData* ed = hpack_sender->encode(static_name_string_1, false,static_value_string_1, false,false,false);

  
  hpack_receiver->decode(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  HeaderPair* hp = hpack_receiver->getNextHeaderPair();
  
  assertEqual((char*)":method\0", hp->name);
  assertEqual((char*)"GET\0", hp->value);
  
  delete(hp);
  delete(hpack_sender);
  delete(hpack_receiver);

  assertEqual(freeMemory(),initial_memory); 
  
}

test(EncodeDecodeDynamic){
  Serial.println(F("Test EncodeDecode Dynamic"));
  int initial_memory = freeMemory();
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);

  char * dynamic_name_string_1 = (char*)":method\0";
  char * dynamic_value_string_1 = (char*)"GET\0";
  EncodedData* ed = hpack_sender->encode(dynamic_name_string_1, false,dynamic_value_string_1, false,false,false);

  
  hpack_receiver->decode(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  HeaderPair* hp = hpack_receiver->getNextHeaderPair();
  
  assertEqual((char*)":method\0", hp->name);
  assertEqual((char*)"GET\0", hp->value);
  
  delete(hp);
  delete(hpack_sender);
  delete(hpack_receiver);

  assertEqual(freeMemory(),initial_memory); 
}
/*


test(EncodeDecodeBoth){
  Serial.println(F("Test EncodeDecode Both"));
  int initial_memory = freeMemory();
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);



  char * static_name_string_1 = (char*)":authority\0";
  char * static_value_string_1 = (char*)"\0";


  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory); 
}

*/

void setup() {
  Serial.begin(9600);
   while(!Serial) {}
  Serial.println(F("Start..."));
   Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
}


void loop() {
  // put your main code here, to run repeatedly:
    Test::run();

}


