#include "connection_module.h"

ConnectionModule::ConnectionModule(void)
{
	for(int i = 0; i < DEVICEID_MAX; i++){
		connection[i] = 0;
	}
	count_zero = 0;
}


ConnectionModule::~ConnectionModule(void)
{

}

bool ConnectionModule::getConnectinfo(unsigned int devid,OrderInfo lastOrder,FileInfo fiinfo){
	if((fiinfo.filesize == 0) && (!lastOrder.assign)){
		count_zero++;
		sleep(1);
		if(count_zero >= MAX_COUNT){
			count_zero = 0;
			return 1;
		}
	}
	return 0;
}


