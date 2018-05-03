#include "HuffmanTree.h"
HuffmanTree::HuffmanTree(){
	//Serial.println("HuffmanTree");
		root = new InternalNode();
		//HuffmanSymbolCode* codedSymbol;
		for(uint16_t i =0; i<= 256; i++){//TODO fix size
			uint32_t coded_symbol = (uint32_t)pgm_read_dword_near(&(huffman_code_table[i]));
			uint8_t coded_length = (uint8_t)pgm_read_dword_near(&(huffman_length_table[i]));
			root->addChar(i, coded_symbol<<(32-coded_length), coded_length, coded_length);
			//addChar(&s_HuffmanSymbolCode[i]);
		}
	};

	HuffmanSymbolCode* HuffmanTree::search(char* encoded){
	//Serial.println(F("HuffmanTree::search"));
		return root->search(encoded);
	};

	HuffmanSymbolCode* HuffmanTree::searchBit(uint8_t* word, uint32_t length, uint32_t bit){
	//Serial.println("HuffmanTree::searchBit");
		return root->searchBit(word,length,bit);
	};

	HuffmanSymbolCode* HuffmanTree::addChar(uint16_t index, uint32_t code, uint8_t coded_length, int count){
	//Serial.println(F("HuffmanTree::addChar"));
		return root->addChar(index,code, coded_length, count);
	};



//Tree structure

Leaf::Leaf(uint16_t index, uint32_t code, uint8_t code_length){
		value = (HuffmanSymbolCode*)malloc(sizeof(HuffmanSymbolCode));
		value->symbol=index;
		value->hexCode=code;
		value->length=code_length;
	};

	HuffmanSymbolCode* Leaf::getValue(){
		return value;
	};

	HuffmanSymbolCode* Leaf::search(char* encoded){
	//Serial.println("Leaf::search");
		return value;
	};
	HuffmanSymbolCode* Leaf::searchBit(uint8_t* word, uint32_t length, uint32_t bit){
	//Serial.println("Leaf::searchBit");
		return value;
	};
HuffmanSymbolCode* Leaf::addChar(uint16_t index, uint32_t code, uint8_t code_length, int count){
	//Serial.println("Leaf::addChar");
		
		value = (HuffmanSymbolCode*)malloc(sizeof(HuffmanSymbolCode));
		value->symbol=index;
		value->hexCode=code;
		value->length=code_length;
		return value;
	};


	InternalNode::InternalNode(Node* l, Node* r){
	//Serial.println("InternalNode1");
		left = l;
		right = r;
	};
	InternalNode::InternalNode(){
	//Serial.println("InternalNode2");

	};



	Node* InternalNode::getLeft(){
		return this->left;
	};
	Node* InternalNode::getRight(){
		return this->right;
	};

	
	HuffmanSymbolCode* InternalNode::search(char* encoded){
	//Serial.println("InternalNode::search");
		if(encoded[0]=='\0'){
	//		Serial.println(F("encoded fin"));
			return NULL;
		}
		char* rest = encoded+1;
		if(encoded[0]=='0'){
			return left->search(rest);
		}else if(encoded[0]=='1'){
			return right->search(rest);
		}else{
			return NULL;
		}
	};

	HuffmanSymbolCode* InternalNode::searchBit(uint8_t* word, uint32_t length, uint32_t bit){
	//Serial.println("InternalNode::searchBit");
		uint32_t aux = bit/8;
		uint32_t aux_bit = bit%8;
		if(aux>=length){
			return NULL;
		}
		/*Serial.print(word[aux],BIN);
		Serial.print("\t|");
		Serial.print(128>>(aux_bit),BIN);
		Serial.print("\t|");
		Serial.println(word[aux] & 128>>(aux_bit));
		*/
		if(word[aux] & 128>>(aux_bit)){
			return right->searchBit(word,length,bit+1);
		}else{
			return left->searchBit(word,length,bit+1);
		}
	};

	HuffmanSymbolCode* InternalNode::addChar(uint16_t index, uint32_t code, uint8_t coded_length, int count){
	//Serial.println("InternalNode::addChar");
		/*Serial.print("Addimg char code: ");
		Serial.print(code);
		Serial.print(" count: ");
		Serial.print(count);
		Serial.print(" val: ");
		Serial.println(int(c));
		*/
		if(count>1){
			if(code & 0x80000000){//bit = 1
				//Serial.println("bit 1");
				//delay(1000);
				if(this->right==NULL){
					//Serial.println("new internal node");
					this->right = new InternalNode();
				}else{
					//Serial.println("old internal node");
					//Serial.println(int(right));
				}
				return this->right->addChar(index, code<<1, coded_length, count-1);
			}else{
				//Serial.println("bit 0");
				if(!this->left){
					//Serial.println("new internal node");
					this->left = new InternalNode();
				}else{
				//	Serial.println("old internal node");
				}
				return this->left->addChar(index, code<<1, coded_length, count-1);

			}
		}else{
			//Serial.println("Leaf");
			if(code & 0x80000000){//bit = 1
			//	Serial.println("bit 1");
				if(!this->right){
			//		Serial.println("new Leaf");
					this->right = new Leaf(index, code, coded_length);
					return ((Leaf*)(this->right))->getValue();
				}
				else{
	//				Serial.println(F("wtf right leaf already created??"));
					return NULL;
				}
			}else{
			//	Serial.println("bit 0");
				if(!this->left){
			//		Serial.println("new Leaf");
					this->left = new Leaf(index, code, coded_length);
					return ((Leaf*)(this->left))->getValue();
				}
				else{
	//				Serial.println(F("wtf left leaf already created??"));
					return NULL;
				}
			}
		}
		//return nullptr;
	};
