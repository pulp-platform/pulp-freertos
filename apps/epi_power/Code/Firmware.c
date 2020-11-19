// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/********************************************************/
/*
* First Base Implementation of PMU Firmware,
* Using FreeRTOS, GAP Implementation
*
*
*/

/* FreeRTOS Inclusions. */
#include "pmsis.h"

/* GAP8 Inclusions */
#ifndef __PULP__
#include "pmsis_driver/pmsis_it.h"
#include "pmsis_hal/GAP8/fll/fll_gap8.h"
#endif

/* Libraries Inclusion */
#include "EPI_Config.h"
#include "EPI_types.h"
#include "EPI_task.h"
#include "EPI_math.h"
#ifdef SPI_TEST
	#include "model_spi.h"
	#include "simulation.h"
#endif
#ifdef MEASURE_ACTIVE
	#include "measure.h"
#endif

/* Utilities to control tasks */
TaskHandle_t taskHandles[N_TASKS] = {NULL};

/* Interrupt Handlers */
void TIMER1_IRQ_handler();
HANDLER_WRAPPER_LIGHT(TIMER1_IRQ_handler);
int PeriodCounterISR = TASKS_OS_PERIODICITY_MUL_FACTOR;

/* Interrupt Handlers */
#ifdef __HACK_FIRMWARE_OPT1
void __attribute__ ((interrupt))
#else
void
#endif
 TIMER1_IRQ_handler()
{

	#ifdef MEASURE_ACTIVE
	timerBuffer[Timerindex++] = (Timer_Data_t) {'I', measureReadCycle()};
	#endif

	BaseType_t xHigherPriorityTask1Woken = pdFALSE;
	BaseType_t xHigherPriorityTask2Woken = pdFALSE;

	vTaskNotifyGiveFromISR( taskHandles[0], &xHigherPriorityTask1Woken );

	PeriodCounterISR--;
	if (PeriodCounterISR <= 0) //TBD: <= or == ? <= also take into account some errors.
	{
		PeriodCounterISR = TASKS_OS_PERIODICITY_MUL_FACTOR;
		vTaskNotifyGiveFromISR( taskHandles[1], &xHigherPriorityTask2Woken);
	}

	/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
    should be performed to ensure the interrupt returns directly to the highest
    priority task.  The macro used for this purpose is dependent on the port in
    use and may be called portEND_SWITCHING_ISR(). */
	portYIELD_FROM_ISR( (xHigherPriorityTask1Woken | xHigherPriorityTask2Woken) );

	//TBD: Since we want always to trigger the CS, What if we pass true and use the var
	// xHigherPriority... to check if everything is ok?
}

/* Program Entry */
int main( void )
{
	// --------------------------------------------------------------- //
	/*** Initialization  ***/
	// --------------------------------------------------------------- //


	/** Controller (GAP8) Clock **/
	/*uint32_t voltage = DCDC_DEFAULT_NV, get_frequency;
	uint32_t frequency = pi_fll_soc_max_freq_at_V(voltage);

	if(pi_pmu_set_voltage(voltage, 1))
	{
		printf("Error changing voltage, check Frequency value!\n");
		printf("Test failed\n");
		pmsis_exit(-2);
	}
	if(pi_fll_set_frequency(FLL_SOC, frequency, 1 ) == -1)
	{
		printf("Error changing freqency, check Voltage value!\n");
		printf("Test failed\n");
		pmsis_exit(-2);
	}

	/* To Print the System Clock */
	#ifdef DEBUG_ACTIVE
	#ifndef __PULP__
	printf("Number of Core: %d,\n System Clock: %u \n", N_EPI_CORE, SystemCoreClock);
	#else
	/* _impure_ptr = 0; */
	printf("Number of Core: %d,\n System Clock: %u \n", N_EPI_CORE, system_core_clock);
	#endif
	#endif

	/** Timer ISR set up **/
	// This has to be called here otherwise it doesn't work in this SDK
#ifndef __PULP__
	NVIC_SetVector( FC_EVENT_TIMER1, (uint32_t)__handler_wrapper_light_TIMER1_IRQ_handler);
	NVIC_EnableIRQ( FC_EVENT_TIMER1 );
#else
	extern void (*isr_table[32])(void);
	extern void timer_irq_handler(void);

	/* init flls */
	for (int i = 0; i < ARCHI_NB_FLL; i++) {
		pi_fll_init(i, 0);
	}

	/* 250 mhz */
	if (pi_fll_set_frequency(FLL_SOC, 250000000u, 0) == -1) {
		exit(-1);
	}

	system_core_clock_update();

	/* make sure irq (itc) is a good state */
	pulp_irq_init();

	/* Hook up isr table. This table is temporary until we figure out how to
	 * do proper vectored interrupts.
	 */
	/* custom isr irq handler */
	isr_table[0xa] = timer_irq_handler;
	isr_table[0xb] = TIMER1_IRQ_handler;
	isr_table[0x1a] = fc_soc_event_handler; // 26

	/* mtvec is set in crt0.S */

	/* deactivate all soc events as they are enabled by default */
	pulp_soc_eu_event_init();

	/* Setup soc events handler. */
	pi_fc_event_handler_init(FC_SOC_EVENT);

	/* TODO: I$ enable*/
	/* enable core level interrupt (mie) */
	irq_clint_enable();
#endif


	/* Memory Allocation */


	/** Check & Start **/

	if ( (TELEMETRY_POLLING_FREQUENCY*500) < TASKS_CONTROL_PERIOD_US)
	{
		#ifdef DEBUG_ACTIVE
		printf("Error in TELEMETRY_POLLING_FREQUENCY parameter!\n");
		#endif
		pmsis_exit(-1);
	}

	/* Start the Scheduler */
	//vTaskStartScheduler();
	return pmsis_kickoff((void *) vInitializationTask);
	//gap_sdk/rtos/freeRTOS/vendors/gwt/TARGET_GWT/pmsis/pmsis_backend/pmsis_backend_native_task_api.c
}
