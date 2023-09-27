#include "allsort_module.h"


DecisionAS::DecisionAS(void)
{

}

DecisionAS::~DecisionAS(void){

}

void DecisionAS::setSearch(Search * clsearch){
	search = clsearch;
}

unsigned int DecisionAS::DecideID(int * exist_bypri,int mode){
	switch(mode){
    	case TIME_ASC:
    		return DecideID_TIMEASC(exist_bypri);
    	case TIME_DESC:
    		return DecideID_TIMEDESC(exist_bypri);
    	case SIZE_ASC:
    		return DecideID_SIZEASC(exist_bypri);
    }
}

unsigned int DecisionAS::DecideID_TIMEASC(int * exist_bypri){
	unsigned int ret = 0;
	unsigned int mintime;
	unsigned int tmptime;
	for(int i = 1;i <= PRIORITY_MAX;i++){
		mintime=4294967295;
		for(int j = 1;j < DATAID_MAX;j++){
			if(exist_bypri[j] == i){
				tmptime = search->getTimestamp(j);
				if(tmptime < mintime && tmptime != 0){
					ret = j;
					mintime = tmptime;
				}
			}
		}
		if(mintime != 4294967295){
			break;
		}
	}

	return ret;
}

unsigned int DecisionAS::DecideID_TIMEDESC(int * exist_bypri){
	unsigned int ret = 0;
	unsigned int maxtime;
	unsigned int tmptime;
	for(int i = 1;i <= PRIORITY_MAX;i++){
		maxtime=0;
		for(int j = 1;j < DATAID_MAX;j++){
			if(exist_bypri[j] == i){
				tmptime = search->getTimestamp(j);
				if(maxtime < tmptime && tmptime != 0){
					ret = j;
					maxtime = tmptime;
				}
			}
		}
		if(maxtime != 0){
			break;
		}
	}

	return ret;
}

unsigned int DecisionAS::DecideID_SIZEASC(int * exist_bypri){
	unsigned int ret = 0;
	unsigned int minsize;
	unsigned int tmpsize;
	for(int i = 1;i <= PRIORITY_MAX;i++){
		minsize=4294967295;
		for(int j = 1;j < DATAID_MAX;j++){
			if(exist_bypri[j] == i){
				tmpsize = search->getSize(j);
				if(tmpsize < minsize && tmpsize != 0){
					ret = j;
					minsize = tmpsize;
				}
			}
		}
		if(minsize != 4294967295){
			break;
		}
	}

	return ret;
}