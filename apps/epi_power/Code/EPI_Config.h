
/***************************************************/
/*
* Parameters for PMU Firmware.
*
*
*/

/* NAME RULES
*
*	Define: all upper-case (separation with '-')
*	Functions: returnValue+libraryFileName+Description, no '_', input/output variables have i/o before the name
*	struct: all lower-case, separation with '_', final _t
*	variable Name: no '_', first letter upper-case. If global, put a 'g' if L1 '1' if L2 '2'
*
*/


#ifndef _EPI_CONFIG_H_
#define _EPI_CONFIG_H_

/****** Configurable ******/

/******************************/
/** Configuration of EPI CPU **/
/******************************/
#define N_EPI_CORE 								9 //36		// Number of Cores of the EPI processor

/* Parameters of EPI CPU */
#define EPI_CORE_CRITICAL_TEMPERATURE			360.0		// Max Temperature for each Core in Kelvin
#define EPI_CORE_TEMPERATURE_MARGIN				10.0		// A margin to the above Temperature
#define EPI_CORE_IDLE_POWER						0.7		// Min Power for a Single Core to stay alive
#define EPI_CORE_MAX_POWER_SINGLE				2.5		// Max Power for each Core
//TBC: are these needed or can be recovered from the max single power and idle power?
#define EPI_CORE_MIN_FREQUENCY					1.2
#define EPI_CORE_MAX_FREQUENCY					3.0

/* Initialization Values */
#define INIT_EPI_CORE_FREQ						1.0
#define INIT_EPI_CORE_VOLT						0.5
#define INIT_EPI_CORE_WORKLOAD					0.5333 //1.7777778
#define INIT_EPI_TOTAL_POWER_BUDGET				22.5 //(36*2.5) //TODO:

/******************************/
/*** Control Configuration ****/
/******************************/
//#define EPI_CONFIG_QUADRANTS					// Uncomment to use Quadrants max Power Consumption
#ifdef EPI_CONFIG_QUADRANTS
	#define EPI_N_QUADRANTS 					4		// Number of Quadrans
#endif
//#define EPI_CONFIG_FREQ_BINDING					// Uncomment to use Frequency Bindings between Cores


/******************************/
/***** PID Configuration ******/
/******************************/
#define PID_INTEGRAL_ACTION						// Uncomment to use Integral Action inside PID
//#define PID_DERIVATIVE_ACTION					// Uncomment to use Derivative Action inside PID
#define PID_SATURATION							// Uncomment to use Output Saturation inside PID
#define PID_FF_ACTION							// Uncomment to use FeedForward Action inside PID
#define PID_ANTI_WINDUP_SAT						// Uncomment to use Anti Wind-up of the Integrator, Saturation kind

/* Initialization Values */
// They have to be defined even if PID_CONFIG_x is disabled //TODO: fix this?
#define INIT_PID_KP 							2//2
#define INIT_PID_KI 							0.2
#define INIT_PID_KD 							3
#define INIT_PID_DT								0.025
#define PID_ANTI_WINDUP_SAT_COEFF				-0.75

/******************************/
/**** Tasks Configuration *****/
/******************************/
#ifdef __PULP__

#ifndef TASKS_CONTROL_PERIOD_US
/* #define TASKS_CONTROL_PERIOD_US 				(500) // Time in us //when change this also hange TELEMETRY_POLLING_FREQUENCY */
#endif

#else
#define TASKS_CONTROL_PERIOD_US 				(100000) // Time in us //when change this also hange TELEMETRY_POLLING_FREQUENCY
#endif
#define TASKS_OS_PERIODICITY_MUL_FACTOR			2 //following the rule Tos = m * Tcontrol, where T is the Period and m this value to be defined here.

/******************************/
/***** Code configuration *****/
/******************************/
//#define USE_CODE_WATCHDOGS
//#define USE_SECURE_IO
//#define USE_TESTS_ON_NUMBERS	//TODO: configASSERT seem not to work

/******************************/
/*** Telemetry Configuration **/
/******************************/
#define TELEMETRY_POLLING_FREQUENCY				300 //ms
#define FREQUENCY_REDUCTION_MAP					// Uncomment to collect the information on the Frequency Reduction causes
#define ERROR_MAP


/****** Private ******/
/* NOT CONFIGURABLE */

/** Code Configuration **/
#define DEBUG_ACTIVE
//#define MEASURE_ACTIVE
#ifdef __PULP__

#ifndef MEASURE_N_ITERATION
/* #define MEASURE_N_ITERATION 					20 // This is needed for both debug and Measure (DEFAULT) */
#endif

#else
#define MEASURE_N_ITERATION 					20 // This is needed for both debug and Measure (DEFAULT)
#endif
/* #define MEASURE_N_ITERATION 					200 // This is needed for both debug and Measure */

#define FLOAT_NUMBERS

//#define SPI_TEST
//#define GPIO_TEST
#define ROBERT_TEST

#define SPI_SPEED_MUL							10000 // default: 1500

#define ALPHA_MAX_VALUE							2.0

/** GAP8 Cluster **/
//#define PULP_USE_CLUSTER
// the stack dimension in bytes for data of each PCore
#define PCORE_STACK_SIZE 						2048

/** Tasks **/
// Number of Tasks
#define N_TASKS 								5

// Priority
#define PERIODIC_CONTROL_PRIORITY 				(tskIDLE_PRIORITY + 6)
#define TASK_OS_PRIORITY 						(tskIDLE_PRIORITY + 4)
#define BMC_PRIORITY 							(tskIDLE_PRIORITY + 3)
#define INITILIZATION_TASK_PRIORITY				8 //(configMAX_PRIORITIES - 1)
#if (defined(DEBUG_ACTIVE) || defined(MEASURE_ACTIVE))
	#define PRINT_TASK_PRIORITY					(tskIDLE_PRIORITY + 2)
#endif

/** Security **/
#ifdef USE_CODE_WATCHDOGS
	//extern int gLoopWatchDog;
	//extern int gTaskOSMiss;
#endif

/** Other Parameters **/
#define LMS_COEFF_NUMBER						2
#define LMS_BATCH_STEPS							(4 + LMS_COEFF_NUMBER * 2)





#endif //#ifndef
