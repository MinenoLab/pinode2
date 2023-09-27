#ifndef INCLUDED_PRIORITYPACKET
#define INCLUDED_PRIORITYPACKET

#include <iostream>
#include <string>
#include <cstring>
#include <stdint.h>
#include <cstdio>

#include <memory>
#include <bitset>
#include <stdexcept>

#include <cstddef>


#define MSG_ACK 255
#define MSG_ORDER 1
#define MSG_DEVID 17
#define MSG_META 18
#define MSG_FILE 19

#define FILEINFO_INITSTRING "nodata"
#define FILEINFO_INITTIMESTAMP "0000000000"


using namespace std;

typedef struct OrderInfo
{
	unsigned int id;
	bool order1;
	bool order2;
	bool order3;
	bool order4;
	bool remove;
	bool assign;
	bool close;
	bool update;
} OrderInfo;

typedef struct FileInfo
{
	unsigned int id;
	unsigned int filesize;
	string timestamp;
	string filename;
} FileInfo;


class PriorityPacket {

private:
	const unsigned int ORDER1_FLAG  = 0x0080; // 2進数 : (0000 0000 1000 0000)
	const unsigned int ORDER2_FLAG  = 0x0040; // 2進数 : (0000 0000 0100 0000)
	const unsigned int ORDER3_FLAG  = 0x0020; // 2進数 : (0000 0000 0010 0000)
	const unsigned int ORDER4_FLAG  = 0x0010; // 2進数 : (0000 0000 0001 0000)
	const unsigned int REMOVE_FLAG  = 0x0008; // 2進数 : (0000 0000 0000 1000)
	const unsigned int ASSIGN_FLAG  = 0x0004; // 2進数 : (0000 0000 0000 0100)
	const unsigned int CLOSE_FLAG   = 0x0002; // 2進数 : (0000 0000 0000 0010)
	const unsigned int UPDATE_FLAG  = 0x0001; // 2進数 : (0000 0000 0000 0001)


public:

	string makeOrder(OrderInfo info);
	size_t getOrder(OrderInfo info,unsigned char * buf);
	//unsigned int orderInterpretation(string packet,OrderInfo * info);
	unsigned int orderInterpretation(unsigned char * packet,OrderInfo * info);
	void clearOrder(OrderInfo * info);

	string makeMETAdata(FileInfo info);
	size_t getMETAdata(FileInfo info,unsigned char * buf);
	//unsigned int METAdataInterpretation(string packet,FileInfo * info);
	unsigned int METAdataInterpretation(unsigned char * packet,FileInfo * info);
	void initMETAdata(FileInfo * info);

	string makeFILEdata(unsigned int filesize,string data);
	size_t getFILEdataHeader(unsigned int datasize,unsigned char * buf);
	unsigned int FILEdataInterpretation(string packet,unsigned int * filesize);

	//string setDataType(unsigned int id,string data);
	size_t setDataType(unsigned int id,unsigned char * buf);
	//unsigned int getDataType(string packet);
	unsigned int getDataType(unsigned char * packet);


	void showOrderInfo(OrderInfo info);
	void showFileInfo(FileInfo info);
	void showOrderInfo(OrderInfo info,string header);
	void showFileInfo(FileInfo info,string header);

	PriorityPacket(void);
	~PriorityPacket(void);
};

# endif