
/***************************************************/
/*
* Math and Control Functions Library
*
*
*/


/* Header include. */
#include "EPI_math.h"
#ifdef FLOAT_NUMBERS
#include "print_float.h"
#endif

/* Other Inclusion */
#include "EPI_Config.h"
//#include "FreeRTOS_util.h" //TODO: ??? maybe common instead of this.
#include "pmsis.h"

#ifdef MEASURE_ACTIVE
	#include "measure.h"
#endif

const numValue V_Fixed = 0.85;
const numValue Icc = 0.1333333;

/* Global Variables Declaration */
// Keep Global variables as few as possible, Higher cost
//TODO: Proper allocation in the right memory.
#ifdef PID_INTEGRAL_ACTION
	static numValue _PIDintegralError[N_EPI_CORE] = {0};
	static numValue  _ki;
#endif
#ifdef PID_DERIVATIVE_ACTION
	static numValue _PIDpreviousError[N_EPI_CORE] = {0};
	static numValue _kd;
#endif
#ifdef PID_SATURATION
	static numValue _SaturationMax;
	static numValue _Pidle;
#endif
static numValue _kp, _TcritPID;  //TODO: initialization of all var


numValue lEPImathPIDcompute(numValue iTargetPower, numValue iMeasuredTemperature, char iCoreNumber) {

	numValue outputPID = 0;

	/* Compute Error */
	numValue error = _TcritPID - iMeasuredTemperature;
	#ifdef USE_TESTS_ON_NUMBERS
		configASSERT( (abs(error) - _TcritPID) < 0)
	#endif
	//printf("er: ");
	//printFloat(error);
	/* Proportional Term */
	outputPID = _kp * error;

	/* Integral Term */
	#ifdef PID_INTEGRAL_ACTION
		_PIDintegralError[iCoreNumber] += error * _ki;
		// if (iCoreNumber == 0)
		// {
		// 	printf("In: ");
		// 	printFloat(_PIDintegralError[iCoreNumber]);
		// 	printFloat(iTargetPower);
		// }

		#ifdef PID_ANTI_WINDUP_SAT
			if (_PIDintegralError[iCoreNumber] > 0.4 ) //TBD: Should I parametrize this 0?
				_PIDintegralError[iCoreNumber] = 0.4;
			else if (_PIDintegralError[iCoreNumber] < (PID_ANTI_WINDUP_SAT_COEFF * iTargetPower)) //TODO: var conversion
			{
				_PIDintegralError[iCoreNumber] = PID_ANTI_WINDUP_SAT_COEFF * iTargetPower; //TODO: var conversion
			}
		#endif
		outputPID += _PIDintegralError[iCoreNumber];
	#endif

	/* Derivative Term */
	#ifdef PID_DERIVATIVE_ACTION
		outputPID += _kd * (error - _PIDpreviousError[iCoreNumber]);

		/* Update Error Global Variable */
		_PIDpreviousError[iCoreNumber] = error;
	#endif

	/* Saturation */
	#ifdef PID_SATURATION
		if (outputPID > _SaturationMax)
			outputPID = _SaturationMax;
		else if (outputPID < (-(iTargetPower - _Pidle)) )
			outputPID = -(iTargetPower - _Pidle);
	#endif

	//printf("outpid: %d, itargetpow: %d\n", (int)outputPID, (int)iTargetPower);
	#ifdef PID_FF_ACTION //TBC: Since our implementation of the PID, maybe is better to not make this definable, but to FORCE it active.
		outputPID = iTargetPower + outputPID;
		// if (iCoreNumber == 0)
		// {
		// 	printf("ou: ");
		// 	printFloat(outputPID);
		// }
	#endif

	#ifdef USE_TESTS_ON_NUMBERS
		configASSERT( (outputPID > 0) && (outputPID <= iTargetPower) )
	#endif
	return outputPID;
}

numValue lEPImathPIDsetParameters(numValue kp, numValue ki, numValue kd, numValue dt, numValue Tcrit, numValue Tmargin, numValue Pidle){ //TBD: better to pass by copy or by reference?

	/* Start BARRIER */ //TODO: implement barrier

	_kp = kp*dt; //TBC
	#ifdef PID_INTEGRAL_ACTION
		_ki = ki*dt;
	#endif
	#ifdef PID_DERIVATIVE_ACTION
		_kd = kd/dt; //TBC
	#endif

	_TcritPID = Tcrit - Tmargin;

	#ifdef PID_SATURATION
		_SaturationMax = 0; //TODO
		_Pidle = Pidle;
	#endif

	/* End BARRIER */


	//TODO: check, if everything is ok then
	return 1;// TBD: void or boolean?

}



numValue lEPImathPowerCompute(numValue iTargetFrequency, numValue iWorkload, numValue *iFormulaCoeff){ //TBD: better to pass by copy or by reference? (same for output)

	/* Formula: P = K1*f*V^2 + K2 + (K3 + K4*f)*w^K5 */

	//return (iFormulaCoeff[0] * iTargetFrequency * (V_Fixed*V_Fixed) ) + iFormulaCoeff[1] +
	//		( (iFormulaCoeff[2] + (iFormulaCoeff[3] * iTargetFrequency)) * iWorkload ); //TODO: Formula miss k5
			//Same formula as Christian. checked.

	//printf("pc: " );
	//printFloat((Icc + (iWorkload * (iTargetFrequency /*/ 1000000000.0*/) * V_Fixed) ) * V_Fixed );
	return ( (Icc + (iWorkload * (iTargetFrequency /*/ 1000000000.0*/) * V_Fixed) ) * V_Fixed );

	/* Formula:
	* P = Pd + Ps
	* Pd = Ceff*f*Vdd^2
	* Ceff = [Weights]T * [PerfCounters]
	*/

	//Simulation //TODO: Change Formula
	//Assume Vdd = 10 fixed
	//return iFormulaCoeff[0] * iTargetFrequency * (10 * 10);

}

numValue lEPImathFrequencyCompute(numValue iCoreTargetPower, numValue iWorkload, numValue *iFormulaCoeff) {

	//TBC: Check formula. Miss (^k5) in the end
	/*return (iCoreTargetPower - iFormulaCoeff[1] - iFormulaCoeff[2] * iWorkload) /
								(V_Fixed * V_Fixed * iFormulaCoeff[0] + iFormulaCoeff[3] * iWorkload);
	*/

	return ( ((iCoreTargetPower - (Icc*V_Fixed)) / iWorkload / (V_Fixed*V_Fixed)) /* *1000000000.0*/);
}


numValue lEPImathLMSrecursive(numValue iError, numValue iInput, numValue iMu){

	return ( iMu * iError * iInput );

}

#ifdef SPI_TEST
void vEPImathResetPID(void) {

	#ifdef PID_INTEGRAL_ACTION
	for (int i = 0; i < N_EPI_CORE; i++)
		_PIDintegralError[i] = 0;
	#endif
	#ifdef PID_DERIVATIVE_ACTION
	for (int i = 0; i < N_EPI_CORE; i++)
		_PIDpreviousError[i] = 0;
	#endif

	return;
}
#endif
