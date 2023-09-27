#include "SV_Prioritydata.h"


Prioritydata::Prioritydata(void)
{
	for(int i=0;i<DEVICEID_MAX;i++){
		for(int j=0;j<DATAID_MAX;j++){
			id2index[i][j] = -1;	
		}
		isexistdevid[i] = 0;
	}
}


Prioritydata::~Prioritydata(void)
{

}

int Prioritydata::initdata(string path){
	ifstream ifs(path.c_str());
	string str;
	int ret = 0;

	svpridata_t tmppridata;

	while(getline(ifs,str))
	{
		string tmp;
		int tmpdevid;
	    istringstream stream(str);
	    (void) getline(stream,tmp,',');
	    tmpdevid = atoi(tmp.c_str());
	    if((0 < tmpdevid) && (tmpdevid < DEVICEID_MAX)){
	    	(void) getline(stream,tmp,',');
		    tmppridata.dataid = atoi(tmp.c_str());
		    (void) getline(stream,tmp,',');
		    tmppridata.dirpath = tmp;
		    (void) getline(stream,tmp,',');
		    tmppridata.priority = atoi(tmp.c_str());
		    pridatalist[tmpdevid].push_back(tmppridata);	
	    }
	}
	for(int i=0;i<DEVICEID_MAX;i++){
		if(0 < pridatalist[i].size()){
			ret += 1;
			isexistdevid[i] = 1;
			cout << "=================" << endl;
			cout << "deviveid = " << i << endl;
			for(int j=0;j<pridatalist[i].size();j++){
				id2index[i][pridatalist[i][j].dataid] = j;
				cout << "dataid = " << pridatalist[i][j].dataid << endl;
				cout << "dirpath = " << pridatalist[i][j].dirpath << endl;
				cout << "pri = " << pridatalist[i][j].priority << endl;
			}
		}
		
	}
	cout << "=================" << endl;

	if(ret == 0){
		return -1;
	}else{
		return ret;
	}
}

string Prioritydata::getDir(int deviceid,int dataid){
	return pridatalist[deviceid][id2index[deviceid][dataid]].dirpath;
}

int Prioritydata::getPriority(int deviceid,int dataid){
	return pridatalist[deviceid][id2index[deviceid][dataid]].priority;
}

int Prioritydata::getSize(int deviceid){
	if(deviceid < 1 || DEVICEID_MAX < deviceid){
		return 0;
	}
	return pridatalist[deviceid].size();
}

bool Prioritydata::isExistData(int deviceid,int dataid){
	if(deviceid < 1 || DEVICEID_MAX <= deviceid){
		//cout << "illigal deviceid" << endl;
		return 0;
	}
	if(dataid < 1 || DATAID_MAX <= dataid){
		//cout << "illigal dataid" << endl;
		return 0;
	}
	if(isexistdevid[deviceid] && (id2index[deviceid][dataid] >= 0)){
		return 1;
	}
	return 0;
}


bool Prioritydata::isExistDevice(int deviceid){
	if(deviceid < 1 || DEVICEID_MAX <= deviceid){
		//cout << "illigal deviceid" << endl;
		return 0;
	}
	return isexistdevid[deviceid];
}