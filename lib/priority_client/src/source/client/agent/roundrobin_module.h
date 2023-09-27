#ifndef INCLUDED_DECISIONRR
#define INCLUDED_DECISIONRR

#include <iostream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>

#include <random>

#include "CL_Prioritydata.h"
//#include "Search.h"


using namespace std;


class DecisionRR {

private:
	unsigned int preindex[PRIORITY_MAX];


public:
	unsigned int DecideID(int * exist_bypri);
	
	DecisionRR(void);
	~DecisionRR(void);
};

# endif