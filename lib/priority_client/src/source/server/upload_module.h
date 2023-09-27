#ifndef INCLUDED_UPLOAD
#define INCLUDED_UPLOAD

#include <iostream>
#include <stdio.h>
#include <string>

#include <ctime>
#include <sstream>
#include <iomanip>

#include "PriorityPacket.h"
#include "dbconfig.h"

#define LOGPATH "log.csv"
#define LOGPATH_MAN "log_man.csv"

using namespace std;

class UploadModule {

private:

public:
	void uploadpsql(unsigned int devid,string filepath,FileInfo info);

	UploadModule(void);
	~UploadModule(void);
};

# endif