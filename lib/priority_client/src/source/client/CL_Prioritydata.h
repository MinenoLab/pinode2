#ifndef INCLUDED_CLPRIORITYDATA
#define INCLUDED_CLPRIORITYDATA

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

#define DATAID_MAX 255
#define PRIORITY_MAX 4

using namespace std;

typedef struct CLPRIDATA
{
	string dirpath;
	int priority;
} clpridata_t;

class Prioritydata {

private:
	clpridata_t pridatalist[DATAID_MAX];
	int pridatasize;

public:
	int initdata(string path);

	string getDir(int dataid);
	int getPriority(int dataid);
	int getSize(void);
	bool isExistData(int dataid);

	Prioritydata(void);
	~Prioritydata(void);
};

# endif