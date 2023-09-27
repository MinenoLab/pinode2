#ifndef INCLUDED_CONNECTION
#define INCLUDED_CONNECTION

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>

#include "PriorityPacket.h"
#include "SV_Prioritydata.h"

#define INIT_VALUE 0
#define MAX_COUNT 5


using namespace std;

class ConnectionModule {

private:
	int count_zero;
	bool connection[DEVICEID_MAX];
	

public:
	bool getConnectinfo(unsigned int devid,OrderInfo lastOrder,FileInfo fiinfo);


	ConnectionModule(void);
	~ConnectionModule(void);
};

# endif