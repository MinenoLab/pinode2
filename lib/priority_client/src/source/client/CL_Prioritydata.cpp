#include "CL_Prioritydata.h"

Prioritydata::Prioritydata(void)
{
	for(int i=0;i<DATAID_MAX;i++){
		pridatalist[i].dirpath = "nodata";
		pridatalist[i].priority = 0;
	}
}


Prioritydata::~Prioritydata(void)
{

}

int Prioritydata::initdata(string path){
	ifstream ifs(path.c_str());
	string str;
	int ret = 0;

	while(getline(ifs,str))
	{
		string tmp;
		int tmpdataid;
	    istringstream stream(str);
	    (void) getline(stream,tmp,',');
	    tmpdataid = atoi(tmp.c_str());
	    if((0 < tmpdataid) && (tmpdataid < DATAID_MAX)){
		    (void) getline(stream,tmp,',');
		    pridatalist[tmpdataid].dirpath = tmp;
		    (void) getline(stream,tmp,',');
		    pridatalist[tmpdataid].priority = atoi(tmp.c_str());
		    ret += 1;
	    }
	}

	pridatasize = 0;
	for(int i=1;i<DATAID_MAX;i++){
		if(pridatalist[i].priority != 0){
			pridatasize++;
			cout << "dataid = " << i << endl;
			cout << "dirpath = " << pridatalist[i].dirpath << endl;
			cout << "pri = " << pridatalist[i].priority << endl;
		}
	}
	cout << "=================" << endl;

	if(ret == 0){
		return -1;
	}else{
		return ret;
	}
}

string Prioritydata::getDir(int dataid){
	return pridatalist[dataid].dirpath;
}

int Prioritydata::getPriority(int dataid){
	//data not exist -> return 0
	return pridatalist[dataid].priority;
}

int Prioritydata::getSize(void){
	return pridatasize;
}

bool Prioritydata::isExistData(int dataid){
	if(pridatalist[dataid].priority != 0){
		return 1;
	}else{
		return 0;
	}
}
