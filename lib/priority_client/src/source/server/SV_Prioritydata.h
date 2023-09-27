#ifndef INCLUDED_SVPRIORITYDATA
#define INCLUDED_SVPRIORITYDATA

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>

#define DEVICEID_MAX 255
#define DATAID_MAX 255
#define PRIORITY_MAX 4

using namespace std;


typedef struct SVPRIDATA
{
	int dataid;
	string dirpath;
	int priority;
} svpridata_t;

class Prioritydata {

private:
	vector<svpridata_t> pridatalist[DEVICEID_MAX];
	int id2index[DEVICEID_MAX][DATAID_MAX];
	bool isexistdevid[DEVICEID_MAX];

public:
	int initdata(string path);

	int getDataID(int deviceid,int dataid);
	string getDir(int deviceid,int dataid);
	int getPriority(int deviceid,int dataid);
	int getSize(int deviceid);
	bool isExistData(int deviceid,int dataid);
	bool isExistDevice(int deviceid);

	Prioritydata(void);
	~Prioritydata(void);
};

# endif