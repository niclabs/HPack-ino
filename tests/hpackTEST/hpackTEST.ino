#include "HPack.h"


void setup() {
  Serial.begin(9600);
  Serial.println(F("Start..."));


  /*int si = 5;
  char * ex1 = new char[si];
  for(int i=0; i<si; i++){
    ex1[i]='1';
  }
  ex1[si-1]='\0';
  char ex2[]="hola";
  char *ex3="hola";
  Serial.println(strlen(ex1));
  Serial.println(strlen(ex2));
  Serial.println(strlen(ex3));*/

  /*byte b[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
  HPack* hp = new HPack(b,15);
  //Serial.println(hp->toString());
  */
  /*
  int ints[] = {0,1,5,30,31,32,127,128,129,130,131,132,133,134,135,18000};
  for(uint8_t prefix = 0; prefix<8; prefix++){
      for(int i = 0; i<sizeof(ints)/sizeof(int); i++){
      Serial.println("-----------------------");
      Serial.print("Encoding Integer: ");
      Serial.println(ints[i]);
      Serial.print("with prefix: ");
      Serial.println(prefix);
      HPack *hpi = hp->encodeInteger(ints[i],prefix);
      Serial.println(hpi->toString());
      uint32_t decoded = hpi->decodeInteger(hpi,prefix);
      Serial.print("Decoding: ");
      Serial.println(decoded);
    }
  }
  */
/*  int ssize = 4;
  String stringss[ssize] = {"asasd","asdasdasdasdsgad","lñkasdjflñu843r 47ñSHIDFLAIWU3YRP98QYLKJWSHC Y498  U23HY4IU H,","sjlfhalskjeh liqwer5pq2u3i4l jkqhblt6u45h6 kjsjdghakuygckjhg4kuy5g45kcwhgvkjh3gl6,jhq4gkxui5k2u34cg5klxgh5kj4g5kj3gh5lx,jhbg6,ljcvh6l225uyc5hyvcuj3bhjhgq3lv4vglq3c3h5qch34g5lcjhlq3jh4g5clqj3hclqqlqhlq34h5cql34hclq3j4hl3jq4hl3h4"};
  for(int i = 0; i<ssize; i++){
      Serial.println("-----------------------");
      Serial.print("Encoding String: ");
      Serial.println(stringss[i]);
      Serial.print("Size: ");
      Serial.println(stringss[i].length());
      HPack *hpi = hp->encodeString(stringss[i],false);
      //Serial.println(hpi->octets[stringss[i].length()]);
      Serial.println(hpi->toString());
      String decoded = hpi->decodeString(hpi);
      Serial.print("Decoding: ");
      Serial.println(decoded);
  }
*/

  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());

  uint32_t incomming_buffer_max_size = 100;
  uint32_t max_table_size= 100;
  HPack* hpack = new HPack(incomming_buffer_max_size, max_table_size);
  HeaderBuffer *hb = hpack->hb;

  HPackData *hp; 
  HPackData *hp2;
  
  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());

  /*
  hp = hb->encodeIndexedHeaderField(100);
  Serial.println(hp->toString());
  Serial.println("-----------------------");
  hp = hb->encodeLiteralHeaderFieldIndexing(100, false,"valuekljhdkjasdhkasdj");
  Serial.println(hp->toString());
  Serial.println("-----------------------");
  hp = hb->encodeLiteralHeaderFieldIndexing("namesadasd", false,"valuekljhdkjasdhkasdj",false);
  Serial.println(hp->toString());
  Serial.println("-----------------------");
  hp = hb->encodeLiteralWithoutIndexing(100, false,"valuekljhdkjasdhkasdj");
  Serial.println(hp->toString());
  Serial.println("-----------------------");
  hp = hb->encodeLiteralWithoutIndexing("namesadasd", false,"valuekljhdkjasdhkasdj",false);
  Serial.println(hp->toString());
  Serial.println("-----------------------");
  hp = hb->encodeLiteralNeverIndexed(500, false, "valuekljhdkjasdhkasdj");
  Serial.println(hp->toString());
  Serial.println("-----------------------");
  */
//Serial.println(0x27u);
//Serial.println(0x27u,BIN);
  
  char * name_string = (char*)"holasd\0";
  char * value_string = (char*)"value1234567890\0";
  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  Serial.println(F("created name and value:"));
  
  Serial.println(F("-------------creating literalHeaderFieldWithIncrementalIndex.-------------"));
  hp = hb->literalHeaderFieldWithIncrementalIndex(true, name_string, false,value_string);//test indexing
  /*Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  Serial.println(F("deleting name and value:"));
  delete[](name);
  delete[](value);
  */
  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  
  hp->toString();

  Serial.println(F("-----------creating indexedHeaderField.-------------"));

  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
 
  hp2 = hb->indexedHeaderField((uint32_t)62);//test finding indexed h f
  
  hp2->toString();

  Serial.println(F("-------------adding literalHeaderFieldWithIncrementalIndex.-------------"));
  int size_added = hb->addData(hp);
  Serial.println(size_added);


  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  Serial.println(F("delete hp"));
  delete(hp);
  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  
  Serial.println(F("-----------adding indexedHeaderField.-------------"));
  size_added = hb->addData(hp2);
  Serial.println(size_added);

  delete(hp2);
  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  HPackData *hpd, *hpd2;
  Serial.println(F("Get next..."));
  hpd = hb->getNext();

  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
  
  hpd->toString();

  
  HeaderPair* hp1 = hb->getHeaderPair(hpd);
  hp1->toString();
  //delete[](hpd->get_name_string());
  //delete[](hpd->get_value_string());
  Serial.print(F("delete hpd"));
  delete(hpd);
  
  //delete[](hp1->name);
  //delete[](hp1->value);
  Serial.print(F("delete hp1"));
  delete(hp1);
  
  hpd2 = hb->getNext();
  hpd2->toString();
  
  HeaderPair* hp22 = hb->getHeaderPair(hpd2);
  hp22->toString();
  Serial.print(F("delete hpd2"));
  delete(hpd2);
  
  //delete[](hp22->name);
  //delete[](hp22->value);
  Serial.print(F("delete hp22"));
  delete(hp22);
  
  /*


  
  hpd->toString();
Serial.print("before next 2freeMemory()=");
    Serial.println(freeMemory());
  hpd2 = hb->getNext();
  //hpd2->toString();
  
  Serial.print("before delete hpairs freeMemory()=");
    Serial.println(freeMemory());
  //delete[](header_pair->name);
  //delete[](header_pair->value);
  //delete(header_pair);
  //delete[](header_pair2->name);
  //delete[](header_pair2->value);
  //delete(header_pair2);
  //delete[](name);
  //delete[](value);
  Serial.println("before deleting buffer");
  Serial.print("freeMemory()=");
    Serial.println(freeMemory());
  //header_pair = decodeLiteralHeaderField(hp, 4);//4 indicates never indexed
  //Serial.print("Decoded Header Pair: ");
  delay(100);
  //header_pair->toString();
  delay(100);
  Serial.println("-----------------------");
  //hb->toString();
  //Serial.println("-----------------------");
*/
/*
  hp = hb->encodeLiteralNeverIndexed("hola", true,"hola",false);
  Serial.println(hp->toString());
  Serial.println("-----------------------");

  hp = hb->encodeLiteralNeverIndexed("hola", true,"hola",true);
  Serial.println(hp->toString());
  Serial.println("-----------------------");

  hp = hb->encodeLiteralNeverIndexed("hola", false,"hola",true);
  Serial.println(hp->toString());
  Serial.println("-----------------------");


*/

  
  /*hp = hb->encodeDynamicTableSizeUpdate(500);
  Serial.println(hp->toString());
  Serial.println("-----------------------");
*/
 delete(hpack);
  //Serial.println(hb->dyn_table->length());
  /*for(int i = 0; i< hb->dyn_table->length(); i++){
    if(hb->dyn_table->table[i]!=nullptr){
      //Serial.println(i);
      delete[](hb->dyn_table->table[i]->name);
      delete[](hb->dyn_table->table[i]->value);
    }
  }
  Serial.println("before deleting dyntable");
  Serial.print("freeMemory()=");
    Serial.println(freeMemory());
  delete[](hb->dyn_table);
  delete(hb);
  Serial.println("after deleting buffer");*/
  Serial.print(F("freeMemory()="));
    Serial.println(freeMemory());
  Serial.println(F("FIN!"));
}

void loop() {
  // put your main code here, to run repeatedly:

}
