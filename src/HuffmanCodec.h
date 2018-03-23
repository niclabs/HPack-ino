#include <Arduino.h>
#include "HuffmanTree.h"

class HuffmanEncodedWord{
public:
	uint8_t* enc_word;
	uint32_t length();
	HuffmanEncodedWord(HuffmanSymbolCode** sca, uint32_t size);
	static void operator delete(void *ptr);
	void toString();
	char* toBits();
private:
	uint32_t enc_size;//bits
	uint32_t enc_size_bytes;//bytes
};


class HuffmanCodec{
public:
	static HuffmanTree* tree;
	static char* decodeWord(char* encoded);
	static char* decodeBytes(uint8_t* word, uint32_t length);
	static HuffmanEncodedWord* encodeWord(char* word);
	static HuffmanSymbolCode* decodeByte(uint8_t*word, uint32_t length, uint32_t bit);
	

private:
	static HuffmanSymbolCode* decode(char* encoded);
	static HuffmanSymbolCode* encode(uint16_t c);

};
