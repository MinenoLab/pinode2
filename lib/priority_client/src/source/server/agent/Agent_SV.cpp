#include "Agent_SV.h"


Agent::Agent(void)
{
	use_connection = 0;
	use_external = 0;
}


Agent::~Agent(void)
{
	delete pridata;
	delete pripacket;
}

//unused
int Agent::setAgentOpt(int option,bool condition)
{
	switch(option){
        case FUNC_EXTERNAL:
        	use_external = condition;
        	break;
        case FUNC_CONNECTION:
        	use_connection = condition;
        	break;
        default:
        	return -1;
        	break;	
    }
    return 0;
}


int Agent::initAgent(string path){
	for(int i; i < DEVICEID_MAX; i++){
		pripacket->clearOrder(&NextOrder[i]);
	}

	pridata = new Prioritydata;
	pridata->initdata(path);
	pripacket = new PriorityPacket;
	if(use_connection){
		connectmodule = new ConnectionModule;
	}
}

OrderInfo Agent::getNextInfo(unsigned int devid){
	return NextOrder[devid];	
}

void Agent::updateNext(unsigned int devid,FileInfo fiinfo){
	OrderInfo lastOrder = NextOrder[devid];
	pripacket->clearOrder(&NextOrder[devid]);
	
	NextOrder[devid].id = 255;
	NextOrder[devid].update = 1;

	if(fiinfo.filesize > 0){
		NextOrder[devid].remove = 1;
	}
	
	
	if(use_external){

	}

	if(use_connection && (NextOrder[devid].id == 255)){
		NextOrder[devid].close = connectmodule->getConnectinfo(devid,lastOrder,fiinfo);
	}



}

bool Agent::getSavefile(unsigned int devid,FileInfo fiinfo,string * path){
	if(pridata->isExistData(devid,fiinfo.id)){
		*path = pridata->getDir(devid,fiinfo.id) + fiinfo.filename;
		return 1;
	}else{
		return 0;
	}
}

int Agent::getDeviceNum(void){
	int count = 0;
	for(int i=0;i<DEVICEID_MAX;i++){
		if(pridata->isExistDevice(i)){
			count++;
		}
	}

	return count;
}