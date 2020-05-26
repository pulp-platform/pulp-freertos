
/***************************************************/
/*
* Task and function library.
*
*
*/


#ifndef _EPI_TASK_H_
#define _EPI_TASK_H_

#include "EPI_Config.h"
#include "EPI_types.h"


/* Utilities to control tasks */
extern TaskHandle_t taskHandles[N_TASKS];
/*
 * 0: Periodic Task
 * 1: TaskOS
 *
*/

/*** Tasks Declaration ***/

void vPeriodicControl( void *parameters );
void vTaskOS( void *parameters );
void vBMC( void *parameters );
void vInitializationTask( void *parameters);
#ifdef MEASURE_ACTIVE
void vPrintTask( void *parameters );
#endif


/*** Functions Declaration ***/

int lReadCalibrationTable(parameter_table_t* oInternalCalibrationTable);
int lSTARTWriteCalibrationTable(void);
int lENDWriteCalibrationTable(void);
//int lReadAccumulationData(parameter_table_t* oInternalAccumulationData); // THIS function ALREADY exists in FreeRTOS_util.h
int lReadAccumulationData2(telemetry_t* oInternalAccumulationData);
int lSTARTWriteAccumulationData(void);
int lENDWriteAccumulationData(void);
//
#ifdef FREQUENCY_REDUCTION_MAP
int lReadReductionMap(uint8_t* oReductionMap);
int lSTARTWriteReductionMap(void);
int lENDWriteReductionMap(void);
#endif
#ifdef ERROR_MAP
int lWriteErrorMap(uint32_t ErrorMap);
int lReadErrorMap(uint32_t* oErrorMap);
#endif
//

numValue lCorePowerComputation(numValue *oCoreTargetPower, parameter_table_t *iParameterTable, uint8_t* FreqReductionMap);
numValue lAlphaPowerReduction(numValue *oCoreReducedPower, numValue *iCoreTargetPower, numValue *iMeasuredTemperature, numValue iDeltaPower, uint8_t* FreqReductionMap);
#ifdef EPI_CONFIG_FREQ_BINDING
void vCoreBindingReduction(numValue *oCoreTargetPower, char *iCoreBindingVector, uint8_t* FreqReductionMap);
#endif


#endif //lib
