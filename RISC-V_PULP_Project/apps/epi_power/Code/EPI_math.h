
/***************************************************/
/*
* Math and Control Functions Library
*
*
*/


#ifndef _EPI_MATH_H_
#define _EPI_MATH_H_

//#include "FreeRTOS_util.h"
#include "EPI_Config.h"

/* Functions Declaration */
numValue lEPImathPIDcompute(numValue iTargetPower, numValue iMeasuredTemperature, char iCoreNumber);
numValue lEPImathPIDsetParameters(numValue kp, numValue ki, numValue kd, numValue dt, numValue Tcrit, numValue Tmargin, numValue Pidle);

numValue lEPImathPowerCompute(numValue iTargetFrequency, numValue iWorkload, numValue *iFormulaCoeff);
numValue lEPImathFrequencyCompute(numValue iCoreTargetPower, numValue iWorkload, numValue *iFormulaCoeff);
//pid_parameters_t TablePIDParameters; //TBD: to create a struc for PID parameters

numValue lEPImathLMSrecursive(numValue iError, numValue iInput, numValue iMu);

#ifdef SPI_TEST
void lEPImathResetPID(void);
#endif

#endif
