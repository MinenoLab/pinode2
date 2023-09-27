#ifndef INCLUDED_SEARCH
#define INCLUDED_SEARCH

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <cstdlib>

#include "PriorityPacket.h"
#include "CL_Prioritydata.h"

using namespace std;

#define NEWEST 1
#define OLDEST 2
#define SMALLEST 3



class Search {

private:
	string filepath[DATAID_MAX];

	FileInfo ret_fileinfo;

	PriorityPacket * pripacket = new PriorityPacket;

	bool isExistNewest(unsigned int id ,string path);
	bool isExistOldest(unsigned int id ,string path);
	bool isExistSmallest(unsigned int id ,string path);
public:
	
	bool isExist(unsigned int id ,string path,int mode);

	FileInfo getMETAdata(unsigned int id,string * path);
	unsigned int getTimestamp(unsigned int id);
	unsigned int getSize(unsigned int id);

	Search(void);
	~Search(void);
	string path2file(const string &path);
};

#endif