#include "HPack.h"
#include "HuffmanCodec.h"



/*Returns a string with de byte bits*/
char* byteToBits(byte b){
	char *s = new char[9];
	byte aux = (byte)128;
	for(uint32_t i = 0; i < 8; i++){
		if(b & aux){
			s[i] = '1';
		}else{
			s[i] = '0';
		}
		aux = aux >> 1;
	}
	s[8] = '\0';
	return s;
};

/*returns the amount of octets used to encode a int I with a prefix p*/
uint32_t getOctetsLength(uint32_t I, uint8_t px){
	//Serial.println("getOctetsLength");
	byte p = 255;
	p = p << (8 - px);
	p = p >> (8 - px);
	if(I >= p){
		uint32_t k = log(I - p) / log(128);
		return k + 2;
	}else{
		return 1;
	}
};

/*creates a header Pair without duplication of name and value*/
HeaderPair::HeaderPair(char* n, char* v){
	//Serial.println("HeaderPair");
	this->name = n;
	this->value = v;
};

static void HeaderPair::operator delete(void *ptr){
	//Serial.println(F("HeaderPair delete"));
	::operator delete[](((HeaderPair*)ptr)->name);
	::operator delete[](((HeaderPair*)ptr)->value);		
    ::operator delete(ptr);
}

/*duplicates the given header pair*/
HeaderPair* HeaderPair::duplicateHeaderPair(){
	//Serial.println(F("duplicateHeaderPair"));
	char* d_name = new char[strlen(this->name)+1];
	char* d_value = new char[strlen(this->value)+1];
	strncpy(d_name, this->name, strlen(this->name)+1);
	strncpy(d_value, this->value, strlen(this->value)+1);

	return new HeaderPair(d_name, d_value);
};


uint32_t HeaderPair::size(){
	return strlen(name)+strlen(value);
};

void HeaderPair::toString(){
	Serial.println(F("HeaderPair::toString "));
	Serial.print(F("\tname:"));
	Serial.println(this->name);
	Serial.print(F("\tvalue:"));
	Serial.println(this->value);
};

HPackDynamicTable::HPackDynamicTable(uint32_t max_size){
	this-> max_size = max_size;
	table_length = (uint32_t)(max_size / 32)+1;
	table = new HeaderPair*[table_length];
	first = 0;
	next = 0;
};



static void HPackDynamicTable::operator delete(void *ptr){
	//Serial.println(F("HPackDynamicTable delete"));
	//Serial.println(((HPackDynamicTable*)ptr)->length());
	for(int i = ((HPackDynamicTable*)ptr)->first,k=0; 
		k<((HPackDynamicTable*)ptr)->length(); 
		(i=i+1)%((HPackDynamicTable*)ptr)->table_length,k++){
		//Serial.println(F("delete entry sd"));
		HeaderPair::operator delete(((HPackDynamicTable*)ptr)->table[i]);			
	}
	::operator delete[](((HPackDynamicTable*)ptr)->table);
    ::operator delete(ptr);
}

//get amount of bytes saved in the table
uint32_t HPackDynamicTable::size(){
	uint32_t total_size = 0;
	uint32_t table_length_used = this->length();
	//Serial.print(F("table length used: "));
	//Serial.println(table_length_used);
	//Serial.print(F("table first: "));
	//Serial.println(first);
	for(uint32_t i=0; i < table_length_used; i++){
		total_size += table[(i+first)%table_length]->size() + 32;
	}
	return total_size;
};

//get amount of entries
uint32_t HPackDynamicTable::length(){
	uint32_t table_length_used = first < next ? (next - first) % table_length : (table_length - first + next)% table_length ;//TODO check this!!
	return table_length_used
;};

void HPackDynamicTable::deleteEntry(uint32_t index){
	//delete[](table[index]->name);
	//delete[](table[index]->value);
	delete(table[index]);
};

//and a header pair to the dynamic table (delete others if needed)
bool HPackDynamicTable::addEntry(HeaderPair* entry){
	uint32_t entry_size = entry->size()+32;
	if(entry_size > max_size){
		//Serial.println(F("entry exceeds size of table"));
		return false;
	}
	//Serial.print(F("current dyn table size"));
	//Serial.println(this->size());
	while(entry_size + this->size() > max_size){//if not available space...	
		//delete "first" entry
		//Serial.println(F("delete 'first' entry, not enough space..."));
		deleteEntry(first);
		first = (first + 1) % table_length;
	}
	//Serial.print(F("added in "));
	//Serial.println(next);
	//entry->toString();
	table[next] = entry;//->duplicateHeaderPair();
	next = (next + 1) % table_length;
	//Serial.print(F("current dyn table size after addition"));
	//Serial.println(this->size());
	return true;
};

HeaderPair* HPackDynamicTable::findEntry(uint32_t index){
	//Serial.print(F("at index:"));
	//Serial.println(index);
	uint32_t table_index = (next + table_length -(index-61)) % table_length;
	//Serial.print(F("at table index:"));
	//Serial.print(table_index);
	//Serial.println(F("found:"));
	//table[table_index]->toString();
	return table[table_index]->duplicateHeaderPair();
};

bool HPackDynamicTable::resize(uint32_t new_max_size){
	uint32_t new_table_length = (uint32_t)(new_max_size / 32) + 1;
	HeaderPair ** new_table =  new HeaderPair*[new_table_length];
	uint32_t new_first = 0;
	uint32_t new_next = 0;
	
	uint32_t new_size = 0;
	uint32_t i = first;
	uint32_t j = 0; //counter of already used table slots

	uint32_t table_length_used = length();
	//Serial.print(F("dynamic table used length: "));
	//Serial.println(table_length_used);
	
	while(j < table_length_used && new_size + table[i]->size()+32 <= new_max_size){		
		//Serial.println(F("moving entry to new table"));
		new_table[j] = table[i];//assign data in old table to new table
		i = (i + 1) % table_length;
		j++;
		new_next = (new_next + 1) % new_table_length;
		new_size += table[i]->size()+32;
	}

	//delete the older table and its -unused- content
	//int table_length_used = length(); already declared above
	uint32_t new_table_length_used = new_first <= new_next ? (new_next - new_first) % new_table_length : new_table_length - new_first + new_next;
	uint32_t unused_data_size = table_length_used - new_table_length_used;
	for(uint32_t k = 0; k < unused_data_size; k++){
		//Serial.print(F("deleting older table entries: "));
		//Serial.println((k + first) % table_length);
		
		deleteEntry((k + first) % table_length);
	}

	//Serial.println(F("deleted older table entries"));
	delete[](table);//Check this...
	//reassign new data
	max_size = new_max_size;
	table_length = new_table_length;
	next = new_next;
	first = new_first;
	table = new_table;
	return true;

};

HPackData::HPackData(uint8_t preamble, uint32_t index){
	//Serial.println("new hpack data: ");
	this->preamble = (uint8_t)preamble;
	this->index = (uint32_t)index;
};


HPackData::HPackData(uint8_t preamble, uint32_t index, bool value_huffman, char* value_string){
	this->preamble = preamble;
	this->index = index;//TODO check index!=0
	this->value_huffman = value_huffman;
	this->value_string = new char[strlen(value_string)+1];
	strncpy(this->value_string,value_string,strlen(value_string)+1);
};

HPackData::HPackData(uint8_t preamble, bool name_huffman, char* name_string, bool value_huffman, char* value_string){
	this->preamble = preamble;
	this->index = (uint32_t)0;
	this->name_huffman = name_huffman;
	this->name_string = new char[strlen(name_string)+1];
	this->value_huffman = value_huffman;
	this->value_string = new char[strlen(value_string)+1];
	strncpy(this->name_string,name_string,strlen(name_string)+1);
	strncpy(this->value_string,value_string,strlen(value_string)+1);
};

HPackData::HPackData(uint32_t max_size){
	this->preamble = (uint8_t)32;
	this->max_size = max_size;
};

static void HPackData::operator delete(void *ptr) {
	//Serial.println(F("HPackData delete"));

	uint8_t preamble = ((HPackData*)ptr)->preamble;
	
	if(preamble!=(uint8_t)32 && preamble!=(uint8_t)128 ){
		uint32_t index = ((HPackData*)ptr)->index;
		if(index==0){
			//Serial.println(F("Deleting name"));
			::operator delete[](((HPackData*)ptr)->name_string);
			//Serial.println(F("Deleted name"));
		}
		//Serial.println(F("Deleting value"));
		::operator delete[](((HPackData*)ptr)->value_string);
		//Serial.println(F("Deleted value"));
		
	}
    ::operator delete(ptr);
}


uint8_t HPackData::get_preamble(){
	return this->preamble;
};
uint32_t HPackData::get_index(){
	return this->index;
};
bool HPackData::get_name_huffman(){
	return this->name_huffman;
};
char* HPackData::get_name_string(){
	return this->name_string;
};
bool HPackData::get_value_huffman(){
	return this->value_huffman;
};
char* HPackData::get_value_string(){
	return this->value_string;
};
uint16_t HPackData::get_max_size(){
	return this->max_size;
};



void HPackData::toString(){
	Serial.println(F("HPackData: "));
	Serial.print(F("preamble: "));
	Serial.println(preamble);

	if(preamble&(uint8_t)32){
		Serial.print(F("max_size: "));
		Serial.println(max_size);	
	}else{
		Serial.print(F("index: "));
		Serial.println(index);
		if(!(preamble&(uint8_t)128)){
			//Serial.println("asdasd");
			if(index==(uint32_t)0){
				Serial.print(F("name_huffman: "));
				Serial.println(name_huffman);
				Serial.print(F("name_string: "));
				Serial.println(name_string);
			}
			Serial.print(F("value_huffman: "));
			Serial.println(value_huffman);
			Serial.print(F("value_string: "));
			Serial.println(value_string);
		}
	}
}



EncodedData* HPackCodec::encodeInteger(uint32_t integer, uint8_t prefix){
	//Serial.println(F("----HPackCodec::encodeInteger"));
	byte* octets;
	uint32_t octets_size;
	uint32_t max_first_octet = (1<<prefix)-1;
	//Serial.print(F("2^n-1: "));
	//Serial.println(max_first_octet);
	if(integer < max_first_octet){
		octets_size = 1;
		octets = new byte[octets_size];
		octets[0] = (byte)(integer << (8 - prefix));
		octets[0] = (byte)octets[0] >> (8 - prefix);
		//Serial.println(octets[0],BIN);
	}else{
		//Serial.println(F("GEQ than 2^n-1"));
		byte b0 = 255; 
		b0 = b0 << (8 - prefix);
		b0 = b0 >> (8 - prefix); 
		integer = integer - b0;
		uint32_t k = log(integer)/log(128);
		octets_size = k+2;
		octets = new byte[octets_size];
		octets[0] = b0;
		//Serial.println(octets[0],BIN);
		
		uint32_t i = 1;
		while(integer >= 128){
			uint32_t encoded = integer % 128;
			encoded += 128;
			octets[i] = (byte) encoded;
			//Serial.println(octets[i],BIN);
			i++;
			integer = integer/128;
		}
		byte bi = (byte) integer & 0xff;
		octets[i] = bi;
		//Serial.println(octets[i],BIN);

	}
	EncodedData *ed = (EncodedData*)malloc(sizeof(EncodedData));
	ed->encoded_data = octets;
	ed->length = octets_size;
	return ed;
};

/*byte* HPackData::encodeString(char* s, bool huffman){
	Serial.println(F("----HPackData::encodeString"));
	Serial.println(s);
	
	if(huffman){
		Serial.println(F("Encoding huffman string"));
		//encode word
		HuffmanEncodedWord *encoded_word = HuffmanCodec::encodeWord(s);		
		//pack and encode encoded_word length
		byte* encoded_string_length = HPackCodec::encodeInteger(encoded_word->length(),7);
		Serial.print(F("encoded_word_length"));
		Serial.println(encoded_word->length());

		encoded_string_length[0] |= (uint8_t)128;
		uint32_t new_size = encoded_word->length()+strlen((char*)encoded_string_length)+1;
		byte * encoded_string = new byte[new_size];
		for(uint32_t i = 0; i < strlen((char*)encoded_string_length); i++){
			encoded_string[i] = encoded_string_length[i];
		}
		
		for(uint32_t i = 0; i < encoded_word->length(); i++){
			encoded_string[i+strlen((char*)encoded_string_length)] = encoded_word->enc_word[i];
		}
		delete[](encoded_string_length);
		delete(encoded_word);
		encoded_string[new_size-1]='\0';
		Serial.print(F("encoded: "));
		Serial.println((char*)encoded_string);
		for(int i =0; i< new_size; i++){
			char* bits = byteToBits(encoded_string[i]);
			Serial.println(bits);	
			delete[](bits);
		}
		

		return encoded_string;
	}else{
		Serial.println(F("Encoding non huffman string"));
		byte* encoded_string_length = HPackCodec::encodeInteger(strlen(s),7);
		byte *encoded_string = new byte[strlen(s)+strlen((char*)encoded_string_length)+1];
		for(uint32_t i = 0; i<strlen((char*)encoded_string_length); i++){
			encoded_string[i]=encoded_string_length[i];
		}
		for(uint32_t i = 0; i< strlen(s); i++){
			encoded_string[i+strlen((char*)encoded_string_length)]=s[i];
		}
		encoded_string[strlen(s)+strlen((char*)encoded_string_length)]='\0';
		delete[](encoded_string_length);
		Serial.print(F("encoded: "));
		Serial.println((char*)encoded_string);
		return encoded_string;
	}
};

*/


EncodedData* HPackCodec::encodeString(char* s, bool huffman){
	//Serial.println(F("----HPackData::encodeString"));
	//Serial.println(s);
	//EncodedData *ed = (EncodedData*)malloc(sizeof(EncodedData));


	if(huffman){
			//ed->encoded_data = octets;
			//ed->length = octets_size;
		return encodeHuffmanString(s);
	}else{
			//ed->encoded_data = octets;
			//ed->length = octets_size;
		return encodeNonHuffmanString(s);
	}
};
EncodedData* HPackCodec::encodeNonHuffmanString(char* s){
	EncodedData *ed = (EncodedData*)malloc(sizeof(EncodedData));
	//Serial.println(F("Encoding non huffman string"));
	EncodedData* encoded_string_length = encodeInteger(strlen(s),7);
	//Serial.print(F("encoded_string_length: "));
	//Serial.println(encoded_string_length[0],BIN);
	byte *encoded_string = new byte[strlen(s)+encoded_string_length->length+1];
	for(uint32_t i = 0; i<encoded_string_length->length; i++){
		encoded_string[i]=encoded_string_length->encoded_data[i];
	}
	for(uint32_t i = 0; i< strlen(s); i++){
		encoded_string[i+encoded_string_length->length]=s[i];
	}
	encoded_string[strlen(s)+encoded_string_length->length]='\0';
	delete[](encoded_string_length->encoded_data);
	delete(encoded_string_length);
	//Serial.print(F("encoded: "));
	//Serial.println((char*)encoded_string);
	ed->encoded_data = encoded_string;
	ed->length = strlen(s)+encoded_string_length->length;
	return ed;
}

EncodedData* HPackCodec::encodeHuffmanString(char* s){
	EncodedData *ed = (EncodedData*)malloc(sizeof(EncodedData));
	//Serial.println(F("Encoding huffman string"));
	//encode word
	HuffmanEncodedWord *encoded_word = HuffmanCodec::encodeWord(s);		
	//pack and encode encoded_word length
	EncodedData* encoded_string_length = encodeInteger(encoded_word->length(),7);
	//Serial.print(F("encoded_word_length"));
	//Serial.println(encoded_word->length());

	encoded_string_length->encoded_data[0] |= (uint8_t)128;
	uint32_t new_size = encoded_word->length()+encoded_string_length->length;
	byte * encoded_string = new byte[new_size];
	for(uint32_t i = 0; i < encoded_string_length->length; i++){
		encoded_string[i] = encoded_string_length->encoded_data[i];
	}
	
	for(uint32_t i = 0; i < encoded_word->length(); i++){
		encoded_string[i+encoded_string_length->length] = encoded_word->enc_word[i];
	}
	delete[](encoded_string_length->encoded_data);
	delete(encoded_string_length);

	delete(encoded_word);
	//encoded_string[new_size]='\0';
	//TODO check this (up)

	//Serial.print(F("encoded: "));
	//Serial.println((char*)encoded_string);
	//for(int i =0; i< new_size; i++){
		//char* bits = byteToBits(encoded_string[i]);
		//Serial.println(bits);	
		//delete[](bits);
	//}

	ed->encoded_data = encoded_string;
	ed->length = new_size;

	return ed;
};
uint8_t HPackData::findPrefix(byte octet){

	if((byte)128&octet){
		return (byte)7;
	}
	if((byte)64&octet){
		return (byte)6;
	}
	
	if((byte)32&octet){
		return (byte)5;
	}
	return (byte)4;
};



EncodedData* HPackData::encode(){
	//Serial.println(F("---HPackData::encode"));
	if(preamble == (uint8_t)32){ // dynamicTableSizeUpdate
		EncodedData* encoded_max_size = HPackCodec::encodeInteger(this->max_size, 5);
		encoded_max_size->encoded_data[0] |= this->preamble;
		return encoded_max_size;//->encoded_data;
	}else{
		uint8_t prefix = findPrefix(preamble);
		EncodedData* encoded_index = HPackCodec::encodeInteger(this->index, prefix);		
		encoded_index->encoded_data[0] |= this->preamble;
		if(preamble == (uint8_t)128){
			//DO stuff ?		
			return encoded_index;//->encoded_data;
		}else{



			uint32_t index_octets_length = getOctetsLength(this->index,prefix);
			EncodedData* encoded_value = HPackCodec::encodeString(this->value_string, this->value_huffman);
			uint32_t encoded_value_octets_length = encoded_value->length;
			uint32_t octets_size = index_octets_length+encoded_value_octets_length;

			EncodedData* encoded_name;
			uint32_t encoded_name_octets_length = 0;
			if(this->index==(uint8_t)0){
				encoded_name = HPackCodec::encodeString(this->name_string, this->name_huffman);
				encoded_name_octets_length = encoded_name->length;
				octets_size += encoded_name_octets_length;
			}
			byte* octets = new byte[octets_size];

			uint32_t aux = 0;
			for(uint32_t i = 0; i< index_octets_length; i++){
				octets[i]=encoded_index->encoded_data[i];
				
			}
			aux = aux + index_octets_length;
			delete[](encoded_index->encoded_data);
			delete(encoded_index);
			for(uint32_t i =0; i< encoded_name_octets_length; i++){
				octets[aux + i] = encoded_name->encoded_data[i];
				
			}
			aux = aux + encoded_name_octets_length;
			if(this->index==0){
				delete[](encoded_name->encoded_data);
				delete(encoded_name);
			}
			for(uint32_t i =0; i<encoded_value_octets_length; i++){
				octets[aux + i] = encoded_value->encoded_data[i];
				
			}
			aux = aux + encoded_value_octets_length;
			delete[](encoded_value->encoded_data);
			delete(encoded_value);
			//octets[aux] = '\0';

			//Serial.println(F("octets encoded: "));
			for(int i =0; i< aux; i++){
				//Serial.println(byteToBits(octets[i]));
			}
			//DO stuff whith preambles...??
			if(preamble == (uint8_t)64){ //literalHeaderFieldWithIncrementalIndex
			//DO stuff ?
			}else if(preamble == (uint8_t)0){ //literalHeaderFieldWithoutIndexing
			//DO stuff ?
			}else if(preamble == (uint8_t)16){ //literalHeaderFieldNeverIndexed
			//DO stuff ?
			}else if(preamble == (uint8_t)32){ //dynamicTableSizeUpdate
			//DO stuff ?
			}else if(preamble == (uint8_t)128){ //indexedHeaderField
			//DO stuff ?
			}

			EncodedData *ed = (EncodedData*)malloc(sizeof(EncodedData));
			ed->encoded_data = octets;
			ed->length = octets_size;

			return ed;
		}
	}
};


HeaderBuffer::HeaderBuffer(uint32_t incomming_buffer_max_size,  uint32_t maxSize){
	//Serial.println(F("----HeaderBuffer"));
	this->buf_size = incomming_buffer_max_size;
	this->buf = new byte[buf_size+1];//TODO check +1
	this->first = 0;
	this->next = 0;
	this->dyn_table = new HPackDynamicTable(maxSize);
};

static void HeaderBuffer::operator delete(void *ptr){
	//Serial.println(F("HeaderBuffer delete"));
	::operator delete[](((HeaderBuffer*)ptr)->buf);	
	HPackDynamicTable::operator delete(((HeaderBuffer*)ptr)->dyn_table);	
    ::operator delete(ptr);
}





uint32_t HeaderBuffer::availableBufSize(){
	//Serial.println(F("----availableBufSize"));
	uint32_t availableSize;
	if(next<first)
		availableSize = first-next;
	else{
		availableSize = buf_size+first-next;
	}
	//Serial.println(availableSize);
	return availableSize;
};

EncodedData* HeaderBuffer::createHeaderBlock(HPackData** hp_datas, uint32_t size){
	//Serial.println(F("----createHeaderBlock"));
	//first creat buffer big enough (then resize)
	byte* block_buffer = new byte[buf_size];
	int size_available = buf_size;
	int index_block = 0;
	for(int i =0; i< size; i++){
		HPackData* hp_data = (HPackData*)(hp_datas[i]);
		EncodedData* enc_data = hp_data->encode();
		if(size_available>=enc_data->length){
			//add data to block
			for(int k = 0; k< enc_data->length; k++){
				block_buffer[index_block] = enc_data->encoded_data[k];
				index_block++;
			}

		}
		else{
			Serial.println(F("headerblock too small..."));
			return NULL;
		
		}
		delete[](enc_data->encoded_data);
		delete(enc_data);
	}
	//if everything went ok, then index the data into dyn table
	for(int i =0; i< size; i++){
		//Serial.println(F("indexing data"));
		HPackData* hp_data = (HPackData*)(hp_datas[i]);
		indexData(hp_data);
	};

	byte* new_block_buffer = new byte[index_block];

	for(int i=0;i<index_block;i++){
		new_block_buffer[i] = block_buffer[i];
	}
	delete[](block_buffer);

	EncodedData *ed = (EncodedData*)malloc(sizeof(EncodedData));
	ed->encoded_data = new_block_buffer;
	ed->length = index_block;
	return ed;
};



void HeaderBuffer::indexData(HPackData* data){
	//Serial.println(F("----indexData"));
	uint8_t preamble = data->get_preamble();
	//adding header...	
	if(preamble==(uint8_t)64){ //literal header field with indexing
		HeaderPair * new_header_pair;
		char *name_cpy;
		if(data->get_index()!=0){ 
			HeaderPair *name_hp = findEntry(data->get_index());
			name_cpy = new char[strlen(name_hp->name)+1];
			strncpy(name_cpy, name_hp->name, strlen(name_hp->name)+1);
			delete(name_hp);
		}else{
			name_cpy=new char[strlen(data->get_name_string())+1];
			strncpy(name_cpy, data->get_name_string(), strlen(data->get_name_string())+1);
		}
		char *value_cpy = new char[strlen(data->get_value_string())+1];
		strncpy(value_cpy, data->get_value_string(), strlen(data->get_value_string())+1);
		new_header_pair = new HeaderPair(name_cpy, value_cpy);
		//}else{//preamble==(uint8_t)128, indexed header field
		
		//HeaderPair *indexed_hp = findEntry(data->get_index());
		//delete(indexed_hp);
		
		//add data to dyn table in given index
		//Serial.print(F("adding to dyn table: "));
		//new_header_pair->toString();
		dyn_table->addEntry(new_header_pair);//TODO check if added...
		//delete(new_header_pair);
	}
	if(preamble==(uint8_t)32){//preamble==(uint8_t)32){ dynamic table size update
		//modify dynamic table size
		//TODO check size
		//Serial.print(F("adding dynamic table size update"));
		dyn_table->resize(data->get_max_size());
	}

}



/*
uint32_t HeaderBuffer::addData(HPackData * data){
	Serial.println(F("---addHPackData"));
	EncodedData* encoded_data = data->encode();

	int encoded_data_length = encoded_data->length; //TODO fix this-> cuando preamble and index ==0-> strlen==0
	if(encoded_data_length > availableBufSize()){
		Serial.println(F("No buffer available"));
		return 0;
	}

	uint8_t preamble = data->get_preamble();
	//adding header...
		
	if(preamble==(uint8_t)64){ //literal header field with indexing
		HeaderPair * new_header_pair;
		char *name_cpy;
		if(data->get_index()!=0){ 
			HeaderPair *name_hp = findEntry(data->get_index());
			name_cpy = new char[strlen(name_hp->name)+1];
			strncpy(name_cpy, name_hp->name, strlen(name_hp->name)+1);
			delete(name_hp);
		}else{
			name_cpy=new char[strlen(data->get_name_string())+1];
			strncpy(name_cpy, data->get_name_string(), strlen(data->get_name_string())+1);
		}
		char *value_cpy = new char[strlen(data->get_value_string())+1];
		strncpy(value_cpy, data->get_value_string(), strlen(data->get_value_string())+1);
		new_header_pair = new HeaderPair(name_cpy, value_cpy);
		//}else{//preamble==(uint8_t)128, indexed header field
		
		//HeaderPair *indexed_hp = findEntry(data->get_index());
		//delete(indexed_hp);
		
		//add data to dyn table in given index
		Serial.print(F("adding to dyn table: "));
		new_header_pair->toString();
		dyn_table->addEntry(new_header_pair);//TODO check if added...
		//delete(new_header_pair);
	}
	if(preamble==(uint8_t)32){//preamble==(uint8_t)32){ dynamic table size update
		//modify dynamic table size
		//TODO check size
		Serial.print(F("adding dynamic table size update"));
		dyn_table->resize(data->get_max_size());
	}
	
	Serial.print(F("adding data to buffer: "));
	Serial.println(encoded_data_length);

	for(uint32_t i = 0; i<encoded_data_length; i++){
		this->buf[next] = encoded_data->encoded_data[i];
		//char * bits = byteToBits(encoded_data->encoded_data[i]);
		//Serial.println(bits);
		//Serial.println(buf[next]);
		//delete[](bits); 
		next = (next+1)%buf_size;
	}

	delete[](encoded_data->encoded_data);
	delete(encoded_data);
	return encoded_data_length;

};*/


uint32_t HeaderBuffer::receiveHeaderBlock(byte* header_block, uint32_t size){
	//Serial.println(F("----receiveHeaderBlock"));
	if(availableBufSize()>=size){
		for(int i =0; i< size; i++){
			buf[next] = header_block[i];
			//Serial.println((char)buf[next]);
			next = (next+1)%buf_size;
		}
	}else{
		Serial.println("Error: ---------------------------------------------no buffer available");
		return -1;
	}
	
};

bool HeaderBuffer::isHuffmanEncoded(uint32_t pointer){
	//Serial.println(F("is huffman?"));
	/*char* bits = byteToBits(buf[pointer]);
	Serial.println(bits);	
	delete[](bits);*/
	//Serial.println(byteToBits(buf[pointer]));
	bool b = (bool)((buf[pointer])&128);
	//Serial.println((bool)b);
	return b;
}

//decodes an integer from the octets saved in the buffer starting at the pointer position with prefix given. See documentation HPACK
uint32_t HeaderBuffer::decodeInteger(uint32_t pointer, uint8_t prefix){
	//Serial.println(F("HeaderBuffer::decodeInteger"));
	return HPackCodec::decodeInteger(buf,prefix, buf_size,pointer);
	/*byte b0 = buf[pointer];
	b0 = b0<<(8-prefix);
	b0 = b0>>(8-prefix);
	byte p = 255;
	p = p<<(8-prefix);
	p = p>>(8-prefix);
	if(b0!=p){
		return (uint32_t) b0;
	}else{
		uint32_t size = buf_size-availableBufSize();
		uint32_t integer = (uint32_t)p;
		uint32_t depth = 0;
		for(uint32_t i = 1; i<size; i++){
			byte bi = buf[(pointer+i)%buf_size];
			if(!(bi&128)){
				integer += (uint32_t)bi*((uint32_t)1<<depth);
				Serial.println(integer);
				return integer;
			}else{
				bi = bi<<1;
				bi = bi>>1;
				integer += (uint32_t)bi*((uint32_t)1<<depth);
			}
			depth = depth+7;
		}
	}
	Serial.println(F("Error: decode Integer error"));
	return -1;*/
};


uint32_t HPackCodec::decodeInteger(byte* encodedInteger, uint8_t prefix, int max_buf_size, int start_pointer){
	//Serial.println(F("HPackCodec::decodeInteger"));
	//int pointer = 0;
	byte b0 = encodedInteger[start_pointer];
	b0 = b0<<(8-prefix);
	b0 = b0>>(8-prefix);
	byte p = 255;
	p = p<<(8-prefix);
	p = p>>(8-prefix);
	//Serial.print(F("p:"));
	//Serial.println(p,BIN);
	//Serial.print(F("first byte:"));
		//Serial.println(encodedInteger[(start_pointer)]);
	if(b0!=p){
		return (uint32_t) b0;
	}else{
		uint32_t size = max_buf_size;//buf_size-availableBufSize();
		//Serial.print(F("second byte:"));
		//Serial.println(encodedInteger[(start_pointer+1)%max_buf_size]);

		//Serial.print(F("max buf size:"));
		/*Serial.println(max_buf_size);
		if(max_buf_size==1){
			return b0;
		}else{
			if(encodedInteger[(start_pointer+1)%max_buf_size]==(byte)0){
				return b0;
			}	
		}
		*/
		//Serial.print(F("size:"));
		//Serial.println(size);
		uint32_t integer = (uint32_t)p;
		uint32_t depth = 0;
		for(uint32_t i = 1; i<size+1; i++){
			byte bi = encodedInteger[(start_pointer+i)%max_buf_size];
			
			//Serial.println(bi,BIN);
			if(!(bi&128)){
				integer += (uint32_t)bi*((uint32_t)1<<depth);
				//Serial.println(integer);
				return integer;
			}else{
				bi = bi<<1;
				bi = bi>>1;
				integer += (uint32_t)bi*((uint32_t)1<<depth);
			}
			depth = depth+7;
		}
	}
	//Serial.println(F("Error: decode Integer error"));
	return -1;
}
char* HeaderBuffer::decodeString(uint32_t pointer, bool huffman, uint32_t size){
	
	return HPackCodec::decodeString(buf, huffman, buf_size, pointer);
	/*
	Serial.println(F("----HeaderBuffer::decodeString"));
	if(huffman){	
		Serial.println(F("decoding huffman String"));
		byte * octets = new byte[size+1];
		for(uint32_t k = 0; k < size; k++){
			octets[k] = buf[(pointer+k)%buf_size];
			char * bits = byteToBits(octets[k]);
			Serial.println(bits);
			delete[](bits);
		}
		octets[size] = '\0';
		char* s = HuffmanCodec::decodeBytes(octets,size);
		delete[](octets);
		return s;
	}else{
		Serial.println(F("decoding NO huffman String"));
		char *s = new char[size+1];// = (char*)malloc(slength);
		for(uint32_t k = 0; k < size; k++){
			
			s[k] = (char)(buf[(pointer+k)%buf_size]);
		}
		s[size]='\0';
		return s;
	}*/
};


char* HPackCodec::decodeString(byte* buf, bool huffman, uint32_t buf_size,int pointer){
	//int pointer = 0;
	//Serial.println(F("----HPackCodec::decodeString"));

	uint32_t encoded_string_length = decodeInteger(buf,7,buf_size, pointer);
	//Serial.print(F("encoded_string_length"));
	//Serial.println(encoded_string_length);
	uint32_t encoded_string_length_octets_size = getOctetsLength(encoded_string_length,7);
	pointer = pointer + encoded_string_length_octets_size;

	if(huffman){	
		//Serial.println(F("decoding huffman String"));
		byte * octets = new byte[encoded_string_length];
		for(uint32_t k = 0; k < encoded_string_length; k++){
			octets[k] = buf[(pointer+k)%buf_size];
			//char * bits = byteToBits(octets[k]);
			//Serial.println(bits);
			//delete[](bits);
		}
		//octets[encoded_string_length] = '\0';
		char* s = HuffmanCodec::decodeBytes(octets,encoded_string_length);
		delete[](octets);
		return s;
	}else{
		//Serial.println(F("decoding NO huffman String"));
		char *s = new char[encoded_string_length+1];// = (char*)malloc(slength);
		for(uint32_t k = 0; k < encoded_string_length; k++){
			s[k] = (char)(buf[(pointer+k)%buf_size]);
		}
		s[encoded_string_length]='\0';
		return s;
	}
};


HPackData* HeaderBuffer::indexedHeaderField(uint32_t index){
	//Serial.print(F("creating indexedHeaderField: "));
	return new HPackData((uint8_t)128, index);
};

uint32_t HeaderBuffer::getBufSize(){
	return this->buf_size;
};

uint32_t HeaderBuffer::getFirst(){
	return this->first;
	
};


HPackData* HeaderBuffer::literalHeaderField(uint8_t preamble, uint32_t index, bool huffman, char* value){
	return new HPackData(preamble, index, huffman, value);
}
HPackData* HeaderBuffer::literalHeaderField(uint8_t preamble, bool nameHuffman,char* name, bool valueHuffman, char* value){
	return new HPackData(preamble, nameHuffman, name, valueHuffman, value);
}

HPackData* HeaderBuffer::literalHeaderFieldWithIncrementalIndex(uint32_t index, bool huffman, char* value){
	return new HPackData((uint8_t)64, index, huffman, value);
};

HPackData* HeaderBuffer::literalHeaderFieldWithIncrementalIndex(bool nameHuffman, char* name, bool valueHuffman, char* value){
	return new HPackData((uint8_t)64, nameHuffman, name, valueHuffman, value);
};

HPackData* HeaderBuffer::literalWithoutIndexing(uint32_t index, bool huffman, char* value){
	return new HPackData((uint8_t)0, index, huffman, value);
};

HPackData* HeaderBuffer::literalWithoutIndexing(bool nameHuffman, char* name, bool valueHuffman, char* value){
	return new HPackData((uint8_t)0,  nameHuffman, name, valueHuffman, value);
};

HPackData* HeaderBuffer::literalNeverIndexed(uint32_t index, bool huffman, char* value){
	return new HPackData((uint8_t)16, index, huffman, value);
};

HPackData* HeaderBuffer::literalNeverIndexed(bool nameHuffman, char* name, bool valueHuffman, char* value){
	return new HPackData((uint8_t)16, nameHuffman, name, valueHuffman, value);
};

HPackData* HeaderBuffer::dynamicTableSizeUpdate(uint32_t maxSize){
	return new HPackData(maxSize);
};


void HeaderBuffer::increaseFirst(uint32_t add){
	first = (first+add)%buf_size;
}

uint32_t HeaderBuffer::increasePointer(uint32_t pointer, uint32_t add){
	return (pointer+add)%buf_size;
}

HPackData* HeaderBuffer::getNext(){
	//Serial.println(F("----getNext"));
	
	if(buf_size<availableBufSize()){
		Serial.println(F("no more octets..."));
		return NULL;
	}

	byte first_octet = buf[first];
	//Serial.println(first_octet,BIN);
	if(first_octet&(byte)128){//indexedHeaderField
		//Serial.println(F("Decoding indexedHeaderField"));
		uint32_t index = decodeInteger(first,7);
		if(index==-1){
			//Serial.println(F("indexedHeaderField: error no next"));
			return NULL;
		}
		if(index ==0){
			//Serial.println(F("no indexedHeaderField: index 0"));
			return NULL;
		}
		uint32_t octets_length = getOctetsLength(index,7);
		increaseFirst(octets_length); //Check if actualize this here or not...
		return indexedHeaderField(index);
	}else{
		uint8_t preamble;
		uint8_t prefix;
		bool indexing = false;
		if(first_octet&(byte)64){//literalHeaderFieldWithIncrementalIndex
			//Serial.println(F("Decoding literalHeaderFieldWithIncrementalIndex"));
			preamble=(byte)64;
			prefix=6;	
			indexing=true;		
		}else if(first_octet&(byte)32){//dynamicTableSizeUpdate
			//Serial.println(F("Decoding dynamicTableSizeUpdate:"));
			uint32_t maxSize = decodeInteger(first,5);
				//Serial.print(F("new size"));
				//Serial.println(maxSize);
			if(maxSize==-1){
				Serial.println(F("dynamicTableSizeUpdate: error no next"));
				return NULL;
			}
			uint32_t octets_length = getOctetsLength(maxSize,5);
			first = (first+octets_length)%buf_size;

			this->dyn_table->resize(maxSize);

			//increaseFirst(octets_length); //Check if actualize this here or not...
			return dynamicTableSizeUpdate(maxSize);
		}else if(first_octet&(byte)16){//literalHeaderFieldWithoutIndexing
			//Serial.println(F("Decoding literalHeaderFieldNeverIndexed"));
			preamble=(byte)16;
			prefix=4;
		}else {//if(first_octet&(byte)0) literalHeaderFieldNeverIndexed
			//Serial.println(F("Decoding literalHeaderFieldWithoutIndexing"));
			preamble=(byte)0;
			prefix = 4;
		}

		uint32_t index = decodeInteger(first,prefix);
		if(index==-1){
			Serial.println(F("literalHeaderFieldWithIncrementalIndex: error no next"));
			return NULL;
		}
		uint32_t index_octets_length = getOctetsLength(index,prefix);
		uint32_t pointer = index_octets_length;
		//Serial.print(F("index_octets_length: "));
		//Serial.println(index_octets_length);

		
		uint32_t some_string_size = decodeInteger((first+pointer)%buf_size,7);
		//Serial.print(F("some string_size: "));
		//Serial.println(some_string_size);
		
		uint32_t some_string_size_octets_length = getOctetsLength(some_string_size,7);
		bool some_huffman = isHuffmanEncoded((first+pointer)%buf_size);
		//pointer += some_string_size_octets_length;
		char *some_string = decodeString((first+pointer)%buf_size, some_huffman, some_string_size+some_string_size_octets_length);

		//for(int k = 0; k< some_string_size; k++){
		//	Serial.println(buf[(first+pointer+k)%buf_size], BIN);
		//}
		HPackData * lhf;
		//Serial.print(F("some string: "));
		//Serial.println(some_string);
		//Serial.println(strlen(some_string));
		pointer += some_string_size+ some_string_size_octets_length;
		
		if(index==0){//->some_string==name
			uint32_t value_size = decodeInteger((first+pointer)%buf_size,7);
			uint32_t value_size_octets_length = getOctetsLength(value_size,7);
			bool value_huffman = isHuffmanEncoded((first+pointer)%buf_size);
			//pointer += value_size_octets_length;
			
			char *value_string = decodeString((first+pointer)%buf_size, value_huffman, value_size+ value_size_octets_length);
			//Serial.print(F("value string: "));
			//Serial.println(value_string);
			pointer += value_size+ value_size_octets_length;
			first = (first+pointer)%buf_size;//increaseFirst(pointer);
			lhf = literalHeaderField(preamble, some_huffman, some_string, value_huffman, value_string);
			delete[](value_string);
		
		}else{ //->some string==value
			first = (first+pointer)%buf_size;//increaseFirst(pointer);
			lhf = literalHeaderField(preamble, index, some_huffman, some_string);
			
			
		}
		HeaderPair* entry;
		if(indexing){
			//Serial.println(F("indexing..."));
			const char* add_value;
			const char* add_name;
			if(index!=0){
				//Serial.println(F("index!=0"));
				entry = findEntry(index);
				add_name = entry->name;
				if(entry->value==""){
					add_value = some_string;
				}
				else{
					add_value = entry->value;
				}
				//Serial.println(F("Add name and value"));
				//Serial.println(add_name);
				//Serial.println(add_value);	
			}
			else{
				//Serial.println(F("index==0"));
				add_name = lhf->get_name_string();//TODO strcpy
				add_value = lhf->get_value_string();//TODO strcpy ??
				//Serial.println(F("Add name and value"));
				//Serial.println(add_name);
				//Serial.println(add_value);				
			}


			char *hp_name = new char[strlen(add_name)+1];			
			//strcpy(hp_name, add_name);

			strncpy(hp_name, add_name, strlen(add_name)+1);
			char *hp_value = new char[strlen(add_value)+1];
			//strcpy(hp_value, add_value);
			strncpy(hp_value, add_value, strlen(add_value)+1);

			//Serial.println(F("hp name and value"));
			//Serial.println(hp_name);
			//Serial.println(hp_value);	
			
			HeaderPair* indexing_hp = new HeaderPair(hp_name, hp_value);

			

			//indexing_hp->toString();
			//Serial.println(F("Adding entry received..."));
			bool added_entry = dyn_table->addEntry(indexing_hp);
			//delete(indexing_hp);
		}
		//Serial.print(F("dyn_table->size() after adition"));
		//Serial.println(dyn_table->size());
		if(index==0){
			//Serial.println(F("TODO index==0 ..."));
		}else{
			if(indexing){
				//Serial.println(F("deleting entry index!=0"));
				delete(entry);
			}
			else{
				//Serial.println(F("TODO index!=0 ..."));
			}
		}
		delete[](some_string);
		return lhf;
	}	 
};

HeaderPair* HeaderBuffer::findEntry(uint32_t index){
	if(index>61){
		return dyn_table->findEntry(index);
	}else{
		//TODO
		//Serial.println(F("TODO: find static entry"));
		//TODO check size

		char* name_s = (char*)pgm_read_word(&(static_header_name_table[index-1]));

		char *name_buffer = new char[30];
		char *value_buffer= new char[30];
		strcpy_P(name_buffer, (char*)pgm_read_word(&(static_header_name_table[index-1])));//TODO check if +-1(?)
		strcpy_P(value_buffer, (char*)pgm_read_word(&(static_header_value_table[index-1])));//TODO check if +-1(?)
		//Serial.println(F("name found: "));
		//Serial.println(name_buffer);
		//Serial.println(F("value found: "));
		//Serial.println(value_buffer);
		return new HeaderPair(name_buffer, value_buffer);//static_table[index-1];

	}

};

HeaderPair* HeaderBuffer::getHeaderPair(HPackData* hpd){
	if(hpd->get_preamble()&(uint8_t)32){
		//Serial.println(F("preamble 32"));
		return NULL;
	}else{
		if(!(hpd->get_preamble()&(uint8_t)128)){
			if(hpd->get_index()!=(uint32_t)0){
				HeaderPair* name_hp = findEntry(hpd->get_index());
				char *name_cpy = new char[strlen(name_hp->name)+1];
				strncpy(name_cpy, name_hp->name, strlen(name_hp->name)+1);
				char *value_cpy = new char[strlen(hpd->get_value_string())+1];
				strncpy(value_cpy, hpd->get_value_string(), strlen(hpd->get_value_string())+1);
				//Serial.println(F("value cpy found: "));
				//Serial.println(value_cpy);
				delete(name_hp);
				return new HeaderPair(name_cpy, value_cpy);
			}else{
				char *name_cpy=new char[strlen(hpd->get_name_string())+1];
				strncpy(name_cpy, hpd->get_name_string(), strlen(hpd->get_name_string())+1);
				char *value_cpy=new char[strlen(hpd->get_value_string())+1];
				strncpy(value_cpy, hpd->get_value_string(), strlen(hpd->get_value_string())+1);
				
				return new HeaderPair(name_cpy,value_cpy);
			}
		}else{
			//Serial.println(F("getting hp...find entry..."));
			return findEntry(hpd->get_index());
		}
	}
};

HeaderPair * HPack::findEntry(uint32_t index){
	return hb->findEntry(index);
};

HPack::HPack(uint32_t max_header_buffer_size, uint32_t max_header_table_size){
	this->header_buffer_size = max_header_buffer_size;
	this->max_header_buffer_size = max_header_buffer_size;
	this->dyn_table_size = max_header_table_size;
	this->max_header_table_size = max_header_table_size;
	hb = new HeaderBuffer(this->header_buffer_size, this->max_header_table_size);
};


EncodedData* HPack::encode(char *name, bool nameHuffman, char* value, bool valueHuffman, bool indexing, bool neverIndexed){

	HPackData *hpd;
	EncodedData* ed;

	//search if name and value are already indexed
	uint32_t index = search_header(name, value);
	HeaderPair * hp = findEntry(index);

	if(index != (uint32_t)0){ //if index found
		if(strncmp(hp->value,value,strlen(value))) { //if name and value are indexed
			hpd =  hb->indexedHeaderField(index);
			
		}else{ //if only name is indexed
			if(indexing){
				hpd = hb->literalHeaderFieldWithIncrementalIndex(index, valueHuffman, value);
			}else{
				if(neverIndexed){//if neverIndexed
					hpd = hb->literalNeverIndexed(index, valueHuffman, value);
}else{
					hpd = hb->literalWithoutIndexing(index, valueHuffman, value);
	  			}
			}
		}
	}else{//if not indexed
		if(indexing){
			hpd = hb->literalHeaderFieldWithIncrementalIndex(nameHuffman, name, valueHuffman,value);
		}else{
			if(neverIndexed){
				hpd = hb->literalNeverIndexed(nameHuffman, name, valueHuffman,value);
			}else{
				hpd = hb->literalWithoutIndexing(nameHuffman, name, valueHuffman,value);
			}
		}
	}
	delete(hp); //???
	ed = hb->createHeaderBlock(&(HPackData*){hpd},1);
	delete(hpd);
  	return ed;
};

void HPack::decode(char* encoded_data, uint32_t encoded_size){
	hb->receiveHeaderBlock(encoded_data, encoded_size);
  //delete[](ed->encoded_data);
  //delete(ed);
  //hpd = hbr->getNext();
  //HeaderPair* hp1 = hbr->getHeaderPair(hpd);
	//TODO
};

HeaderPair* HPack::getNextHeaderPair(){
	//TODO
	HPackData * hpd = hb->getNext();
	HeaderPair* hp = hb->getHeaderPair(hpd);
	delete(hpd);
	return hp;
};


static void HPack::operator delete(void *ptr){
	//Serial.println(F("HPack delete"));
	HeaderBuffer::operator delete(((HPack*)ptr)->hb);
    ::operator delete(ptr);
};


uint32_t HPack::search_header(char* name, char* value){
	uint32_t dynamic_index = search_header_in_dyn_table(name,value);
	
	if(dynamic_index==0){
		return search_header_in_static_table(name, value);
	}
	return dynamic_index;
};

uint32_t HPack::search_header_in_static_table(char* name, char* value){
	return this->binary_search_static_table(name, value,0,60);
};


uint32_t HPack::binary_search_static_table(char*name, char*value, int start, int end){
	uint32_t middle = (start+end)/2;
	//Serial.print("searching binary tree at ");
	//Serial.println(start);
	//Serial.println(middle);
	//Serial.println(end);

	//char*found_name = (char*)pgm_read_dwor(d&(static_header_name_table[middle]));
	char found_name[30];
	strcpy_P(found_name, (char*)pgm_read_word(&(static_header_name_table[middle])));
	//this->hb->static_table
	//Serial.print("looking for ");
	//Serial.println((char*)name);
	//Serial.print("found ");
	//Serial.println((char*)found_name);

	//Serial.print("looking for size: ");
	//Serial.println(strlen((char*)name));


	while(strncmp((char*)found_name,name,strlen(name))==0 && (strlen((char*)name)==strlen((char*)found_name))){//} && found_name[sizeof(name)]=='\0'){
		//Serial.print("#Name Found!!");
		//look for value

		char found_value[30];
		strcpy_P(found_value, (char*)pgm_read_word(&(static_header_value_table[middle])));

		if(strlen(found_value)==0){
			//Serial.println("Value Empty");			
			return (uint32_t)(middle+1);
		}

		//Serial.print("looking for value ");
		//Serial.println((char*)value);
		//Serial.print("found ");
		//Serial.println((char*)found_value);

		//Serial.print("looking for value size: ");
		//Serial.println(strlen((char*)value));

		//Serial.print("found value size: ");
		//Serial.println(strlen((char*)found_value));


		int compare = strncmp((char*)found_value,value,strlen(value));
		if(compare==0 & strlen((char*)value)==strlen((char*)found_value)){
		//	Serial.println("####Value Found!!");
			return (uint32_t)(middle+1);//TODO: check for repeated names...(check value)
		}
		if(compare>0){
		//	Serial.print("looking for lower value ");
			end = middle;
			if(middle==0){
		//		Serial.print("#Name not Found :(");
				return (uint32_t)0;
			}
			middle=middle-1;


			strcpy_P(found_name, (char*)pgm_read_word(&(static_header_name_table[middle])));
		}else{
			//Serial.print("looking for greater value ");
			start = middle;
			if(middle==60){
				//Serial.print("#Name not Found :(");
				return (uint32_t)0;
			}
			middle=middle+1;
			strcpy_P(found_name, (char*)pgm_read_word(&(static_header_name_table[middle])));
		}
	}
	if(start==end){
		//Serial.print("#Name not Found :(");
		return (uint32_t)0;
	}
	
	if(strncmp((char*)found_name,name,strlen(name))>0){
		//Serial.println("looking in inferior half");
		if(middle==0){
		//	Serial.print("#Name not Found :(");
			return (uint32_t)0;
		}
		return binary_search_static_table(name, value, start, middle-1);
	}else{
		//Serial.println("looking in superior half");
		if(middle==60){
		//	Serial.print("#Name not Found :(");
			return (uint32_t)0;
		}
		return binary_search_static_table(name, value, middle+1, end);
	}

		 

		//static_header_name_table
		//static_header_value_table

};

uint32_t HPack::search_header_in_dyn_table(char* name, char* value){
	return this->hb->dyn_table->search_header(name, value);
};

uint32_t HPackDynamicTable::search_header(char* name, char* value){
	/*table
	first
	table_length
	length()//get amount of entries
	size()//get amount of bytes saved in the table
	*/
	int table_length = this->length();

	uint32_t index_empy_value = 0;
	for(int i = first,k=0; k<table_length; (i=i+1)%table_length,k++){
		//Serial.println("searching in dyn table");
		HeaderPair * hp = table[i];
		//hp->toString();
		//Serial.println(strlen(hp->name));
		//Serial.println(strlen(name));
		//Serial.println((char*)hp->name);
		//Serial.println((char*)name);

		//Serial.println(strlen(hp->value));
		//Serial.println(strlen(value));
		//Serial.println((char*)hp->value);
		//Serial.println((char*)value);
		if(strncmp(hp->name,name,(max(strlen(hp->name),strlen(name))))==0){ //comparing name
			if(strlen(hp->value)==0){
				index_empy_value = (table_length-1-i)+62;
			}
			if(strncmp(hp->value,value,(max(strlen(hp->value),strlen(value))))==0){//comparing value
				//Serial.println("Header pair found!!");
				return (table_length-1-i)+62;//TODO check i + 61??
			}
		}
	}
	//Serial.println("Header pair NOT found!!");
	return index_empy_value;
};