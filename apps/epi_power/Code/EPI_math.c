// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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
#include "EPI_types.h"
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
	if ( (abs(error) - _TcritPID) > 0)
	{
		#ifdef ERROR_MAP
		ErrorMap |= BM_ERROR_NUMBERS_VALUE;
		#endif

		//error = ?? //TODO
	}
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

		//TODO: this saturation is problematic because it depends on the Target power.
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

	/* Do this directly is the main to avoid also var assage bug
	#ifdef USE_TESTS_ON_NUMBERS
	if ( (outputPID <= 0) || (outputPID > iTargetPower) )
	{
		#ifdef ERROR_MAP
		ErrorMap |= BM_ERROR_NUMBERS_VALUE;
		#endif

		outputPID = EPI_CORE_IDLE_POWER; //TBD: maybe too much conservative in case > ?? (consider still there is an error!)
	}
	#endif
	*/
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

	//return ( ((iCoreTargetPower - (Icc*V_Fixed)) / iWorkload / (V_Fixed*V_Fixed)) /* *1000000000.0*/);
	float step = 10.0;

	numValue Frequency = ( ((iCoreTargetPower - (Icc*V_Fixed)) / iWorkload / (V_Fixed*V_Fixed)));
	Frequency *= step;
	uint32_t StepFreq = (uint32_t) Frequency;

	return ((float) StepFreq / step);


}


numValue lEPImathLMSrecursive(numValue *oParam, numValue *oP_curr, numValue *iPrevParam, numValue *iP_prev, numValue *iInput, numValue iError, numValue iLambda){

	// p_est = p_est(prev) + K*error;
	// error = measured_power - power_computed_with_prev_paramenters;
	// k = P*[V, VVF]
	// P = P(prev)/lambda/(t-1) - P(prev)*[V, VVF]*[V, VVF]T*P(prev)/(t-1)/(lambda*(lambda*(t-1) + [V, VVF]T*P(prev)*[V, VVF])))


	float T2[LMS_COEFF_NUMBER];
	float T3[LMS_COEFF_NUMBER*LMS_COEFF_NUMBER];
	float T4[LMS_COEFF_NUMBER*LMS_COEFF_NUMBER];
	float K[LMS_COEFF_NUMBER];
	//float coeff1 = (iPass/(iPass-1))/iLambda;
	float coeff2 = 0;

	MatMul(T2, iP_prev, iInput, LMS_COEFF_NUMBER, LMS_COEFF_NUMBER, 1);
	MatMul(T3, T2, iInput, LMS_COEFF_NUMBER, 1, LMS_COEFF_NUMBER); //since the vector is Nx1, no need to do the Transpose since the Matrix is implemented as an array
	MatMul(T4, T3, iP_prev, LMS_COEFF_NUMBER, LMS_COEFF_NUMBER, LMS_COEFF_NUMBER);

	// is a number, maybe is worth to create its own function
	//MatMul(coeff2, iInput, T2, 1, iN, 1); //since the vector is Nx1, no need to do the Transpose since the Matrix is implemented as an array
	for (uint8_t i=0; i < LMS_COEFF_NUMBER; i++)
	{
		coeff2 += iInput[i] * T2[i];
	}

	coeff2 = (coeff2 + iLambda) * iLambda;

	// MatSum, but optimized!
	for (uint8_t i=0; i < LMS_COEFF_NUMBER*LMS_COEFF_NUMBER; i++)
	{
		oP_curr[i] = (iP_prev[i]/iLambda - T4[i]/coeff2);
	}

	MatMul(K, oP_curr, iInput, LMS_COEFF_NUMBER, LMS_COEFF_NUMBER, 1);

	// MatSum, but optimized!
	for (uint8_t i=0; i < LMS_COEFF_NUMBER*LMS_COEFF_NUMBER; i++)
	{
		oParam[i] = iPrevParam[i] + K[i]*iError;
	}


	return 0.0; //here you should return maybe diff between parameters? a value for errors?
}

void lEPImathLMSstoreP (numValue *oParam, numValue *iInput)
{
	float T2[LMS_COEFF_NUMBER];

	MatMul(T2, iInput, iInput, LMS_COEFF_NUMBER, 1, LMS_COEFF_NUMBER);

	for (uint8_t i=0; i < LMS_COEFF_NUMBER*LMS_COEFF_NUMBER; i++)
		oParam[i] += T2[i];

	return;
}



void MatMul(numValue *O, numValue *iA, numValue *iB, int r1, int col1, int col2)
{
	int r2 = col1;
	float sum = 0;

	for (uint8_t i = 0; i < r1; i++)
	  for (uint8_t j = 0; j < col2; j++)
	  {
		for (uint8_t k = 0; k < r2; k++)
		{
		  sum += iA[i*col1 + k] * iB[k*col2 + j];
		}

		O[i*col2 + j] = sum;
		sum = 0.0;
	  }

	 return;
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
