


#ifndef _EPI_TYPES_H_
#define _EPI_TYPES_H_

#include "EPI_Config.h"


#ifdef FLOAT_NUMBERS
	typedef float numValue;
#else
	typedef int numValue;
#endif

#define BM_RESET				0x0

/* BitMap Definition for Frequency reduction */
#ifdef FREQUENCY_REDUCTION_MAP
//
#define BM_FRM_MAX_SINGLE_POW_SAT	0x1
#define BM_FRM_ALPHA_RED 			(0x1 << 1)
#define BM_FRM_PID_TEMP_RED			(0x1 << 2)
#define BM_FRM_BINDING_RED			(0x1 << 3)
#define BM_FRM_MAX_SINGLE_FREQ		(0x1 << 4)
//20, 40, 80, 100
//quadrants

//
#endif



#ifdef ERROR_MAP
// 1st 8 bits
#define BM_ERROR_FREERTOS_MEMORY_ALLOCATION_TASK 	0x1
#define BM_ERROR_FREERTOS_MEMORY_ALLOCATION_OBJ 	(0x1 << 1)
#define BM_ERROR_REACHED_EOT						(0x1 << 2)
#define BM_ERROR_INITIALIZATION						(0x1 << 3)
// 2nd 8 bits
#define BM_ERROR_CRITICAL							(0x1 << 8)
#define BM_ERROR_PROBLEMATIC						(0x1 << 9)
#define BM_ERROR_LIGHT								(0x1 << 10)
//3rd 8 bits
#define BM_ERROR_SHARED_VAR_READ					(0x1 << 16)
#define BM_ERROR_SHARED_VAR_WRITE					(0x1 << 17)
#define BM_ERROR_NUMBERS_VALUE						(0x1 << 18)

//4th 8 bits, left for debug
#define BM_ERROR_D_SPI_R_HS							(0x1 << 24)
#define BM_ERROR_D_SPI_W_HS							(0x1 << 25)
#define BM_ERROR_D_SPI_R_INDEX						(0x1 << 26)
#define BM_ERROR_D_SPI_W_INDEX						(0x1 << 27)
#define BM_ERROR_D_SPI_TOS_HS						(0x1 << 28)
#define BM_ERROR_D_SPI_TOS_INDEX					(0x1 << 29)

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
	numValue PowerFormulaCoeff[N_EPI_CORE][LMS_COEFF_NUMBER];
	numValue TotalPowerBudget;
	#ifdef EPI_CONFIG_QUADRANTS
		numValue QuadrantPowerBudget[EPI_N_QUADRANTS];
	#endif
	#ifdef EPI_CONFIG_FREQ_BINDING
		char CoreBindingVector[N_EPI_CORE]; //TODO: can we optimize this? we actually need only half of the matrix.
	#endif
} parameter_table_t;







#endif //lib
