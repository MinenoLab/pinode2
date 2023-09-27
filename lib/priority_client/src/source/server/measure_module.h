#ifndef INCLUDED_MEASURE
#define INCLUDED_MEASURE

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iomanip>

#include <ctime>
#include <sstream>
#include <mutex>

#include "PriorityPacket.h"
#include "SV_Prioritydata.h"
#include "server_state.h"

#define LOGPATH "log.csv"
#define LOGPATH_MAN "log_man.csv"

using namespace std;

class MeasureModule {

private:
	bool outfile;
	ofstream log;

	std::mutex log_mutex_;
	double lastsec[DEVICEID_MAX];


	bool outfile_man;
	ofstream log_man;
	bool man_flag;

	std::mutex log_man_mutex_;
	double lastsec_man[DEVICEID_MAX];
	
	
	double get_sec(void);


public:
	void setOutFile(void);
	void setOutFile(string path);

	void setOutFile_man(void);
	void setOutFile_man(string path);

	void measureThroughput(unsigned int devid,int state,FileInfo fiinfo);
	void measureStart(unsigned int devid);
	void measureEnd(unsigned int devid,double datasize);

	MeasureModule(void);
	~MeasureModule(void);
};

# endif