#include "PriorityPacket.h"

PriorityPacket::PriorityPacket(void)
{

}


PriorityPacket::~PriorityPacket(void)
{

}

string PriorityPacket::makeOrder(OrderInfo info){
	string orderpacket;
	uint32_t datatype = (uint32_t) MSG_ORDER;
	uint32_t dataid = (uint32_t) info.id;
	uint32_t order;
	unsigned char tmp_char;

	orderpacket.clear();

	//cout << "receve id = " << dataid << endl;

	tmp_char = (unsigned char) (datatype % 256);
	orderpacket += tmp_char;
	tmp_char = (unsigned char) (dataid % 256);
	orderpacket += tmp_char;

	order = ORDER1_FLAG*info.order1 + ORDER2_FLAG*info.order2 
  			+ ORDER3_FLAG*info.order3 + ORDER4_FLAG*info.order4 
  			+ REMOVE_FLAG*info.remove + ASSIGN_FLAG*info.assign
  			+ CLOSE_FLAG*info.close + UPDATE_FLAG*info.update;
	tmp_char = (unsigned char) (order % 256);
	orderpacket += tmp_char;

	return orderpacket;
}

size_t PriorityPacket::getOrder(OrderInfo info,unsigned char * buf){
	//string orderpacket;
	uint32_t datatype = (uint32_t) MSG_ORDER;
	uint32_t dataid = (uint32_t) info.id;
	uint32_t order;
	order = ORDER1_FLAG*info.order1 + ORDER2_FLAG*info.order2 
  			+ ORDER3_FLAG*info.order3 + ORDER4_FLAG*info.order4 
  			+ REMOVE_FLAG*info.remove + ASSIGN_FLAG*info.assign
  			+ CLOSE_FLAG*info.close + UPDATE_FLAG*info.update;
	//unsigned char tmp_char[1];

	//orderpacket.clear();

	//cout << "receve id = " << dataid << endl;

	/*cout << "input before = " << bitset<8>(buf[0]) << "|" << bitset<8>(buf[1]) << "|" 
		 << bitset<8>(buf[2]) << endl;*/
	buf[0] = (unsigned char) (datatype % 256);
	buf[1] = (unsigned char) (dataid % 256);
	buf[2] = (unsigned char) (order % 256);

	//cout << "recvbuf = " << bitset<8>(recvbuf[0]) << "|" << bitset<8>(recvbuf[1]) << endl;
    
    /*cout << "input after = " << bitset<8>(buf[0]) << "|" << bitset<8>(buf[1]) << "|" 
		 << bitset<8>(buf[2]) << endl;*/

	return sizeof(unsigned char)*3;
	/*orderpacket += tmp_char;
	tmp_char = (unsigned char) (dataid % 256);
	orderpacket += tmp_char;

	order = ORDER1_FLAG*info.order1 + ORDER2_FLAG*info.order2 
  			+ ORDER3_FLAG*info.order3 + ORDER4_FLAG*info.order4 
  			+ REMOVE_FLAG*info.remove + ASSIGN_FLAG*info.assign
  			+ CLOSE_FLAG*info.close + UPDATE_FLAG*info.update;
	tmp_char = (unsigned char) (order % 256);
	orderpacket += tmp_char;

	return orderpacket;*/
}

unsigned int PriorityPacket::orderInterpretation(unsigned char * packet, OrderInfo * info){
	unsigned int datatype;
	unsigned int order;

	clearOrder(info);

	/*
	datatype = (unsigned int)(unsigned char)packet[0];
	info->id = (unsigned int)(unsigned char)packet[1];
	order = (unsigned int)(unsigned char)packet[2];
	*/
	datatype = (unsigned int)packet[0];
	info->id = (unsigned int)packet[1];
	order = (unsigned int)packet[2];
	
	if( (order & ORDER1_FLAG) != 0 ){info->order1 = 1;}
	if( (order & ORDER2_FLAG) != 0 ){info->order2 = 1;}
	if( (order & ORDER3_FLAG) != 0 ){info->order3 = 1;}
	if( (order & ORDER4_FLAG) != 0 ){info->order4 = 1;}
	if( (order & REMOVE_FLAG) != 0 ){info->remove = 1;}
	
	if( (order & ASSIGN_FLAG) != 0 ){info->assign = 1;}
	if( (order & CLOSE_FLAG) != 0 ){info->close = 1;}
	if( (order & UPDATE_FLAG) != 0 ){info->update = 1;}


	return datatype;
}

void PriorityPacket::clearOrder(OrderInfo * info){
	info->id = 0;
	info->order1 = 0;
	info->order2 = 0;
	info->order3 = 0;
	info->order4 = 0;
	info->remove = 0;
	info->assign = 0;
	info->close  = 0;
	info->update = 0;
}


string PriorityPacket::makeMETAdata(FileInfo info){
	string metadata_packet;
	uint32_t datatype = (uint32_t) MSG_META;
	uint32_t dataid = (uint32_t) info.id;
	uint32_t filesize = (uint32_t) info.filesize;
	uint32_t namesize = (uint32_t) info.filename.size();
	unsigned char tmp_char;


	tmp_char = (unsigned char) (datatype % 256);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (dataid % 256);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 24);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 16);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 8);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize % 256);
	metadata_packet += tmp_char;
	metadata_packet += info.timestamp;
	tmp_char = (unsigned char) (namesize % 256);
	metadata_packet += tmp_char;
	metadata_packet += info.filename;
	

	return metadata_packet;
}

size_t PriorityPacket::getMETAdata(FileInfo info,unsigned char * buf){
	string metadata_packet;
	uint32_t datatype = (uint32_t) MSG_META;
	uint32_t dataid = (uint32_t) info.id;
	uint32_t filesize = (uint32_t) info.filesize;
	uint32_t namesize = (uint32_t) info.filename.size();
	//unsigned char tmp_char;


	buf[0] = (unsigned char) (datatype % 256);
	buf[1] = (unsigned char) (dataid % 256);
	buf[2] = (unsigned char) (filesize >> 24);
	buf[3] = (unsigned char) (filesize >> 16);
	buf[4] = (unsigned char) (filesize >> 8);
	buf[5] = (unsigned char) (filesize % 256);

	
	memcpy(buf+6, info.timestamp.c_str(), 10);

	/*cout << "datatype = " << datatype << endl;
	cout << "datatype(field) = " << bitset<8>(buf[0]) << endl;
	cout << "dataid = " << datatype << endl;
	cout << "datatype(field) = " << bitset<8>(buf[1]) << endl;
	cout << "filesize = " << filesize << endl;
	cout << "filesize(field) = " << bitset<8>(buf[2]) << "|" << bitset<8>(buf[3]) << "|" 
		 << bitset<8>(buf[4]) << "|" << bitset<8>(buf[5]) << endl;
	cout << "filesize(field) = " << bitset<8>(buf[2]) << "" << bitset<8>(buf[3]) << "" 
		 << bitset<8>(buf[4]) << "" << bitset<8>(buf[5]) << endl;*/

	//cout << "timestamp:" << info.timestamp << endl;
	/*cout << "timestamp:" << info.timestamp.c_str() << endl;
	cout << "timestamp:" << buf+6 << endl;*/

	buf[16] = (unsigned char) (namesize % 256);
	memcpy(buf+17, info.filename.c_str(), namesize);

	/*cout << "filename:" << info.filename << endl;
	cout << "filename:" << buf+17 << endl;*/

	return 17+namesize;
	
	/*cout << "filesize(uint) = " << sizeof(filesize) << endl;
	cout << "buf.data after  = " << bitset<32>(filesize) << endl;
	cout << "filesize(uchar)= " << sizeof((unsigned char) filesize) << endl;
	cout << "buf.data after  = " << bitset<32>((unsigned char) filesize) << endl;
	cout << "namesize = " << namesize << endl;*/

	/*tmp_char = (unsigned char) (datatype % 256);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (dataid % 256);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 24);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 16);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 8);
	metadata_packet += tmp_char;
	tmp_char = (unsigned char) (filesize % 256);
	metadata_packet += tmp_char;
	metadata_packet += info.timestamp;
	tmp_char = (unsigned char) (namesize % 256);
	metadata_packet += tmp_char;
	metadata_packet += info.filename;
	return 1;
	*/
}


unsigned int PriorityPacket::METAdataInterpretation(unsigned char * packet,FileInfo * info){
	unsigned int datatype;
	unsigned int namesize;

	datatype = (unsigned int)packet[0];
	info->id = (unsigned int)packet[1];
	info->filesize = 16777216*(unsigned int)packet[2] + 65536*(unsigned int)packet[3]
						+ 256*(unsigned int)packet[4] + (unsigned int)packet[5];

	info->timestamp.clear();
	
	for(int i=0;i<10;i++){
    	info->timestamp += packet[6+i];
  	}
	

	namesize = (unsigned int)packet[16];
	info->filename.clear();
	for(int i=0;i<(unsigned int)packet[16];i++){
    	info->filename += packet[17+i];
  	}

	return datatype;
}

void PriorityPacket::initMETAdata(FileInfo * info){
	info->id = 0;
	info->filesize = 0;
	info->timestamp = FILEINFO_INITTIMESTAMP;
	info->filename = FILEINFO_INITSTRING;
}

string PriorityPacket::makeFILEdata(unsigned int filesize,string data){
	string packet;
	uint32_t datatype = (uint32_t) MSG_FILE;
	uint32_t tmp_filesize = (uint16_t) filesize;
	unsigned char tmp_char;

	tmp_char = (unsigned char) (datatype % 256);
	packet += tmp_char;
	tmp_char = (unsigned char) (filesize >> 8);
	packet += tmp_char;
	tmp_char = (unsigned char) (filesize % 256);
	packet += tmp_char;
	packet += data;

	
	return packet;
}

size_t PriorityPacket::getFILEdataHeader(unsigned int datasize,unsigned char * buf){
	string packet;
	uint32_t datatype = (uint32_t) MSG_FILE;
	//uint32_t datasize = (uint16_t) datasize;
	unsigned char tmp_char;

	buf[0] = (unsigned char) (datatype % 256);
	buf[1] = (unsigned char) ((uint32_t)datasize >> 8);
	buf[2] = (unsigned char) ((uint32_t)datasize % 256);
	

	return 3;
}


unsigned int PriorityPacket::FILEdataInterpretation(string packet,unsigned int * filesize){
	unsigned int datatype;
	
	datatype = (unsigned int)(unsigned char)packet[0];
	*filesize = 256*(unsigned int)(unsigned char)packet[1] + (unsigned int)(unsigned char)packet[2];


	return datatype;
}

size_t setDataType(unsigned int id,unsigned char * buf){
	buf[0] = (unsigned char)id;

	return 1;
}

unsigned int PriorityPacket::getDataType(unsigned char * packet){
	unsigned int datatype;
	datatype = (unsigned int)packet[0];

	if(datatype != MSG_ACK && datatype != MSG_ORDER && 
		datatype != MSG_DEVID && datatype != MSG_META && 
		datatype != MSG_FILE){
		return -1;
	}
	return datatype;
}


void PriorityPacket::showOrderInfo(OrderInfo info){
	cout << "id = " << info.id << endl;
	cout << "order1 = " << info.order1 << endl;
	cout << "order2 = " << info.order2 << endl;
	cout << "order3 = " << info.order3 << endl;
	cout << "order4 = " << info.order4 << endl;
	cout << "remove = " << info.remove << endl;
	cout << "assign = " << info.assign << endl;
	cout << "close = " << info.close << endl;
	cout << "update = " << info.update << endl;
}

void PriorityPacket::showFileInfo(FileInfo info){
	cout << "id = " << info.id << endl;
	cout << "filesize = " << info.filesize << endl;
	cout << "timestamp = " << info.timestamp << endl;
	cout << "filename = " << info.filename << endl;
}

void PriorityPacket::showOrderInfo(OrderInfo info,string header){
	cout << header << "id = " << info.id << endl;
	cout << header << "order1 = " << info.order1 << endl;
	cout << header << "order2 = " << info.order2 << endl;
	cout << header << "order3 = " << info.order3 << endl;
	cout << header << "order4 = " << info.order4 << endl;
	cout << header << "remove = " << info.remove << endl;
	cout << header << "assign = " << info.assign << endl;
	cout << header << "close = " << info.close << endl;
	cout << header << "update = " << info.update << endl;

}

void PriorityPacket::showFileInfo(FileInfo info,string header){
	cout << header << "id = " << info.id << endl;
	cout << header << "filesize = " << info.filesize << endl;
	cout << header << "timestamp = " << info.timestamp << endl;
	cout << header << "filename = " << info.filename << endl;
}