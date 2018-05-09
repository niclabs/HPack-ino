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
        
        EncodedData * encoded_integer = HPackCodec::encodeInteger(integers[i],prefix);
        uint32_t octets_length = getOctetsLength(integers[i],prefix);
        //Serial.print(F("encoded length: "));
        //Serial.println(octets_length);
        //for(int k =0; k<octets_length; k++){
          //Serial.println(encoded_integer[k],BIN);
        //}
        
        uint32_t decoded = HPackCodec::decodeInteger(encoded_integer->encoded_data,prefix, octets_length,0);  
        delete[](encoded_integer->encoded_data);       
        delete(encoded_integer);    
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
  EncodedData * encoded_string = HPackCodec::encodeString(string,huffman);

  //Serial.println((char*)encoded_string);
  //Serial.println(strlen((char*)encoded_string));
  char* string_decoded = HPackCodec::decodeString(encoded_string->encoded_data, huffman, encoded_string->length,0);

  //Serial.println(strlen(string));
  //Serial.println(strlen(string_decoded));
  //Serial.println(string_decoded[5],BIN);
  
  assertEqual(string, string_decoded);
  delete[](encoded_string->encoded_data);
  delete(encoded_string);
  delete[](string_decoded);
  assertEqual(freeMemory(),initial_memory);     
}

test(EncodeDecodeStringHuffman){
  int initial_memory = freeMemory();
  char* string;
  bool huffman;

  string = "Holaasdasdaksjdhaskjdhkjahdkjasd!|1\0";
  huffman = true;
  EncodedData * encoded_string = HPackCodec::encodeString(string,huffman);

  //Serial.println((char*)encoded_string);
  //Serial.println(strlen((char*)encoded_string));
  char* string_decoded = HPackCodec::decodeString(encoded_string->encoded_data, huffman, encoded_string->length, 0);

  //Serial.println(strlen(string));
  //Serial.println(strlen(string_decoded));
  //Serial.println(string_decoded[5],BIN);
  
  assertEqual(string, string_decoded);
  delete[](encoded_string->encoded_data);
  delete(encoded_string);
  delete[](string_decoded);
  assertEqual(freeMemory(),initial_memory);     
}



test(LiteralHeaderFieldNeverIndexedWithoutIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithoutIndexHuffman"));
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
  hp = hbs->literalNeverIndexed(true, name_string, true,value_string);

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
};

test(LiteralHeaderFieldNeverIndexedWithoutIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithoutIndexNoHuffman"));
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

  
  hp = hbs->literalNeverIndexed(false, name_string, false,value_string);
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


test(LiteralHeaderFieldNeverIndexedWithIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithIndexHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalNeverIndexed(1, true,value_string);
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


test(LiteralHeaderFieldNeverIndexedWithIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldNeverIndexedWithIndexNOHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalNeverIndexed(1, false,value_string);
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

test(LiteralHeaderFieldIndexingWithoutIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldIndexingWithoutIndexNoHuffman"));
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
  hp = hbs->literalHeaderFieldWithIncrementalIndex(false, name_string, false,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);

  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);

  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual(name_string, hp1->name);
  assertEqual(value_string, hp1->value);
  delete(hp1);
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


test(LiteralHeaderFieldIndexingWithoutIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldIndexingWithoutIndexHuffman"));
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
  hp = hbs->literalHeaderFieldWithIncrementalIndex(true, name_string, true,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual(name_string, hp1->name);
  assertEqual(value_string, hp1->value);
  delete(hp1);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


test(LiteralHeaderFieldIndexingWithIndexNoHuffman){
  Serial.println(F("Test LiteralHeaderFieldIndexingWithoutIndexNoHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  //char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalHeaderFieldWithIncrementalIndex(1, false,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0", hp1->name);
  assertEqual(value_string, hp1->value);
  delete(hp1);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


test(LiteralHeaderFieldIndexingWithIndexHuffman){
  Serial.println(F("Test LiteralHeaderFieldIndexingWithoutIndexHuffman"));
  int initial_memory = freeMemory();
  HPackData *hp, *hpd; 
  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
 HPack* hpack_sender = new HPack(incomming_buffer_max_size, max_table_size);
  HPack* hpack_receiver = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hbs = hpack_sender->hb;
  HeaderBuffer *hbr = hpack_receiver->hb;
  //char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  hp = hbs->literalHeaderFieldWithIncrementalIndex(1, true,value_string);
  EncodedData* ed = hbs->createHeaderBlock(&(HPackData*){hp},1);
  delete(hp);
  hbr->receiveHeaderBlock(ed->encoded_data, ed->length);
  delete[](ed->encoded_data);
  delete(ed);
  hpd = hbr->getNext();
  HeaderPair* hp1 = hbr->getHeaderPair(hpd);
  delete(hpd);
  assertEqual((char*)":authority\0", hp1->name);
  assertEqual(value_string, hp1->value);
  delete(hp1);
  
  delete(hpack_sender);
  delete(hpack_receiver);
  assertEqual(freeMemory(),initial_memory);      
}


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
/*

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

*/


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
