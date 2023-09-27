#ifndef INCLUDED_DECISIONAS
#define INCLUDED_DECISIONAS

#include <iostream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>

//#include <random>

#include "CL_Prioritydata.h"
#include "Search.h"

#define TIME_ASC 1
#define TIME_DESC 2
#define SIZE_ASC 3

using namespace std;


class DecisionAS {

private:
	Search * search;

	unsigned int DecideID_TIMEASC(int * exist_bypri);
  	unsigned int DecideID_TIMEDESC(int * exist_bypri);
    unsigned int DecideID_SIZEASC(int * exist_bypri);


public:
	void setSearch(Search * clsearch);
	unsigned int DecideID(int * exist_bypri,int mode);
	
	DecisionAS(void);
	~DecisionAS(void);
};

# endif