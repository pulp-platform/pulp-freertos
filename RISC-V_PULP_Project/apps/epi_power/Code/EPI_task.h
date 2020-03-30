
/***************************************************/
/*
* Task and function library.
*
*
*/


#ifndef _EPI_TASK_H_
#define _EPI_TASK_H_

#include "EPI_Config.h"

#ifdef SPI_TEST
	//#include "gap_spi.h"
#endif

/*** Data Struct ***/

typedef struct _sensor_data {

	numValue Frequency;
	numValue Voltage;
	numValue Temperature;
	//char Core; //TBD
} sensor_data_t;

typedef struct _telemetry {
	//TODO: Need to adjust this because we are passing only 1 Power (either Total or Mean)
	sensor_data_t TelSensorData[N_EPI_CORE];
	numValue PowerConsumption[N_EPI_CORE];
	numValue TotalPower;

	//numValue Workload;
} telemetry_t;

typedef struct _parameter_table {

	numValue TargetFreq[N_EPI_CORE];
	numValue Workload[N_EPI_CORE];
	numValue PowerFormulaCoeff[LMS_COEFF_NUMBER];
	numValue TotalPowerBudget;
	#ifdef EPI_CONFIG_QUADRANTS
		numValue QuadrantPowerBudget[EPI_N_QUADRANTS];
	#endif
	#ifdef EPI_CONFIG_FREQ_BINDING
		char CoreBindingVector[N_EPI_CORE]; //TODO: can we optimize this? we actually need only half of the matrix.
	#endif
} parameter_table_t;

/* BitMap Definition for Frequency reduction */
#ifdef FREQUENCY_REDUCTION_MAP
//
#define BM_MAX_SINGLE_POW_SAT	0x1
#define BM_ALPHA_RED 			0x2
#define BM_PID_TEMP_RED			0x4
#define BM_BINDING_RED			0x8
#define BM_MAX_SINGLE_FREQ		0x10
//20, 40, 80, 100
//quadrants
#define BM_RESET				0x0
//
#endif

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
int lReadReductionMap(char* oReductionMap);
int lSTARTWriteReductionMap(void);
int lENDWriteReductionMap(void);
#endif
//

numValue lCorePowerComputation(numValue *oCoreTargetPower, parameter_table_t *iParameterTable, char* FreqReductionMap);
numValue lAlphaPowerReduction(numValue *oCoreReducedPower, numValue *iCoreTargetPower, numValue *iMeasuredTemperature, numValue iDeltaPower, char* FreqReductionMap);
#ifdef EPI_CONFIG_FREQ_BINDING
void vCoreBindingReduction(numValue *oCoreTargetPower, char *iCoreBindingVector, char* FreqReductionMap);
#endif


#endif //lib
