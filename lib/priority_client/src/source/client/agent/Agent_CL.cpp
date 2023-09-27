#include "Agent_CL.h"

Agent::Agent(void)
{

}

Agent::~Agent(void){
	delete pripacket;
	delete pridata;
	delete search;
}

int Agent::initAgent(string path){
	updatelist = 0;

	pripacket->initMETAdata(&NextInfo);

	pridata = new Prioritydata;
	pridata->initdata(path);
	pripacket = new PriorityPacket;
	search = new Search;
	for(int i = 0; i< DATAID_MAX; i++){
		exist_bypri[i] = 0;
	}
	roundrobin = new DecisionRR;
	allsearch = new DecisionAS;
	allsearch->setSearch(search);

	mode = FILO_RR;
	searchmode = NEWEST;
	sortmode = 0;
}

FileInfo Agent::getNextInfo(string * path){
	//cout << "filepath in agent : " << filepath << endl;
	*path = filepath;
	return NextInfo;
}

void Agent::updateExist(void){
	for(int i = 1; i < DATAID_MAX; i++){
		if(pridata->isExistData(i)){
			exist_bypri[i] = (int)search->isExist(i,pridata->getDir(i),searchmode) * pridata->getPriority(i);	
		}else{
			exist_bypri[i] = 0;
		}
	}
	updatelist = 1;
}


int Agent::updateNext(OrderInfo order){
	if(order.update){
		if(order.remove){
			string cmd = "rm -f " + filepath + " 2>/dev/null";
			cout << "cmd = " << cmd << endl;
			system(cmd.c_str());

			if(pridata->isExistData(NextInfo.id)){
				exist_bypri[NextInfo.id] = (int)search->isExist(NextInfo.id,pridata->getDir(NextInfo.id),searchmode) * pridata->getPriority(NextInfo.id);	
			}else{
				exist_bypri[NextInfo.id] = 0;
			}
		}

		pripacket->initMETAdata(&NextInfo);
		if(order.assign){
			sendid = order.id;
		}else{
			//update existlist
			if(!updatelist){
				for(int i = 1; i < DATAID_MAX; i++){
					if(pridata->isExistData(i)){
						exist_bypri[i] = (int)search->isExist(i,pridata->getDir(i),searchmode) * pridata->getPriority(i);	
					}else{
						exist_bypri[i] = 0;
					}
				}
			}
			
			//detect sendid
			if(sortmode == 0){
				sendid = roundrobin->DecideID(exist_bypri);
			}else{
				sendid = allsearch->DecideID(exist_bypri,sortmode);
			}

			updatelist = 0;
		}
		

		if(sendid == 0){
			return 0;
		}else{
			NextInfo = search->getMETAdata(sendid,&filepath);
			return sendid;
		}
	}

}

void Agent::setMode(int newmode){
	mode = newmode;
	updatelist = 0;
	switch(mode){
    	case FILO_RR:
    		searchmode = NEWEST;
    		sortmode = 0;
    		break;
		case FIFO_RR:
			searchmode = OLDEST;
    		sortmode = 0;
			break;
		case FILO:
			searchmode = NEWEST;
    		sortmode = TIME_DESC;
			break;
		case FIFO:
			searchmode = OLDEST;
    		sortmode = TIME_ASC;
			break;
		case SPTF:
			searchmode = SMALLEST;
			sortmode = SIZE_ASC;
			break;
	}
}
