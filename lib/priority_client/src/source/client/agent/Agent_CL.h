#ifndef INCLUDED_AGENT_CL
#define INCLUDED_AGENT_CL

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstdlib>


#include "PriorityPacket.h"
#include "CL_Prioritydata.h"
#include "Search.h"

#include "roundrobin_module.h"
#include "allsort_module.h"

#define FILO_RR 1
#define FIFO_RR 2
#define FILO 3
#define FIFO 4
#define SPTF 5

using namespace std;


class Agent {

private:
	int exist_bypri[DATAID_MAX];
	unsigned int sendid;
	string filepath;
	bool updatelist;
	int mode;
	int sortmode;
	int searchmode;

	FileInfo NextInfo;
	
	PriorityPacket * pripacket;
	Prioritydata * pridata;
	Search * search;

	DecisionRR * roundrobin;
	DecisionAS * allsearch;

public:
	int initAgent(string path);
	void updateExist(void);
	int updateNext(OrderInfo order);
	FileInfo getNextInfo(string * path);

	void setMode(int newmode);
	
	Agent(void);
	~Agent(void);
};

# endif