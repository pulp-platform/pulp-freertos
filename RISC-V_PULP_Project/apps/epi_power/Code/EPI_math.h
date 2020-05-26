
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
#include "EPI_types.h"

/* Functions Declaration */
numValue lEPImathPIDcompute(numValue iTargetPower, numValue iMeasuredTemperature, char iCoreNumber);
numValue lEPImathPIDsetParameters(numValue kp, numValue ki, numValue kd, numValue dt, numValue Tcrit, numValue Tmargin, numValue Pidle);

numValue lEPImathPowerCompute(numValue iTargetFrequency, numValue iWorkload, numValue *iFormulaCoeff);
numValue lEPImathFrequencyCompute(numValue iCoreTargetPower, numValue iWorkload, numValue *iFormulaCoeff);
//pid_parameters_t TablePIDParameters; //TBD: to create a struc for PID parameters

numValue lEPImathLMSrecursive(numValue *oParam, numValue *oP_curr, numValue *iPrevParam, numValue *iP_prev, numValue *iInput, numValue iError, numValue iLambda);
void lEPImathLMSstoreP (numValue *oParam, numValue *iInput);

void MatMul(numValue *O, numValue *iA, numValue *iB, int r1, int col1, int col2);

#ifdef SPI_TEST
void lEPImathResetPID(void);
#endif

#endif
