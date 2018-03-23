#include <Arduino.h>



char* byteToBits(byte b);
uint32_t getOctetsLength(uint32_t I, uint8_t prefix);

class HeaderPair{
public:
	char* name;
	char* value;
	HeaderPair(char* n, char* v);
	void toString();
	HeaderPair* duplicateHeaderPair();
	uint32_t size();

	static void operator delete(void *ptr);
};

class HPackDynamicTable {
public:
	HPackDynamicTable(uint32_t maxSize);
	uint32_t size();//get amount of bytes saved in the table
	uint32_t length();//get amount of entries
	bool addEntry(HeaderPair* entry);
	HeaderPair* findEntry(uint32_t index);
	bool resize(uint32_t size);

	static void operator delete(void *ptr);
private:
	uint32_t first;
	uint32_t next;
	uint32_t max_size;
	uint32_t table_length;
	HeaderPair** table;
	const static uint32_t FIRST_INDEX = 62;
	void deleteEntry(uint32_t index);

};

class HPackData{
public:
	byte* encode();
	void toString();

	HPackData(uint8_t preamble, uint32_t index);
	HPackData(uint8_t preamble, uint32_t index, bool value_huffman, char* value_string);
	HPackData(uint8_t preamble, bool name_huffman, char* name_string, bool value_huffman, char* value_string);
	HPackData(uint32_t max_size);

	static void operator delete(void *ptr);
	byte* encodeInteger(uint32_t i, uint8_t prefix);
	byte* encodeString(char* s, bool huffman);
	

	uint8_t findPrefix(byte octet);

	//getters
	uint8_t get_preamble();
	uint32_t get_index();
	bool get_name_huffman();
	char* get_name_string();
	bool get_value_huffman();
	char* get_value_string();
	uint16_t get_max_size();

private: 
	uint8_t preamble;
	uint32_t index;
	bool name_huffman;
	char* name_string; 
	bool value_huffman;
	char* value_string;
	uint16_t max_size;

	const static uint32_t IndexedHeaderFieldPreamble = 128;
	const static uint32_t LiteralHeaderFieldWithIncrementalIndexingPreamble = 64;
	const static uint32_t LiteralHeaderFieldWithoutIndexingPreamble = 0;
	const static uint32_t LiteralHeaderFieldNeverIndexedPreamble = 16;
	const static uint32_t DinamicTableUpdatePreamble = 32;
};






class HeaderBuffer{
public:	
	HeaderBuffer(uint32_t incomming_buffer_max_size, uint32_t maxSize);

	static void operator delete(void *ptr);

	static HeaderPair* const static_table[];
	HPackDynamicTable * dyn_table;

	
	HeaderPair* getHeaderPair(HPackData* hpd);
	uint32_t availableBufSize();
	uint32_t addData(HPackData * data);
	
	HPackData* indexedHeaderField(uint32_t index);//indexed header

	HPackData* literalHeaderField(uint8_t preamble, uint32_t index, bool huffman, char* value);
	HPackData* literalHeaderField(uint8_t preamble, bool nameHuffman,char* name, bool valueHuffman, char* value);
	HPackData* literalHeaderFieldWithIncrementalIndex(uint32_t index, bool huffman, char* value);//indexed name
	HPackData* literalHeaderFieldWithIncrementalIndex(bool nameHuffman, char* name, bool valueHuffman, char* value);//new name
	HPackData* literalWithoutIndexing(uint32_t index, bool huffman, char* value);//indexed name
	HPackData* literalWithoutIndexing(bool nameHuffman, char* name, bool valueHuffman, char* value);//new name
	HPackData* literalNeverIndexed(uint32_t index, bool huffman, char* value);//indexed name
	HPackData* literalNeverIndexed(bool nameHuffman, char* name, bool valueHuffman, char* value);//new name
	HPackData* dynamicTableSizeUpdate(uint32_t maxSize);


	bool isHuffmanEncoded(uint32_t pointer);

	HPackData* getNext();



	char* toString();

	uint32_t decodeInteger(uint32_t pointer, uint8_t prefix);
	char* decodeString(uint32_t pointer, bool huffman, uint32_t size);
private:
	HeaderPair* findEntry(uint32_t index);
	uint32_t buf_size;
	byte *buf;// = new byte[1000];
	uint32_t first; 
	uint32_t next;
	


	void increaseFirst(uint32_t add);
	uint32_t increasePointer(uint32_t pointer, uint32_t add);
};




class HPack{
public:
	HPack(uint32_t max_header_buffer_size, uint32_t max_header_table_size);
	HeaderBuffer* hb;

	static void operator delete(void *ptr);
private:
	uint32_t header_buffer_size;
	uint32_t max_header_buffer_size;
	uint32_t dyn_table_size;
	uint32_t max_header_table_size;
};




//HeaderPairs in static table

const char name_0[] PROGMEM = ":authority";   
const char name_1[] PROGMEM = ":method";
const char name_2[] PROGMEM = ":method";
const char name_3[] PROGMEM = ":path";
const char name_4[] PROGMEM = ":path";
const char name_5[] PROGMEM = ":scheme";

const char name_6[] PROGMEM = ":scheme";
const char name_7[] PROGMEM = ":status";
const char name_8[] PROGMEM = ":status";
const char name_9[] PROGMEM = ":status";
const char name_10[] PROGMEM = ":status";

const char name_11[] PROGMEM = ":status";
const char name_12[] PROGMEM = ":status";
const char name_13[] PROGMEM = ":status";
const char name_14[] PROGMEM = "accept-charset";
const char name_15[] PROGMEM = "accept-encoding";

const char name_16[] PROGMEM = "accept-language";
const char name_17[] PROGMEM = "accept-ranges";
const char name_18[] PROGMEM = "accept";
const char name_19[] PROGMEM = "access-control-allow-origin";
const char name_20[] PROGMEM = "age";

const char name_21[] PROGMEM = "allow";
const char name_22[] PROGMEM = "authorization";
const char name_23[] PROGMEM = "cache-control";
const char name_24[] PROGMEM = "content-disposition";
const char name_25[] PROGMEM = "content-encoding";

const char name_26[] PROGMEM = "content-language";
const char name_27[] PROGMEM = "content-language";
const char name_28[] PROGMEM = "content-length";
const char name_29[] PROGMEM = "content-location";
const char name_30[] PROGMEM = "content-range";

const char name_31[] PROGMEM = "content-type";
const char name_32[] PROGMEM = "cookie";
const char name_33[] PROGMEM = "date";
const char name_34[] PROGMEM = "etag";
const char name_35[] PROGMEM = "expect";

const char name_36[] PROGMEM = "expires";
const char name_37[] PROGMEM = "from";
const char name_38[] PROGMEM = "host";
const char name_39[] PROGMEM = "if-match";
const char name_40[] PROGMEM = "if-modified-since";

const char name_41[] PROGMEM = "if-none-match";
const char name_42[] PROGMEM = "if-range";
const char name_43[] PROGMEM = "if-unmodified-since";
const char name_44[] PROGMEM = "last-modified";
const char name_45[] PROGMEM = "link";

const char name_46[] PROGMEM = "location";
const char name_47[] PROGMEM = "max-forwards";
const char name_48[] PROGMEM = "proxy-authenticate";
const char name_49[] PROGMEM = "proxy-authorization";
const char name_50[] PROGMEM = "range";

const char name_51[] PROGMEM = "referer";
const char name_52[] PROGMEM = "refresh";
const char name_53[] PROGMEM = "retry-after";
const char name_54[] PROGMEM = "server";
const char name_55[] PROGMEM = "set-cookie";

const char name_56[] PROGMEM = "strict-transport-security";
const char name_57[] PROGMEM = "transfer-encoding";
const char name_58[] PROGMEM = "user-agent";
const char name_59[] PROGMEM = "vary";
const char name_60[] PROGMEM = "via";

const char name_61[] PROGMEM = "www-authenticate";


const char value_0[] PROGMEM = "";   
const char value_1[] PROGMEM = "GET";
const char value_2[] PROGMEM = "POST";
const char value_3[] PROGMEM = "/";
const char value_4[] PROGMEM = "/index.html";
const char value_5[] PROGMEM = "http";

const char value_6[] PROGMEM = "https";
const char value_7[] PROGMEM = "200";
const char value_8[] PROGMEM = "204";
const char value_9[] PROGMEM = "206";
const char value_10[] PROGMEM = "304";

const char value_11[] PROGMEM = "400";
const char value_12[] PROGMEM = "404";
const char value_13[] PROGMEM = "500";
const char value_14[] PROGMEM = "";
const char value_15[] PROGMEM = "gzip, deflate";

const char value_16[] PROGMEM = "";
const char value_17[] PROGMEM = "";
const char value_18[] PROGMEM = "";
const char value_19[] PROGMEM = "";
const char value_20[] PROGMEM = "";

const char value_21[] PROGMEM = "";
const char value_22[] PROGMEM = "";
const char value_23[] PROGMEM = "";
const char value_24[] PROGMEM = "";
const char value_25[] PROGMEM = "";

const char value_26[] PROGMEM = "";
const char value_27[] PROGMEM = "";
const char value_28[] PROGMEM = "";
const char value_29[] PROGMEM = "";
const char value_30[] PROGMEM = "";

const char value_31[] PROGMEM = "";
const char value_32[] PROGMEM = "";
const char value_33[] PROGMEM = "";
const char value_34[] PROGMEM = "";
const char value_35[] PROGMEM = "";

const char value_36[] PROGMEM = "";
const char value_37[] PROGMEM = "";
const char value_38[] PROGMEM = "";
const char value_39[] PROGMEM = "";
const char value_40[] PROGMEM = "";

const char value_41[] PROGMEM = "";
const char value_42[] PROGMEM = "";
const char value_43[] PROGMEM = "";
const char value_44[] PROGMEM = "";
const char value_45[] PROGMEM = "";

const char value_46[] PROGMEM = "";
const char value_47[] PROGMEM = "";
const char value_48[] PROGMEM = "";
const char value_49[] PROGMEM = "";
const char value_50[] PROGMEM = "";

const char value_51[] PROGMEM = "";
const char value_52[] PROGMEM = "";
const char value_53[] PROGMEM = "";
const char value_54[] PROGMEM = "";
const char value_55[] PROGMEM = "";

const char value_56[] PROGMEM = "";
const char value_57[] PROGMEM = "";
const char value_58[] PROGMEM = "";
const char value_59[] PROGMEM = "";
const char value_60[] PROGMEM = "";

const char value_61[] PROGMEM = "";

// Then set up a table to refer to your strings.

const char* const static_header_name_table[] PROGMEM = {name_0, name_1, name_2, name_3, name_4, name_5, name_6, name_7, name_8, name_9,name_10, name_11, name_12, name_13, name_14, name_15, name_16, name_17, name_18, name_19, name_20, name_21, name_22, name_23, name_24, name_25, name_26, name_27, name_28, name_29, name_30, name_31, name_32, name_33, name_34, name_35, name_36, name_37, name_38, name_39, name_40, name_41, name_42, name_43, name_44, name_45, name_46, name_47, name_48, name_49, name_50, name_51, name_52, name_53, name_54, name_55, name_56, name_57, name_58, name_59, name_60, name_61};
const char* const static_header_value_table[] PROGMEM = {value_0, value_1, value_2, value_3, value_4, value_5, value_6, value_7, value_8, value_9,value_10, value_11, value_12, value_13, value_14, value_15, value_16, value_17, value_18, value_19, value_20, value_21, value_22, value_23, value_24, value_25, value_26, value_27, value_28, value_29, value_30, value_31, value_32, value_33, value_34, value_35, value_36, value_37, value_38, value_39, value_40, value_41, value_42, value_43, value_44, value_45, value_46, value_47, value_48, value_49, value_50, value_51, value_52, value_53, value_54, value_55, value_56, value_57, value_58, value_59, value_60, value_61};


