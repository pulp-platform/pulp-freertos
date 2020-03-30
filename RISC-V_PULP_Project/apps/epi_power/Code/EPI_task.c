/***************************************************/
/*
* Task and function library.
*
*
*/

/* FreeRTOS Inclusions. */
#include "pmsis.h"

/* GAP8 Inclusions */
#include "pmsis_driver/pmsis_drivers.h"

/* Libraries Inclusion */
#include "EPI_task.h"
#include "EPI_math.h"
#include "EPI_Config.h"
#ifdef MEASURE_ACTIVE
	#include "measure.h"
#endif
#ifdef SPI_TEST
	#include "model_spi.h"
	#include "simulation.h"
#endif

/* Others */
#ifdef FLOAT_NUMBERS
	#include "print_float.h"
#endif

/*** Global Variable ***/

PI_L2 telemetry_t gAccumulationData;
PI_L2 parameter_table_t gCalibrationTable;
#ifdef FREQUENCY_REDUCTION_MAP
PI_L2 uint8_t gFreqReductionMap[N_EPI_CORE];
#endif

/* One Mutex for each Shared Global Variable*/
SemaphoreHandle_t gSemAccumulationData = NULL;
SemaphoreHandle_t gSemCalibrationTable = NULL;
#ifdef FREQUENCY_REDUCTION_MAP
SemaphoreHandle_t gSemReductionMap = NULL;
#endif

/* SPI */
#ifdef SPI_TEST
PI_L2 uint8_t txData[SPI_Bytes] = {0};
PI_L2 uint8_t txData2[SPI_Bytes] = {0};
PI_L2 uint8_t rxData[SPI_Bytes] = {0};
PI_L2 uint8_t rxData2[SPI_Bytes] = {0};
#endif

/*** Tasks ***/

void vPeriodicControl( void *parameters ) {

	/*------------------------------------*/
	/********* Function Variables *********/
	/*------------------------------------*/

	// Control Variables
	numValue ComputedCoreFrequency[N_EPI_CORE];
	numValue ComputedCoreVoltage[N_EPI_CORE];
	numValue MeasuredTemperature[N_EPI_CORE];
	numValue CoreTargetPower[N_EPI_CORE];
	numValue CoreReducedPower[N_EPI_CORE] = {0};

	numValue DeltaPower = 0;
	numValue TotalEpiPower = 0;

	// Internal to Global Variables
	parameter_table_t ParameterTable;
	sensor_data_t TelemetrySensorAccumulation[N_EPI_CORE];
	#ifdef FREQUENCY_REDUCTION_MAP
	char FreqReductionMap[N_EPI_CORE];
	#else
	char* FreqReductionMap = NULL;
	//It has to be always defined. Done this to improve performance while keeping the code readable
	#endif
	numValue AccTotalPower = 0;
	numValue TelemetryPowerAccumulation[N_EPI_CORE];

	//TBC: these variables
	int PeriodicityCounter; //To fetch data from Global Var
	int TelemetryCounter; //Periodicity of the telemetry. Maybe to change.
	const int TelemetryHorizon = (TELEMETRY_POLLING_FREQUENCY * 1000) / TASKS_CONTROL_PERIOD_US; //TBD: is it better to save cycle or memory?
	//the 2 var above are used for the optimization for cycles (not memory)

	//Simulation
	#ifdef MEASURE_ACTIVE
		int dIterationCount = 0;
	#endif



	/*------------------------------------*/
	/*********** Initialization ***********/
	/*------------------------------------*/

	#ifdef FREQUENCY_REDUCTION_MAP
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
		FreqReductionMap[i] = BM_RESET;
	#endif

	//Get initial Values
	if (!lReadCalibrationTable(&ParameterTable))
	{
		//Do Something.. //TODO
	}
	/*** C.1: Compute Target Power per Core ***/
	TotalEpiPower = lCorePowerComputation(CoreTargetPower, &ParameterTable, FreqReductionMap);
	#ifdef USE_TESTS_ON_NUMBERS
		configASSERT(TotalEpiPower > 0)
	#endif
	/*** C.2: Compute Delta Power ***/
	DeltaPower = TotalEpiPower - ParameterTable.TotalPowerBudget;

	PeriodicityCounter = TASKS_OS_PERIODICITY_MUL_FACTOR + 1; //TBC: I put '+1' (ONLY HERE) to create an initial shift so the TaskOS can have time to compute and pass the values.
	//TBC: Check how much time TaskOS needs to compute the values (can it do it in the 500us cycles or need more and so it is preempted?)
	TelemetryCounter = TelemetryHorizon;

	/*------------------------------------*/
	/***** Algorithtm Initialization ******/
	/*------------------------------------*/

	for (uint8_t i = 0; i < N_EPI_CORE; i++)
	{
		ComputedCoreFrequency[i] = ParameterTable.TargetFreq[i]; //TBD
		CoreReducedPower[i] = CoreTargetPower[i];
	}

	//xTaskNotifyGive(NULL); //check if null is ok

	/* END Algorithm Initialization */


	/* Task Code */
	 for (;;)
	 {

		/*-----------------------------------------------*/
		/* 0: Wait the timer to trigger a new execturion */
		/*-----------------------------------------------*/


		ulTaskNotifyTake( pdTRUE, 	// xClearCountOnExit: if pdTRUE = Binary Semaphore; if number = Counting Semaphore (# of times to be called before exit).
					portMAX_DELAY); // xTicksToWait. portMAX_DELAY. //TBD

		#ifdef GPIO_TEST
		//GPIO_A17_toggle();
		//GPIO_A19_toggle();
		//pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 1);
		#endif

		#ifdef MEASURE_ACTIVE
		if (dIterationCount >= (MEASURE_N_ITERATION-1))
		{
			lPerformanceCheck = 1;
			pi_timer_stop(FC_TIMER_1);
			timerBuffer[Timerindex++] = (Timer_Data_t) {'F', measureReadCycle()};

			printf("End\n");

			//vTaskSuspend(taskHandles[1]);
			vTaskSuspend(NULL);
		}
		else
		{
			dIterationCount++;
			//printf("%d\n", dIterationCount);
			CSCheck = 1;
			timerBuffer[Timerindex++] = (Timer_Data_t) {'P', measureReadCycle()}; //P for Periodic Task
		}
		#endif

		/*-----------------------------------------------*/
		/********* 1: Write Computed Values(k-1) *********/
		/*-----------------------------------------------*/

		#ifdef SPI_TEST
		//resource mutex
		xSemaphoreTake(gSemSPI_HR, portMAX_DELAY);
		//1 Handshake to start. The message has different order and 2 different value
		SPI_AsyncWrite(SPI_bits, SPI_WRITE_HANDSHAKE);
		xSemaphoreTake(gSemSPI_write, portMAX_DELAY);

		SPI_AsyncRead(SPI_bits, rxData);
		xSemaphoreTake(gSemSPI_read, portMAX_DELAY);
		if (!( (rxData[0] == SPI_WRITE_HANDSHAKE[0]) &&
			(rxData[1] == SPI_WRITE_HANDSHAKE[1]) &&
			(rxData[2] == SPI_WRITE_HANDSHAKE[2]) &&
			(rxData[3] == SPI_WRITE_HANDSHAKE[3]) ))
		{
			//TODO something ...

			#ifdef GPIO_TEST
			pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 1);
			#endif
		}
		else
		{
			#ifdef GPIO_TEST
			pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 0);
			#endif

			//SPI_AsyncWrite(SPI_bits, SPI_COM_WRITE_FREQ);
			//xSemaphoreTake(gSemSPI_write, portMAX_DELAY);

			for (uint8_t i = 0; i < N_EPI_CORE; i++)
			{
				txData[0] = SPI_TX_HEADER;
				txData[1] = i;

				uint16_t extract = (uint16_t )(ComputedCoreFrequency[i] * 10000.0);

				//printf("%hu\n\r", extract);
				//printFloat(ComputedCoreFrequency[i]);

				uint8_t* conv_pt = &extract;
				//uint8_t* conv_pt = ComputedCoreFrequency + i*2; //&ComputedCoreFrequency[i];
				txData[2] = conv_pt[0];
				txData[3] = conv_pt[1];
				//SPI_AsyncWrite(SPI_bits, (ComputedCoreFrequency + SPI_bits*i));
				SPI_AsyncWrite(SPI_bits, txData);
				xSemaphoreTake(gSemSPI_write, portMAX_DELAY);
			}
		}
		//#else
			//nothing
		//release mutex
		xSemaphoreGive(gSemSPI_HR);
		#endif //spi

		// While Waiting for the DMA we do other stuff:

		/*** A: WatchDog resetting ***/
		/*
		#ifdef USE_CODE_WATCHDOGS
			gLoopWatchDog = 1;
		#endif
		This is commented because it gives compilers linking errors--.


		/*** B: Manage the Information Data ***/
		TelemetryCounter--;
		for (uint8_t i = 0; i < N_EPI_CORE; i++)
		{
			/*** B.1: Compute Accumulation Values ***/
			TelemetrySensorAccumulation[i].Frequency 	+= ComputedCoreFrequency[i];
			TelemetrySensorAccumulation[i].Voltage 		+= ComputedCoreVoltage[i];
			TelemetrySensorAccumulation[i].Temperature	+= MeasuredTemperature[i];

			TelemetryPowerAccumulation[i] 	+= CoreReducedPower[i];
			//Total Power is computed below, before the accumulation
		}

		/*** B.2: Write Accumulation Values ***/
		//Check if the Window Horizon for the measurement is reached
		//TBD: Better to do 2 for(36) and 1 if, or 1 for and 36if? If the second is better, move this if inside the for above.
		if (TelemetryCounter <= 0) //TBD: <= or == ? <= also take into account some errors.
		{
			TelemetryCounter = TelemetryHorizon;

			// Take the Mutex
			if (lSTARTWriteAccumulationData())
			{
				for (uint8_t i = 0; i < N_EPI_CORE; i++)
				{
					gAccumulationData.TelSensorData[i].Frequency 	= TelemetrySensorAccumulation[i].Frequency / TelemetryHorizon;
					gAccumulationData.TelSensorData[i].Voltage 		= TelemetrySensorAccumulation[i].Voltage / TelemetryHorizon;
					gAccumulationData.TelSensorData[i].Temperature 	= TelemetrySensorAccumulation[i].Temperature / TelemetryHorizon;

					gAccumulationData.PowerConsumption[i] 			= TelemetryPowerAccumulation[i] / TelemetryHorizon;

					AccTotalPower 									+= TelemetryPowerAccumulation[i];

					// Resetting the Accumulation Internal Variables
					TelemetrySensorAccumulation[i].Frequency 	= 0;
					TelemetrySensorAccumulation[i].Voltage		= 0;
					TelemetrySensorAccumulation[i].Temperature	= 0;

					TelemetryPowerAccumulation[i] 				= 0;
				}
				#ifdef USE_TESTS_ON_NUMBERS
					configASSERT((AccTotalPower / TelemetryHorizon) > (EPI_CORE_IDLE_POWER * N_EPI_CORE) )
				#endif
				gAccumulationData.TotalPower = AccTotalPower / TelemetryHorizon;
				// Resetting
				AccTotalPower = 0;

				// Do this ONLY if the Mutex was correctly taken
				if(!lENDWriteAccumulationData())
				{
					//TODO: Do something.
				}
			}
			else
			{
				//TODO: Do something.
			}
		}

		#ifdef MEASURE_ACTIVE
		timerBuffer[Timerindex++] = (Timer_Data_t) {'A', measureReadCycle()}; //B for Section B end.
		#endif



		/*-----------------------------------------------*/
		/*************** 2: Read Values(k) ***************/
		/*-----------------------------------------------*/

		#ifdef SPI_TEST
		xSemaphoreTake(gSemSPI_HR, portMAX_DELAY);
		//1 Handshake to start. The message has different order and 2 different value
		SPI_AsyncWrite(SPI_bits, SPI_READ_HANDSHAKE);
		xSemaphoreTake(gSemSPI_write, portMAX_DELAY);

		SPI_AsyncRead(SPI_bits, rxData);
		xSemaphoreTake(gSemSPI_read, portMAX_DELAY);
		if (!( (rxData[0] == SPI_READ_HANDSHAKE[0]) &&
			(rxData[1] == SPI_READ_HANDSHAKE[1]) &&
			(rxData[2] == SPI_READ_HANDSHAKE[2]) &&
			(rxData[3] == SPI_READ_HANDSHAKE[3]) ))
		{
			//TODO something ...

			#ifdef GPIO_TEST
			pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 1);
			#endif
		}
		else
		{
			#ifdef GPIO_TEST
			pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 0);
			#endif

			//SPI_AsyncWrite(SPI_bits, SPI_COM_READ_TEMP);
			//xSemaphoreTake(gSemSPI_write, portMAX_DELAY);

			for (uint8_t i = 0; i < N_EPI_CORE; i++)
			{
				//SPI_AsyncRead(SPI_bits, (MeasuredTemperature + SPI_Bytes * i) );
				SPI_AsyncRead(SPI_bits, rxData );
				xSemaphoreTake(gSemSPI_read, portMAX_DELAY);

				uint16_t* conv_pt = rxData + 2;
				//printf("%hu\n\r", (*conv_pt)-27300);
				MeasuredTemperature[i] = ( (numValue) *conv_pt ) / 100.0;
			}
		}
		xSemaphoreGive(gSemSPI_HR);
		//#else
			//for (uint8_t i = 0; i < N_EPI_CORE; i++)
				//MeasuredTemperature[i] = 80 + 274;

		#endif //SPI


		// While Waiting for the DMA we do other stuff, PART 2:


		/*** C: Read Calibration Table ***/

		//Since the TaskOS has a periodicity Tos = m * Tcontrol:
		PeriodicityCounter--; //it is better to -- instead of ++ to compare it with 0 instead of a Global constant Define.
		if (PeriodicityCounter <= 0) //TBD: <= or == ? <= also take into account some errors.
		//TBD: Do we need an extra layer (or a substitutive one) where instead of this we just check a global var? (be careful with races)
		{
			PeriodicityCounter = TASKS_OS_PERIODICITY_MUL_FACTOR; //TBD: if we use global variable check, this should be put elsewhere.
			if(!lReadCalibrationTable(&ParameterTable))
			{
				//Do something.
			}

			/*** C.1: Compute Target Power per Core ***/
			TotalEpiPower = lCorePowerComputation(CoreTargetPower, &ParameterTable, FreqReductionMap);
			#ifdef USE_TESTS_ON_NUMBERS
				configASSERT(TotalEpiPower > 0)
			#endif

			/*** C.2: Compute Delta Power ***/
			DeltaPower = TotalEpiPower - ParameterTable.TotalPowerBudget;
			// printf("DP:  ");
			// printFloat(DeltaPower);
			// printFloat(TotalEpiPower);
			// printFloat(CoreTargetPower[0]);
			#ifdef USE_TESTS_ON_NUMBERS
				configASSERT( (abs(DeltaPower) < ParameterTable.TotalPowerBudget))
			#endif
		}

		#ifdef MEASURE_ACTIVE
		timerBuffer[Timerindex++] = (Timer_Data_t) {'C', measureReadCycle()}; //C for Section C end.
		#endif

		// printf("DP:  ");
		// printFloat(DeltaPower);
		// printFloat(TotalEpiPower);
		// printFloat(CoreTargetPower[0]);

		/* D: Read pending BMC */
		/*
		if (BMC PENDING)
		{
			xTaskNotifyGive( BMC_TASK);
			//do other stuff?
		}

		//WAIT THE DMA READ TO END!

		/*-----------------------------------------------*/
		/********** 3: Compute Frequency Values **********/
		/*-----------------------------------------------*/

		/*** 3.A: Power Dispatching Layer ***/

		// Started Above: C.1, C.2
		//If necessary, we Reduce Power of each Core to meet Global Power Budget
		if  (DeltaPower > 0)
		{
			lAlphaPowerReduction(CoreReducedPower, CoreTargetPower, MeasuredTemperature, DeltaPower, FreqReductionMap);
		}
		else
		{
			for (uint8_t i = 0; i < N_EPI_CORE; i++)
				CoreReducedPower[i] = CoreTargetPower[i];
		}

		//printFloat(CoreReducedPower[0]);

		#ifdef MEASURE_ACTIVE
		timerBuffer[Timerindex++] = (Timer_Data_t) {'a', measureReadCycle()}; //2 for Part 2 end.
		#endif

		//printFloat(CoreReducedPower[0]);

		/*** 3.B: Compute PID Values ***/
		for (uint8_t i = 0; i < N_EPI_CORE; i++)
		{
			#ifdef FREQUENCY_REDUCTION_MAP
			numValue CoreBefore = CoreReducedPower[i]; //TODO: Optimize this....
			#endif

			CoreReducedPower[i] = lEPImathPIDcompute(CoreReducedPower[i], MeasuredTemperature[i], i);

			#ifdef FREQUENCY_REDUCTION_MAP
			if (CoreReducedPower[i] < CoreBefore)
			{
				FreqReductionMap[i] |= BM_PID_TEMP_RED;
				// Do this ONLY if the Mutex was correctly taken
			}
			#endif
		}

		//printFloat(CoreReducedPower[0]);

		#ifdef MEASURE_ACTIVE
		timerBuffer[Timerindex++] = (Timer_Data_t) {'p', measureReadCycle()}; //2 for Part 2 end.
		#endif

		/*** 3.C: Check Bindings between Cores ***/
		#ifdef EPI_CONFIG_FREQ_BINDING
		vCoreBindingReduction(CoreReducedPower, ParameterTable.CoreBindingVector, FreqReductionMap);
		#endif

		#ifdef MEASURE_ACTIVE
		timerBuffer[Timerindex++] = (Timer_Data_t) {'b', measureReadCycle()}; //2 for Part 2 end.
		#endif

		/*** 3.D: Compute Frequence/Duty Cycle ***/
		// Frequency from Power. (newton rapton) (vdd is in function of f )
		for (uint8_t i = 0; i < N_EPI_CORE; i++)
		{
			ComputedCoreFrequency[i] = lEPImathFrequencyCompute(CoreReducedPower[i], ParameterTable.Workload[i], ParameterTable.PowerFormulaCoeff);
			//TODO: can I optimize this formula? Because I have to pass all the time the address of the Formula Coeff which are fixed per 2 iteration of the control task
			#ifdef USE_TESTS_ON_NUMBERS
				configASSERT( (ComputedCoreFrequency[i] > EPI_CORE_MIN_FREQUENCY) && (ComputedCoreFrequency < EPI_CORE_MAX_FREQUENCY) ) //TODO: here put EPI_CORE_MIN_FREQ e MAX_FREQ.
			#endif
		}

		//printFloat(ComputedCoreFrequency[0]);
		//printf("%x\n", FreqReductionMap[0]);

		/* Check (again? @askChristian) Binding between Cores (Frequencies) */
		#ifdef EPI_CONFIG_FREQ_BINDING
		vCoreBindingReduction(ComputedCoreFrequency, ParameterTable.CoreBindingVector, FreqReductionMap);
		#endif

		/*** Not sure the right position for this code, it may be put while waiting for the DMA ***/
		//TBD: we don't need to do it every cycle? only when bmc ask? Do I or everyhing?
		#ifdef FREQUENCY_REDUCTION_MAP
		if(lSTARTWriteReductionMap())
		{
			for (uint8_t i = 0; i < N_EPI_CORE; i++)
			{
				gFreqReductionMap[i] = FreqReductionMap[i]; //TBD: This is an istant measurement. Do we want a Full OR for the given period of time?
				FreqReductionMap[i] = BM_RESET;
			}

			// Do this ONLY if the Mutex was correctly taken
			if(!lENDWriteReductionMap())
			{
				//TODO: Do something.
			}
		}
		else
		{
			//TODO: Do something.
			//TBD: Reset anyway?
			for (uint8_t i = 0; i < N_EPI_CORE; i++)
			{
				FreqReductionMap[i] = BM_RESET;
			}
		}
		#endif



		#ifdef MEASURE_ACTIVE
		timerBuffer[Timerindex++] = (Timer_Data_t) {'e', measureReadCycle()};
		#endif




		/* taskYIELD is used in case of Cooperative Scheduling, to allow/force Contex Switches */
		#if configUSE_PREEMPTION == 0
       		taskYIELD();
    	#endif
	}

	/* Cannot and Shouldn't reach this point, but If so... */
	// TODO: SIGNAL HAZARDOUS ERROR, FAILURE!
	vTaskDelete( NULL );
}

void vTaskOS( void *parameters ) {

	/* Description */
	/* This task is responsable to be the interpreter and the messenger between the OS of the
	* EPI CPU server and the firmaware of the PMU. The task will also check the voltage regulator
	* values (initially only the EPI core VR, then also HBM, DRAM, MB, and other sockets VR values);
	* it also do an adaptive control algorithm to update the parameters that are used to compute
	* optimal voltage values (both set values and PID parameters).
	* The task will held data as workload histrory, mean power consumption, etc.
	*/

	/* Function Variables */
	numValue MeasuredPowerVR;

	//Least Square Variables
	numValue RMSError;
	numValue InputOfLMS[LMS_COEFF_NUMBER];
	numValue MeanFreq, MeanWorkload, MeanVoltage;
	// Convergence factor
	numValue Mu = 1; //TODO: Def
	// Forgetting Factor
	numValue Lambda  = 1; //TODO: Def

	//Global To Internal Variables
	telemetry_t AccumulationData;
	parameter_table_t CalibrationTable;

	/* Initialization */
	lReadCalibrationTable(&CalibrationTable); //TODO: if = 1 else = 0 etc...
	MeanFreq = 0;
	MeanWorkload = 0;
	MeanVoltage = 0;
	#ifdef SPI_TEST
	numValue numConverter = 0;
	#endif

	/* Task Code */
	for (;;)
	{

		/*-----------------------------------------------*/
		/* 0: Wait the timer to trigger a new execturion */
		/*-----------------------------------------------*/

		ulTaskNotifyTake( pdTRUE, 	// xClearCountOnExit: if pdTRUE = Binary Semaphore; if number = Counting Semaphore (# of times to be called before exit).
					portMAX_DELAY); // xTicksToWait. portMAX_DELAY. //TBD

		#ifdef GPIO_TEST
		//GPIO_A19_toggle();
		//pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 1);
		#endif

		/*-----------------------------------------------*/
		/********** 1: Read Voltage Regulators ***********/
		/*-----------------------------------------------*/
		#ifdef SPI_TEST

		#else
		MeasuredPowerVR = 10; //DATA_MeasuredPowerVR[dIterationCount];
		#endif

		// /*** 1.B: Read Accumulated values ***/
		// if(!lReadAccumulationData2(&AccumulationData))
		// {
		// 	//Do something.
		// }
		//
		// /*-----------------------------------------------*/
		// /********** 2: Update Calibration Table **********/
		// /*-----------------------------------------------*/
		//
		// RMSError = MeasuredPowerVR - AccumulationData.TotalPower;
		// #ifdef USE_TESTS_ON_NUMBERS
		// 	configASSERT(abs(RMSError) < AccumulationData.TotalPower)
		// #endif
		//
		// //TBC: Mean of Workload and Frequence
		// for (uint8_t i = 0; i < N_EPI_CORE; i++)
		// {
		// 	MeanVoltage 	+= AccumulationData.TelSensorData[i].Voltage;
		// 	MeanFreq 		+= AccumulationData.TelSensorData[i].Frequency;
		// 	//MeanWorkload 	+= ??; //TODO
		// }
		//
		// MeanVoltage = MeanVoltage / N_EPI_CORE;
		// MeanFreq = MeanFreq / N_EPI_CORE;
		// //MeanWorkload = MeanWorkload / N_EPI_CORE; //TODO
		// 	MeanWorkload = 100;
		// #ifdef USE_TESTS_ON_NUMBERS
		// 	configASSERT(MeanVoltage > 0)
		// 	configASSERT(MeanFreq > 0)
		// 	configASSERT(MeanWorkload > 0)
		// #endif
		//
		// //InputOfLMS = {f*vd^2, 1, w^k5, f*w^k5};
		// InputOfLMS[0] = MeanFreq*10*10;
		// InputOfLMS[1] = 1;
		// InputOfLMS[2] = MeanWorkload;
		// InputOfLMS[3] = MeanFreq*MeanWorkload;
		//
		// // Reset
		// MeanVoltage = 0;
		// MeanFreq = 0;
		// MeanWorkload = 0;

//Questa parte sotto dà problemi
/*
		for (int i = 0; i < LMS_COEFF_NUMBER; i++)
		{
			CalibrationTable.PowerFormulaCoeff[i] += lEPImathLMSrecursive(RMSError, InputOfLMS[i], Mu);

			#ifdef USE_TESTS_ON_NUMBERS
				configASSERT(CalibrationTable.PowerFormulaCoeff[i] ??) //TODO: find an Assert
			#endif

		}
*/

		/*-----------------------------------------------*/
		/********** 3: Check request from EPI OS *********/
		/*-----------------------------------------------*/

		//TBD: I decided to do this via polling:
		#ifdef SPI_TEST
		xSemaphoreTake(gSemSPI_HR, portMAX_DELAY);

		//1 Handshake to start. The message has different order and 2 different value
		SPI_AsyncWrite(SPI_bits, SPI_WRITE_UI);
		xSemaphoreTake(gSemSPI_write, portMAX_DELAY);

		SPI_AsyncRead(SPI_bits, rxData);
		xSemaphoreTake(gSemSPI_read, portMAX_DELAY);
		if (!( (rxData[0] == SPI_WRITE_UI[0]) &&
			(rxData[1] == SPI_WRITE_UI[1]) &&
			(rxData[2] == SPI_WRITE_UI[2]) &&
			(rxData[3] == SPI_WRITE_UI[3]) ))
		{
			//TODO something ...

			#ifdef GPIO_TEST
			//pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 1);
			pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 1);
			#endif
		}
		else
		{
			#ifdef GPIO_TEST
			//pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 0);
			pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 0);
			#endif

			//SPI_AsyncWrite(SPI_bits, SPI_COM_WRITE_FREQ);
			//xSemaphoreTake(gSemSPI_write, portMAX_DELAY);

			for (uint8_t i = 0; i < N_EPI_CORE; i++)
			{
				SPI_AsyncRead(SPI_bits, rxData );
				xSemaphoreTake(gSemSPI_read, portMAX_DELAY);

				uint16_t* conv_pt = rxData + 2;
				//printf("%hu\n\r", (*conv_pt)-27300);
				//if (i == )
				CalibrationTable.TargetFreq[i] = ( (numValue) *conv_pt ) / 10000.0;

				SPI_AsyncRead(SPI_bits, rxData );
				xSemaphoreTake(gSemSPI_read, portMAX_DELAY);
				//printf("%hu\n\r", (*conv_pt)-27300);
				//if (i == )
				CalibrationTable.Workload[i] = ( (numValue) *conv_pt ) / 10000.0;
			}

			SPI_AsyncRead(SPI_bits, rxData );
			xSemaphoreTake(gSemSPI_read, portMAX_DELAY);

			uint16_t* conv_pt = rxData + 2;
			//printf("%hu\n\r", (*conv_pt)-27300);
			//if (i == )
			CalibrationTable.TotalPowerBudget = ( (numValue) *conv_pt ) / 1000.0;
		}


		xSemaphoreGive(gSemSPI_HR);
		#endif //spi

		/*-----------------------------------------------*/
		/***************** 4: Write Data *****************/
		/*-----------------------------------------------*/

		/*** 4.1: Write Calibration Table ***/
		if (lSTARTWriteCalibrationTable()) //TODO:
		{
			gCalibrationTable = CalibrationTable;

			lENDWriteCalibrationTable(); //TODO:
		}

		/*** 4.2: Write Data to the OS Memeory ***/


		/* taskYIELD is used in case of Cooperative Scheduling, to allow/force Contex Switches */
		#if configUSE_PREEMPTION == 0
       		taskYIELD();
    	#endif
	}

	/* Cannot and Shouldn't reach this point, but If so... */
	// TODO: SIGNAL HAZARDOUS ERROR, FAILURE!
	vTaskDelete( NULL );
}

void vBMC( void *parameters ) {

	/* Description */
	/* This task is responsable of dialoguing with the BMC task of the EPI core of the other socket (same MB)
	* This task will be activated by a notification sent by TControl that will check the necessity of trasmission
	* through a polling (500 ms of period)
	*/

	/* Task Code */
	for (;;)
	{

		ulTaskNotifyTake( pdTRUE, 	// xClearCountOnExit: if pdTRUE = Binary Semaphore; if number = Counting Semaphore (# of times to be called before exit).
					portMAX_DELAY); // xTicksToWait

		#ifdef GPIO_TEST
		//pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 1);
		#endif

		/* Do BMC STUFF */

		/* taskYIELD is used in case of Cooperative Scheduling, to allow/force Contex Switches */
		#if configUSE_PREEMPTION == 0
	   		taskYIELD();
		#endif
	}

	/* Cannot and Shouldn't reach this point, but If so... */
	// TODO: SIGNAL HAZARDOUS ERROR, FAILURE!
	vTaskDelete( NULL );
}



void vInitializationTask( void *parameters){

	/* Initialization */

	#ifdef MEASURE_ACTIVE
	measureInitializeNStart();
	CSCheck = 1;
	lPerformanceCheck = 0;
	#endif

	/** GPIO Initialization **/
	#ifdef GPIO_TEST
	//pin: 9
	pi_gpio_pin_configure(NULL, PI_GPIO_A19_PAD_33_B12, PI_GPIO_OUTPUT);
	pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 0);
	//pin: 6
	pi_gpio_pin_configure(NULL, PI_GPIO_A17_PAD_31_B11, PI_GPIO_OUTPUT);
	pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 0);
	#endif

	/** SPI Initialization **/
	#ifdef SPI_TEST
	vSPI_Init();

	//Mutex cannot be used in ISR!
	//Semaphores are created empty
	if ( (gSemSPI_write = xSemaphoreCreateBinary()) == NULL )
	{
		#ifdef DEBUG_ACTIVE
			printf("gSemSPI_write Binary is NULL!\n");
		#endif

		//TODO: do something

		pmsis_exit(-10);
	}

	if ( (gSemSPI_read = xSemaphoreCreateBinary()) == NULL )
	{
		#ifdef DEBUG_ACTIVE
			printf("gSemSPI_read Binary is NULL!\n");
		#endif

		//TODO: do something

		pmsis_exit(-10);
	}

	if ( (gSemSPI_HR = xSemaphoreCreateMutex()) == NULL )
	{
		#ifdef DEBUG_ACTIVE
			printf("gSemSPI_HR Mutex is NULL!\n");
		#endif

		//TODO: do something

		exit(-10);
	}

	#endif


	/** Create Mutexes (semaphores) **/
	//No need to "Give" Semaphore after creation.
	if ( (gSemAccumulationData = xSemaphoreCreateMutex()) == NULL )
	{
		#ifdef DEBUG_ACTIVE
			printf("SemAccumulationData Mutex is NULL!\n");
		#endif

		//TODO: do something

		exit(-10);
	}
	if ( (gSemCalibrationTable = xSemaphoreCreateMutex()) == NULL )
	{
		#ifdef DEBUG_ACTIVE
			printf("SemCalibrationTable Mutex is NULL!\n");
		#endif

		//TODO: do something

		exit(-10);
	}
	#ifdef FREQUENCY_REDUCTION_MAP
	if ( (gSemReductionMap = xSemaphoreCreateMutex()) == NULL )
	{
		#ifdef DEBUG_ACTIVE
			printf("SemReductionMap Mutex is NULL!\n");
		#endif

		//TODO: do something

		exit(-10);
	}
	#endif

	//to Set PID Parameters
	if(!lEPImathPIDsetParameters(INIT_PID_KP, INIT_PID_KI, INIT_PID_KD, INIT_PID_DT,
						EPI_CORE_CRITICAL_TEMPERATURE, EPI_CORE_TEMPERATURE_MARGIN, EPI_CORE_IDLE_POWER) ) // TOCHECK: is it proper? How should I do?
	{
		#ifdef DEBUG_ACTIVE
			printf("Initialization Error\n");
		#endif

		//TODO: do something

		return 0;
	}

	if( xTaskCreate(
		vPeriodicControl,
		"Periodic Control",
		(configMINIMAL_STACK_SIZE*2 + 2400), //TBD
		NULL,
		PERIODIC_CONTROL_PRIORITY,
		&taskHandles[0]
		) != pdPASS )
	{
		#ifdef DEBUG_ACTIVE
			printf("Periodic Task is NULL!\n");
		#endif

		//TODO: do something

		pmsis_exit(-5);
	}

	if( xTaskCreate(
		vTaskOS,
		"TaskOS",
		(configMINIMAL_STACK_SIZE*2 + 100), //TBD
		NULL,
		TASK_OS_PRIORITY,
		&taskHandles[1]
		) != pdPASS )
	{
		#ifdef DEBUG_ACTIVE
			printf("Task OS is NULL!\n");
		#endif

		//TODO: do something

		pmsis_exit(-5);
	}

	#ifdef MEASURE_ACTIVE
	if( xTaskCreate(
		vPrintTask,
		"PrintTask",
		configMINIMAL_STACK_SIZE * 2, //TBD
		NULL,
		PRINT_TASK_PRIORITY,
		&taskHandles[3]
		) != pdPASS )
	{
		#ifdef DEBUG_ACTIVE
			printf("Task Print is NULL!\n");
		#endif

		//TODO: do something

		pmsis_exit(-5);
	}
	#endif


	// Variables Initialization
	for (char i = 0; i < N_EPI_CORE; i++)
	{
		gCalibrationTable.TargetFreq[i] 			= INIT_EPI_CORE_FREQ;
		gCalibrationTable.Workload[i]				= INIT_EPI_CORE_WORKLOAD;
		#ifdef EPI_CONFIG_FREQ_BINDING
			gCalibrationTable.CoreBindingVector[i] 	= i%8; //TDB:
		#endif
	}

	//numValue app[LMS_COEFF_NUMBER] = {10, -1, -3, 20}{4.5139e-10, -3.1343e-19, -3.1343e-17, 2.8715e-12}; //TODO: change how to set this parameters
	numValue app[LMS_COEFF_NUMBER] = {4.5139e-10, -3.1343e-19, -3.1343e-17, 2.8715e-12}; //TODO: change how to set this parameters
	for (char i = 0; i < LMS_COEFF_NUMBER; i++)
		gCalibrationTable.PowerFormulaCoeff[i] 		= app[i]; //INIT_POWER_FORMULA_COEFF[i]; //TODO:
	gCalibrationTable.TotalPowerBudget 				= INIT_EPI_TOTAL_POWER_BUDGET;
	//TODO:
	/*#ifdef EPI_CONFIG_QUADRANTS
		float QuadrantPowerBudget[EPI_N_QUADRANTS];
	#endif
	*/


	// "Start" Timer Interrupt
    uint32_t time_us = (uint32_t) TASKS_CONTROL_PERIOD_US;  /* Timer value to be compared here. */
    pi_timer_irq_set(FC_TIMER_1, (time_us), 0);


	/* Task Code */

	// AutoSuspend //TBD
	//non posso o non funziona niente
	//vTaskSuspend(NULL); //taskSuspend or TaskDelete?

	//TBD: for Security
	vTaskPrioritySet(NULL, (UBaseType_t)tskIDLE_PRIORITY + 1 );

	for(;;)
	{
		while( ( eTaskGetState(taskHandles[0]) != eSuspended ) ||
	           ( eTaskGetState(taskHandles[1]) != eSuspended ) )
	    {
			//GPIO_A17_toggle();
			//pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 0);
			//slack time here
	        pi_yield();
	    }

		//Shoudn't reach here
		//TODO: signal error
	}

	/* Cannot and Shouldn't reach this point, but If so... */
	// TODO: SIGNAL HAZARDOUS ERROR, FAILURE!
	vTaskDelete( NULL );

	pmsis_exit(-9);
}

#ifdef MEASURE_ACTIVE
void vPrintTask (void* parameters){

	/* Description */
	/* This task is used to print the measure system
	*/
	Timer_Data_t timer;
	timer.where = 'L';

	volatile int wait = 0; //to avoid compiling optimization which deleted code
	int cycles = 0;

	/* Initialization */

	/* Task Code */
	for (;;)
	{
		if (lPerformanceCheck)
		{
			printf("Done\n");

			for(int p=0; p<(15*MEASURE_N_ITERATION+7); p++)
			{
				if (timerBuffer[p].where != '0')
				{
					if (timerBuffer[p].where == 'I')
						printf("\n");

					printf("Timer: %d, Where: %c, value: %d\n", p, timerBuffer[p].where, timerBuffer[p].cycle);
				}

				if (timerBuffer[p].where == 'F')
					break;
			}
			printf("End.\n");
			vTaskSuspend(NULL);
		}
		else
		{
			//printf("Entering the Else TPM Task\n");
			if (CSCheck)
			{
				timerBuffer[Timerindex++] = (Timer_Data_t) {'L', measureReadCycle()};
				CSCheck = 0;
			}
		}

	/* taskYIELD is used in case of Cooperative Scheduling, to allow/force Contex Switches */
	#if configUSE_PREEMPTION == 0
   		taskYIELD();
	#endif
	}

	/* Cannot and Shouldn't reach this point, but If so... */
	// TODO: SIGNAL HAZARDOUS ERROR, FAILURE!
	vTaskDelete( NULL );
}
#endif



/*-----------------------------------------------*/
/******************* Functions *******************/
/*-----------------------------------------------*/

numValue lCorePowerComputation(numValue *oCoreTargetPower, parameter_table_t *iParameterTable, char* FreqReductionMap){

	numValue TotalPower = 0;

	//TBD: here we can also pass the pointer to the array and call the function only 1 time. The function will do for and sum.
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
	{
		oCoreTargetPower[i] = lEPImathPowerCompute(iParameterTable->TargetFreq[i], iParameterTable->Workload[i], iParameterTable->PowerFormulaCoeff);
		#ifdef USE_TESTS_ON_NUMBERS
			configASSERT(oCoreTargetPower[i] > 0)
		#endif
		//TODO: for quadrants, we should change this to reflect quadrants disposition so we can add quadrant power also inside this for
		//printf("CtP r: %d\n", (int)oCoreTargetPower[i]);
		//Saturation check for single Core PowerMax Margin
		if (oCoreTargetPower[i] > EPI_CORE_MAX_POWER_SINGLE)
		{
			oCoreTargetPower[i] = EPI_CORE_MAX_POWER_SINGLE;

			//Bitmap of Frequency Reduction.
			#ifdef FREQUENCY_REDUCTION_MAP
			FreqReductionMap[i] |= BM_MAX_SINGLE_POW_SAT;
			#endif
		}
		TotalPower += oCoreTargetPower[i];
	}

	//printFloat(oCoreTargetPower[0]);

	return TotalPower;
}

numValue lAlphaPowerReduction(numValue *oCoreReducedPower, numValue *iCoreTargetPower, numValue *iMeasuredTemperature, numValue iDeltaPower, char* FreqReductionMap){

	numValue TotalEpiPower = 0;
	numValue Alpha[N_EPI_CORE];
	numValue TotalAlpha = 0;

	numValue TotalPower = 0;

	//Compute Alpha
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
	{
		#ifdef USE_TESTS_ON_NUMBERS
			configASSERT((EPI_CORE_CRITICAL_TEMPERATURE - iMeasuredTemperature[i]) > 0)
		#endif
		Alpha[i] = 1 / (EPI_CORE_CRITICAL_TEMPERATURE - iMeasuredTemperature[i]);
		#ifdef USE_TESTS_ON_NUMBERS
			configASSERT( (Alpha[i] > 0) && (Alpha[i] <= 1))
		#endif
		//TODO: Failed because Alpha is an Int.

		TotalAlpha += Alpha[i];
		//TODO: for quadrants, we should change this to reflect quadrants disposition so we can add quadrant power also inside this for
	}

	//Normalize Alpha
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
	{
		// Alpha Normalized. For optimization we use Alpha[i]
		Alpha[i] = Alpha[i] / TotalAlpha;
		#ifdef USE_TESTS_ON_NUMBERS
			configASSERT( (Alpha[i] > 0) && (Alpha[i] <= 1))
		#endif
		// Here Christian added this:
		/*
			 % saturation of reduction up to PT (or PT-PTmin if no switch off)
			 for k=1:length(PT)
				 %
				if Delta*alpha_norm(k)>PT(k)
				 alpha_norm(k)=PT(k)/Delta;
				end
			 %
			 end
		*/
	}

	//Apply Alpha Reduction
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
	{
		// Updated CoreTargetPower
		oCoreReducedPower[i] = iCoreTargetPower[i] - (Alpha[i] * iDeltaPower);
		#ifdef USE_TESTS_ON_NUMBERS
			configASSERT(oCoreReducedPower[i] > 0)
		#endif

		#ifdef FREQUENCY_REDUCTION_MAP
			FreqReductionMap[i] |= BM_ALPHA_RED;
		#endif

		//TODO: if no bindings (def) we should compute here the new TotalEpiPower += CoreTargetPower[i];, otherwise do after bindings
		#ifndef EPI_CONFIG_FREQ_BINDING //TODO: remove this and remove the return.
			TotalPower += oCoreReducedPower[i];
		#endif
	}

	return TotalPower;
}

#ifdef EPI_CONFIG_FREQ_BINDING
void vCoreBindingReduction(numValue *oArray, char *iCoreBindingVector, char* FreqReductionMap){

	// The Way I thought about the binding, to save both cycles and memeory is that the EPI OS give us information about the parallelization
	//  Inside a vector of dimension N_EPI_CORE. Each core that needs to be binded with another one has the same number (from 1 to (N_EPI_CORE/2)),
	//  0 if there is no binding. We scan the array looking for the minimum value per each group with the same number, and then
	//  we do a second scan by applying this minimum value.

	numValue MinGroupFreq[(N_EPI_CORE / 2)];

	//TODO: Optimize This
	//Initialization
	for (int i = 0; i < (N_EPI_CORE / 2); i++ )
		MinGroupFreq[i] = 100; //TODO: choose a proper number!

	// Scan
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
		if (iCoreBindingVector[i] != 0)
		{
			#ifdef USE_TESTS_ON_NUMBERS
				configASSERT((iCoreBindingVector[i] < (N_EPI_CORE/2)) && (iCoreBindingVector[i] > 0) )
			#endif
			if ((oArray[i] < MinGroupFreq[iCoreBindingVector[i]]))
			{
				MinGroupFreq[iCoreBindingVector[i]] = oArray[i];
				#ifdef USE_TESTS_ON_NUMBERS
					configASSERT(MinGroupFreq[iCoreBindingVector[i]] < 0.1) //TODO: put the number above.
				#endif
			}
		}

	// Apply
	for (uint8_t i = 0; i < N_EPI_CORE; i++)
		if (iCoreBindingVector[i] != 0)
		{
			oArray[i] = MinGroupFreq[iCoreBindingVector[i]];

			#ifdef FREQUENCY_REDUCTION_MAP
			FreqReductionMap[i] |= BM_BINDING_RED;
			#endif
		}

	// Christian Bindings: To check if J starts from 1 or from i.
	// the problem I thought about transitivity (A linked with B and B linked with C, so we also have to link A with C) is not present
	// since in the matrix also A is linked with C.
	//TODO: Evaluate which is more optimal
	/*
		% %
		 for i=1:length(PT)
		     for j=1:length(PT)
		         if B_mat(i,j)==1 && abs(i-j)>0
		            PT_temp(i)=min(PT_temp(i),PT_temp(j));
		            PT_temp(j)=min(PT_temp(i),PT_temp(j));
		         end
		     end
		 end
 	*/
}
#endif

/*-----------------------------------------------*/
/********************* I/Os **********************/
/*-----------------------------------------------*/

//TBD: there are 2 possibilities: create only 1 funct and pass void* and the sem, or create one for each Semaphore
// The first save code space and is more elegant, the second should be more secure
int lReadCalibrationTable(parameter_table_t* oInternalCalibrationTable){

	// Take the Mutex
	if ( xSemaphoreTake( gSemCalibrationTable, 1 ) == pdFALSE ) //TBD: xTicksToWait
	{
		// Mutex not taken in time
		#ifdef DEBUG_ACTIVE
			printf("Calibr Read Mutex is not taken in time!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
	{
		// Copy the Data
		*oInternalCalibrationTable = gCalibrationTable;

		// Release the Mutex
		if ( xSemaphoreGive( gSemCalibrationTable ) == pdFALSE )
		{
			/* An error occurred. Semaphores are implemented using queues.
			* An error can occur if there is no space on the queue to post a message
			* indicating that the semaphore was not first obtained correctly.
			*/
			// Mutex not Released Correctly
			#ifdef DEBUG_ACTIVE
				printf("Calibr Read Mutex is not Released Correctly!\n");
			#endif

			//TODO: do something

			return 0;
		}
		else
			return 1;
	}
}
int lSTARTWriteCalibrationTable(void){

	// Take the Mutex
	if ( xSemaphoreTake( gSemCalibrationTable, 1 ) == pdFALSE ) //TBD: xTicksToWait
	{
		// Mutex not taken in time
		#ifdef DEBUG_ACTIVE
			printf("Calibr Write Mutex is not taken in time!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
		return 1;
}
int lENDWriteCalibrationTable(void){

	// Release the Mutex
	if ( xSemaphoreGive( gSemCalibrationTable ) == pdFALSE )
	{
		/* An error occurred. Semaphores are implemented using queues.
		* An error can occur if there is no space on the queue to post a message
		* indicating that the semaphore was not first obtained correctly.
		*/
		// Mutex not Released Correctly
		#ifdef DEBUG_ACTIVE
			printf("Calibr Write Mutex is not Released Correctly!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
		return 1;
}
int lReadAccumulationData2(telemetry_t* oInternalAccumulationData){

	// Take the Mutex
	if ( xSemaphoreTake( gSemAccumulationData, 1 ) == pdFALSE ) //TBD: xTicksToWait
	{
		// Mutex not taken in time
		#ifdef DEBUG_ACTIVE
			printf("Accum Read Mutex is not taken in time!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
	{
		// Copy the Data
		*oInternalAccumulationData = gAccumulationData;

		// Release the Mutex
		if ( xSemaphoreGive( gSemAccumulationData ) == pdFALSE )
		{
			/* An error occurred. Semaphores are implemented using queues.
			* An error can occur if there is no space on the queue to post a message
			* indicating that the semaphore was not first obtained correctly.
			*/
			// Mutex not Released Correctly
			#ifdef DEBUG_ACTIVE
				printf("Accum Read Mutex is not Released Correctly!\n");
			#endif

			//TODO: do something

			return 0;
		}
		else
			return 1;
	}
}
int lSTARTWriteAccumulationData(void){

	// Take the Mutex
	if ( xSemaphoreTake( gSemAccumulationData, 1 ) == pdFALSE ) //TBD: xTicksToWait
	{
		// Mutex not taken in time
		#ifdef DEBUG_ACTIVE
			printf("Accum Write Mutex is not taken in time!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
		return 1;
}
int lENDWriteAccumulationData(void){

	// Release the Mutex
	if ( xSemaphoreGive( gSemAccumulationData ) == pdFALSE )
	{
		/* An error occurred. Semaphores are implemented using queues.
		* An error can occur if there is no space on the queue to post a message
		* indicating that the semaphore was not first obtained correctly.
		*/
		// Mutex not Released Correctly
		#ifdef DEBUG_ACTIVE
			printf("Accum Write Mutex is not Released Correctly!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
		return 1;
}

#ifdef FREQUENCY_REDUCTION_MAP
int lReadReductionMap(char* oReductionMap){

	// Take the Mutex
	if ( xSemaphoreTake( gSemReductionMap, 1 ) == pdFALSE ) //TBD: xTicksToWait
	{
		// Mutex not taken in time
		#ifdef DEBUG_ACTIVE
			printf("RedMap Read Mutex is not taken in time!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
	{
		// Copy the Data
		*oReductionMap = gFreqReductionMap; //TBC: warning: assignment makes integer from pointer without a cast

		// Release the Mutex
		if ( xSemaphoreGive( gSemReductionMap ) == pdFALSE )
		{
			/* An error occurred. Semaphores are implemented using queues.
			* An error can occur if there is no space on the queue to post a message
			* indicating that the semaphore was not first obtained correctly.
			*/
			// Mutex not Released Correctly
			#ifdef DEBUG_ACTIVE
				printf("RedMap Read Mutex is not Released Correctly!\n");
			#endif

			//TODO: do something

			return 0;
		}
		else
			return 1;
	}
}
int lSTARTWriteReductionMap(void){

	// Take the Mutex
	if ( xSemaphoreTake( gSemReductionMap, 1 ) == pdFALSE ) //TBD: xTicksToWait
	{
		// Mutex not taken in time
		#ifdef DEBUG_ACTIVE
			printf("RedMap Write Mutex is not taken in time!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
		return 1;
}
int lENDWriteReductionMap(void){

	// Release the Mutex
	if ( xSemaphoreGive( gSemReductionMap ) == pdFALSE )
	{
		/* An error occurred. Semaphores are implemented using queues.
		* An error can occur if there is no space on the queue to post a message
		* indicating that the semaphore was not first obtained correctly.
		*/
		// Mutex not Released Correctly
		#ifdef DEBUG_ACTIVE
			printf("RedMap Write Mutex is not Released Correctly!\n");
		#endif

		//TODO: do something

		return 0;
	}
	else
		return 1;
}
#endif //#ifdef FREQUENCY_REDUCTION_MAP
//
