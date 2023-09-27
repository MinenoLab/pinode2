#include "roundrobin_module.h"


DecisionRR::DecisionRR(void)
{
	random_device rnd;
	for (int i = 0; i < PRIORITY_MAX; i++)
	{
		preindex[i] = (unsigned int)rnd();
		preindex[i] = (preindex[i] % (DATAID_MAX - 1)) + 1;
	}
}

DecisionRR::~DecisionRR(void){

}

unsigned int DecisionRR::DecideID(int * exist_bypri){
	unsigned int index[PRIORITY_MAX];
	for (int i = 1; i <= PRIORITY_MAX; i++)
	{
		index[i-1] = preindex[i-1];
	}
	for(int i = 1;i <= PRIORITY_MAX;i++){
		for(int j = 0;j <= DATAID_MAX;j++){
			index[i-1] = (index[i-1] + 1) % (DATAID_MAX - 1);
			if(exist_bypri[index[i-1]] == i){
				preindex[i-1] = index[i-1];
				return index[i-1];
			}
		}
	}
	return 0;
}
