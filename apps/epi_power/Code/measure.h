

#ifndef _MEASURE_LIBRARY_
#define _MEASURE_LIBRARY_

#include "EPI_Config.h"
//#include "gap_common.h"
#include "pmsis.h"

/* Data Struct */
typedef struct _Timer_Data {
	char where;
	//int time;
	int cycle;
} Timer_Data_t;

void measureInitializeNStart ( void );
extern inline uint32_t measureReadCycle ( void );

extern int lPerformanceCheck;
extern int CSCheck;
extern int Timerindex;
#ifdef __PULP__
extern Timer_Data_t timerBuffer[15*MEASURE_N_ITERATION+7+2000];
#else
extern Timer_Data_t timerBuffer[15*MEASURE_N_ITERATION+7];
#endif

//#define measureReadCycle

#endif
