#include "HuffmanCodec.h"


HuffmanTree* HuffmanCodec::tree = new HuffmanTree();


HuffmanEncodedWord::HuffmanEncodedWord(HuffmanSymbolCode** sca, uint32_t size){
	Serial.println(F("HuffmanEncodedWord"));
	Serial.print(F("size"));
		Serial.println(size);
		uint32_t aux_enc_size = (uint32_t)0;
		for(int i = 0; i< size; i++){
			aux_enc_size += (uint32_t)(((HuffmanSymbolCode*)sca[i])->length);
			Serial.print((char)(sca[i]->symbol));
			Serial.print(F("->"));
			Serial.print((sca[i]->symbol));
			Serial.print(F("->"));
			Serial.println(((HuffmanSymbolCode*)sca[i])->length);
		}
		Serial.print(F("bits size"));
		Serial.println(aux_enc_size);
		enc_size = aux_enc_size;//bits size
		aux_enc_size = (uint32_t)(aux_enc_size%8)==0?aux_enc_size/8:aux_enc_size/8+1;//byte size;
		enc_size_bytes = aux_enc_size;//bytes size
		Serial.print(F("encbytesSize"));
		Serial.println(enc_size_bytes);
		this->enc_word = new uint8_t[aux_enc_size];
		for(int asd=0; asd<aux_enc_size; asd++){//Init with 0
			this->enc_word[asd]=0;
		}
		int b = 0, j = 0;
		for(int i = 0; i< size; i++){
			HuffmanSymbolCode *sc = sca[i];
			uint32_t bits = sc->hexCode<<(32-sc->length);
			uint8_t aux_byte[] = {(uint8_t)(bits>>b>>24),(uint8_t)(bits>>b<<8>>24),(uint8_t)(bits>>b<<16>>24),(uint8_t)(bits>>b<<24>>24),(uint8_t)((bits<<(24+8-b))>>24)};

			for(int a=0; a<5; a++){
				if(aux_byte[a]!=0x0){
					this->enc_word[j+a] |= aux_byte[a];
				}
			}
			j = j+((b+sc->length)/8);
			b = (b+sc->length)%8;
		}
		//fill with EOS
		uint8_t padding = enc_size%8;
		if(padding!=(uint8_t)0){
			//TODO fix fill with 1's is ok??
			byte bf = this->enc_word[aux_enc_size-1];
			byte ab = 255;
			ab = (ab<<padding);
			ab = ab>>padding;
			this->enc_word[aux_enc_size-1] = bf|ab;
		}
	};

static void HuffmanEncodedWord::operator delete(void *ptr){
	Serial.println(F("HuffmanEncodedWord delete"));
	::operator delete[](((HuffmanEncodedWord*)ptr)->enc_word);	
    ::operator delete(ptr);
}


uint32_t HuffmanEncodedWord::length(){
	//Serial.println("HuffmanEncodedWord::length");
		return enc_size_bytes;
	}

void HuffmanEncodedWord::toString(){//TODO check this function
	Serial.println(F("HuffmanEncodedWord::toString"));
	int s = (enc_size%8==0?enc_size/8: enc_size/8+1);
	//char *st = new char[s+1];
	for(int i =0; i<s; i++){
		Serial.println((char)enc_word[i]);
	}
}


/*
	char* HuffmanCodec::decodeWord(char* encoded){
	//Serial.println("HuffmanCodec::decodeWord");
		//checkInit();
		//char str[];//max possible size
		char *str; 
   		str = new char[((strlen(encoded)*8)/5)];
   		Serial.print(F("InitialSize: "));
   		Serial.println((strlen(encoded)*8)/5);
		
		int i = 0;
		while(*encoded!=0){
			HuffmanSymbolCode* c = decode(encoded);
			i++;
			if(c==nullptr){
				break;
			}
			str[i] = (char)c->symbol;
			//Serial.println((char)c->symbol);
			encoded = encoded+(c->length);
			//Serial.print("encoded left: ");
			//Serial.println(encoded);
		}
		//str[i+1]= '\0';
		char *word = new char[i+1];
		for(int j=0; j<i;j++){
			word[j]=str[j];
		}
		word[i+1]='\0';
		delete[](str);
		//word = (char*)realloc(str, i+1);
		//if(word==0){
		//	Serial.println(F("realloc failed"));
		//	return str;
		//}
		return word;
	};*/

	char * HuffmanCodec::decodeBytes(uint8_t* word, uint32_t length){
	//Serial.println("HuffmanCodec::decodeBytes");
		//checkInit();
		
		uint32_t r = (length*8/5)+1;
		//Serial.print(F("reserved: "));
		//Serial.println(r);
		char *str = new char[r];
		uint32_t bit = 0;
		int i = 0;


		for(int j=0; j<length; j++){
			Serial.println(word[j],BIN);
		}
		Serial.print(F("decoding Bytes: "));
		while((bit/8)<length){
			HuffmanSymbolCode* c = decodeByte(word, length, bit);
			if(c==nullptr){
				Serial.println();
				break;
			}	
			*(str+i)=(char)c->symbol;
			Serial.print((char)c->symbol);
			bit +=c->length;
			i++;
		}
		Serial.println();
		char * new_str = new char[i+1];

		for(int j=0;j<=i;j++){
			new_str[j]=str[j];
		}
		delete[](str);
		Serial.println(new_str);
		Serial.print(F("length i "));
		Serial.println(i);
		return new_str;
	};

	HuffmanEncodedWord* HuffmanCodec::encodeWord(char* word){
	//Serial.println("HuffmanCodec::encodeWord");
		//checkInit();
		//HuffmanEncodedWord* ew = new HuffmanEncodedWord(word.length());
		HuffmanSymbolCode** sca =  new HuffmanSymbolCode*[strlen(word)];
		Serial.print(F("largo de la palabra: "));
		Serial.println(strlen(word));
		for(int i = 0; i<strlen(word); i++){

			HuffmanSymbolCode* sc = (HuffmanSymbolCode*)encode((uint16_t)word[i]);

			sca[i] = sc;
			Serial.print(word[i]);
			Serial.print(F(" encoded in symbol: "));
			Serial.println((byte)sca[i]->hexCode,BIN);
		//	ew[i] = sc;
		}
		HuffmanEncodedWord* hew = new HuffmanEncodedWord(sca,strlen(word));
		
		for(int i =0; i < strlen(word); i++){
			delete(sca[i]);
		}
		delete(sca);

		return hew;
	};

	
	HuffmanSymbolCode* HuffmanCodec::decodeByte(uint8_t* word, uint32_t length, uint32_t bit){
	//Serial.println("HuffmanCodec::decodeByte");
		//checkInit();
		if(bit/8>=length){
			Serial.println(F("nullptr"));
			return nullptr;
		}
		return tree->searchBit(word,length,bit);
	};
	HuffmanSymbolCode* HuffmanCodec::decode(char* encoded){
	//Serial.println("HuffmanCodec::decode");
		//checkInit();
		return tree->search(encoded);
	};

	HuffmanSymbolCode* HuffmanCodec::encode(uint16_t c){
	Serial.println(F("huffmanCodec::encode"));
		//checkInit();

		//Serial.println(c);
		uint32_t coded_symbol = (uint32_t)pgm_read_dword_near(&(huffman_code_table[c]));
		uint8_t coded_length = (uint8_t)pgm_read_dword_near(&(huffman_length_table[c]));
		HuffmanSymbolCode* encoded = (HuffmanSymbolCode*)malloc(sizeof(HuffmanSymbolCode));
		//HuffmanSymbolCode valueptr = {c, coded_symbol, coded_length};
		//encoded = (HuffmanSymbolCode*)&valueptr;
		encoded->symbol = c;
		encoded->hexCode = coded_symbol;
		encoded->length = coded_length;

		Serial.println(encoded->symbol);
		//Serial.println(encoded->symbol,BIN);
		Serial.println(encoded->hexCode);
		Serial.println(encoded->hexCode,BIN);
		Serial.println(encoded->length);
		return encoded;
	}

	/*void HuffmanCodec::init(){
			//root = new InternalNode();
			HuffmanSymbolCode* codedSymbol;
			for(int i =0; i<= 256; i++){//TODO fix size
				codedSymbol = &s_HuffmanSymbolCode[i];
				root->addChar(codedSymbol, codedSymbol->hexCode<<(32-codedSymbol->length), codedSymbol->length);
				//addChar(&s_HuffmanSymbolCode[i]);
			}
		
	};

	void HuffmanCodec::checkInit(){
		if(root->left==nullptr){
			init();
		}
	};
	*/






