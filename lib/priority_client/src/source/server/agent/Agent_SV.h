#ifndef INCLUDED_AGENT_SV
#define INCLUDED_AGENT_SV

#include <iostream>
#include <stdio.h>
#include <string>

#include "PriorityPacket.h"
#include "SV_Prioritydata.h"

#include "connection_module.h"

#define INIT_VALUE 0

#define FUNC_EXTERNAL 255

#define FUNC_CONNECTION 100


using namespace std;

class Agent {

private:

	ConnectionModule * connectmodule;
	bool use_connection;

	bool use_external; //unused

	OrderInfo NextOrder[DEVICEID_MAX];

	Prioritydata * pridata;
	PriorityPacket * pripacket;
	



public:
	int setAgentOpt(int option,bool condition);
	int initAgent(string path);

	OrderInfo getNextInfo(unsigned int devid);
	void updateNext(unsigned int devid,FileInfo fiinfo);

	bool getSavefile(unsigned int devid,FileInfo fiinfo,string * path);

	int getDeviceNum(void);


	Agent(void);
	~Agent(void);
};

# endif