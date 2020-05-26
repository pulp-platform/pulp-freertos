

#ifndef _ROBERT_H_
#define _ROBERT_H_


#include "EPI_Config.h"
#include "EPI_types.h"

/**** CHANGE TO CONFIGURE ****/

/** ----REMEMBER!!!: TO FULLY ENABLE THIS you need to COMMENT "SPI_TEST" and "MEASURE_ACTIVE" and UNcomment "ROBERT_TEST" inside EPI_Config.h. Thanks ---- **/

#define ROBERT_AUTO_RUN             //if enabled the firmware will keep going, if commented the firmware should stop after all output matrixes are filled.
#define ROBERT_FREQ_COMPARE         //if enabled it will also directly compare the freq.

#define ROBERT_NUMBER_OF_COMMANDS               4			//Number of different input commands
#define ROBERT_UI_STEPS                         750			//Number of Steps (=activation of TASKOS) before changing commands

/** OTHERS NOT TO BE CHANGED **/
#define ROBERT_PERIODIC_TOT_STEPS               (ROBERT_NUMBER_OF_COMMANDS * ROBERT_UI_STEPS * TASKS_OS_PERIODICITY_MUL_FACTOR)

extern int ROBERT_uiIndex;
extern int ROBERT_uiCounter;
extern int ROBERT_periodicCounter;



/************ DATA WILL BE OUTPUTTED HERE **************/

numValue ROBERT_uiFreqRedMap[ROBERT_NUMBER_OF_COMMANDS * ROBERT_UI_STEPS];
numValue Robert_uiErrorMap[ROBERT_NUMBER_OF_COMMANDS * ROBERT_UI_STEPS];

numValue ROBERT_computedFreq[ROBERT_PERIODIC_TOT_STEPS][N_EPI_CORE];
//actually these values above are a bit different because of approximation
//in fact to pass the data thorugh the spi I pass it as a short unsigned int
//so I make at the 4th decimal. BTW in the new implementation this should not
//be a problem since I stepped the freq with 100Mhz steps---

#ifdef ROBERT_FREQ_COMPARE
numValue ROBERT_freqDiff[ROBERT_PERIODIC_TOT_STEPS][N_EPI_CORE] = {{0}};
#endif

/*************** INSERT HERE THE DATA *****************/

//The temperature measured by the sensor and received by the fimware. One temp for each core for each step
numValue ROBERT_measuredTemp[ROBERT_PERIODIC_TOT_STEPS][N_EPI_CORE] = {{ }}; //Insert here!

//The taget frequency per core (input) for each different input command
numValue ROBERT_uiTargetFreq[ROBERT_NUMBER_OF_COMMANDS][N_EPI_CORE] = {{ }}; //Insert here!

//The target workload per core (input) for each different input command
numValue ROBERT_uiWorkload[ROBERT_NUMBER_OF_COMMANDS][N_EPI_CORE] = {{ }}; //Insert here!

//The target power budget (unique, not per core) for each different input command
numValue ROBERT_uiPowerBudget[ROBERT_NUMBER_OF_COMMANDS]  = { }; //Insert here!

//The actual recorded total power consumed by the model - If you do not have the data just put 0, it should be fine for now
numValue ROBERT_uiSimPower[ROBERT_NUMBER_OF_COMMANDS * ROBERT_UI_STEPS] = { 0 }; //Insert here!

#ifdef ROBERT_FREQ_COMPARE
//If you want to compare, insert here the data of the Outputted freq per core for each step.
numValue ROBERT_recordedFreqOut[ROBERT_PERIODIC_TOT_STEPS][N_EPI_CORE] = {{ }}; //Insert here!
#endif




#endif  //ROBERT_H
