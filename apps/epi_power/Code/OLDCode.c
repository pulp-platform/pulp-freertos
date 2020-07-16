//printf("Core TP: %d\n", (int)CoreTargetPower[0]);
//printf("Core TP: "); printFloat(CoreTargetPower[0]);
//printf("Total 2: %d \n", (int)printTOT);
//printf("Total 2: "); printFloat(printTOT);


/*****************/
/******************************************************/
/* TRY SPI HERE */
/******************************************************/
/*****************/

// Code Example
spi_master_config_t  iMaster_config;
uint32_t bits = 32;
uint32_t mode = 0;
uint32_t hz;

SPI_MasterGetDefaultConfig(&iMaster_config);

iMaster_config.whichCsn                                 = uSPI_csn1;

iMaster_config.bitsPerFrame = ( uint32_t ) bits;
iMaster_config.cpol = (mode & 0x2) ? uSPI_ClockPolarityActiveLow : uSPI_ClockPolarityActiveHigh;
iMaster_config.cpha = (mode & 0x1) ? uSPI_ClockPhaseSecondEdge : uSPI_ClockPhaseFirstEdge;

/*
masterConfig.baudRate                      = 1000000;
masterConfig.bitsPerFrame                  = 8;
masterConfig.cpol                          = uSPI_ClockPolarityActiveHigh;
masterConfig.cpha                          = uSPI_ClockPhaseFirstEdge;
masterConfig.direction                     = uSPI_MsbFirst;
masterConfig.qpi                                      = uSPI_Single;

// OR: void SPI_MasterGetDefaultConfig(spi_master_config_t *masterConfig);

/* (SPIM_Type *base, spi_master_config_t *masterConfig, uint32_t srcClock_Hz)
* @param base SPI peripheral address.
* @param masterConfig Pointer to the structure spi_master_config_t.
* @param srcClock_Hz Module source input clock in Hertz.
*/

//SPI_MasterGetDefaultConfig(&masterConfig);

//SPIM_Type *base;
/*
 * @brief Completion callback function pointer type.
 *
 * @param base SPI peripheral address.
 * @param handle Pointer to the handle for the SPI master.
 * @param status Success or error code describing whether the transfer completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 *
 * typedef void (*spi_master_transfer_callback_t)(SPIM_Type *base,
										   spi_master_handle_t *handle,
										   status_t status,
										   void *userData);
*/

SPI_MasterInit( SPIM1 /* = spi_address[iMaster_config.whichCsn]*/, &iMaster_config, SystemCoreClock );

iMaster_config.clkDiv = ( SystemCoreClock >> 1) / hz;
SPI_MasterFrequencyConfig( SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,  &iMaster_config );

/*This function set SPI chip select.
 *
 * @param base SPI peripheral address.
 * @param whichCsn which Chip select.
 * @param status   Chip select status.
*/
//SPI_Master_CS(base, whichCsn, status);

//spi_flash_io.c Example

/*
* @brief Writes data into the data buffer master mode and waits till complete to return.
*
* In master mode, the 16-bit data is appended to the 16-bit command info. The command portion
*
* @param base SPI peripheral address.
* @param tx_buffer Udma tx buffer base address.
* @param tx_length Udma tx buffer size.
* @param rx_buffer Udma rx buffer base address.
* @param rx_length Udma rx buffer size.
* @param bit_width Udma transfer bit width.
*/

const uint32_t tx_buffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int rx_buffer = 0;

uint32_t SPI_CommandSeq[6];
spi_command_sequence_t cmd_SPI_CommandSeq;

/*#define*/ int DUMMY = 15;

/* Dummy cycles */
{
	/* these are in GAP8.h */
	SPI_CommandSeq[0] = SPIM_CMD_SOT( iMaster_config.whichCsn );
	SPI_MasterTransferBlocking( SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,
								( uint32_t * ) SPI_CommandSeq,
								sizeof( uint32_t ), NULL, 0, 32 );

	SPI_CommandSeq[0] = SPIM_CMD_SEND_CMD( 0x06, 8, uSPI_Single );
	SPI_MasterTransferBlocking( SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,
								( uint32_t * ) SPI_CommandSeq,
								sizeof( uint32_t ), NULL, 0, 32 );

	SPI_CommandSeq[0] = SPIM_CMD_EOT( 0 );
	SPI_MasterTransferBlocking( SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,
								( uint32_t * ) SPI_CommandSeq,
								sizeof( uint32_t ), NULL, 0, 32 );

	memset( &cmd_SPI_CommandSeq, 0, sizeof(spi_command_sequence_t) );
	cmd_SPI_CommandSeq.cmd       = 0x71;
	cmd_SPI_CommandSeq.cmd_bits  = 8;
	cmd_SPI_CommandSeq.addr_bits = 32;
	cmd_SPI_CommandSeq.addr      = ( 0x80000380 | DUMMY );
	cmd_SPI_CommandSeq.cmd_mode  = uSPI_Single;
	cmd_SPI_CommandSeq.addr_mode = uSPI_Single;
	cmd_SPI_CommandSeq.csn = iMaster_config.whichCsn;
	SPI_MasterTransferCommandSequence( SPIM1 /* = spi_address[iMaster_config.whichCsn]*/, &cmd_SPI_CommandSeq );
}

/* send something */
{
	SPI_CommandSeq[0] = SPIM_CMD_SOT( iMaster_config.whichCsn );
	SPI_MasterTransferBlocking(  SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,
								( uint32_t * ) SPI_CommandSeq,
								sizeof( uint32_t ), NULL, 0, 32 );
	SPI_CommandSeq[0] = SPIM_CMD_SEND_CMD( 0x06, 8, uSPI_Single );
	SPI_MasterTransferBlocking(  SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,
								( uint32_t * ) SPI_CommandSeq,
								sizeof( uint32_t ), NULL, 0, 32 );
	SPI_CommandSeq[0] = SPIM_CMD_EOT( 0 );
	SPI_MasterTransferBlocking(  SPIM1 /* = spi_address[iMaster_config.whichCsn]*/,
								( uint32_t * ) SPI_CommandSeq,
								sizeof( uint32_t ), NULL, 0, 32 );
	memset(&cmd_SPI_CommandSeq, 0, sizeof(spi_command_sequence_t));
	cmd_SPI_CommandSeq.cmd       = 0x71;
	cmd_SPI_CommandSeq.cmd_bits  = 8;
	cmd_SPI_CommandSeq.addr_bits = 32;
	cmd_SPI_CommandSeq.addr      = 0x00800004;
	cmd_SPI_CommandSeq.cmd_mode  = uSPI_Single;
	cmd_SPI_CommandSeq.addr_mode = uSPI_Single;
	cmd_SPI_CommandSeq.tx_bits   = 8;
	cmd_SPI_CommandSeq.tx_data   = 0x10 | 0x08;/* 512B/page | uniform sectors of 64kB */
	cmd_SPI_CommandSeq.data_mode = uSPI_Single;
	cmd_SPI_CommandSeq.csn = iMaster_config.whichCsn;
	SPI_MasterTransferCommandSequence(  SPIM1 /* = spi_address[iMaster_config.whichCsn]*/, &cmd_SPI_CommandSeq );

	/* "write"
		cmd_SPI_CommandSeq.cmd_bits  = 8;
		cmd_SPI_CommandSeq.addr_bits = 32;
		cmd_SPI_CommandSeq.addr      = prvAddr;
		cmd_SPI_CommandSeq.tx_bits   = 512*8; // 1 page of 512B
		cmd_SPI_CommandSeq.tx_data   = 0;
		cmd_SPI_CommandSeq.tx_buffer = prvBuffer;
		cmd_SPI_CommandSeq.csn = iMaster_config.whichCsn;
		cmd_SPI_CommandSeq.cmd       = 0x02;
		cmd_SPI_CommandSeq.cmd_mode  = uSPI_Single;
		cmd_SPI_CommandSeq.addr_mode = uSPI_Single;
		cmd_SPI_CommandSeq.data_mode = uSPI_Single;
	*/

	/* "read"
		cmd_SPI_CommandSeq.cmd_bits  = 8;
		cmd_SPI_CommandSeq.addr_bits = 32;
		cmd_SPI_CommandSeq.addr      = addr;
		cmd_SPI_CommandSeq.rx_bits   = size*8;
		cmd_SPI_CommandSeq.dummy     = DUMMY;
		cmd_SPI_CommandSeq.rx_buffer = buf;
		cmd_SPI_CommandSeq.csn = iMaster_config.whichCsn;
		cmd_SPI_CommandSeq.cmd       = 0x0C;
		cmd_SPI_CommandSeq.cmd_mode  = uSPI_Single;
		cmd_SPI_CommandSeq.addr_mode = uSPI_Single;
		cmd_SPI_CommandSeq.data_mode = uSPI_Single;
	*/
}


SPI_MasterDeInit(SPIM1);

/* Altre funzioni interessanti: */
/*!
 * @brief This function does a read polling in SPI master transfer.
 *
 * This function use polling the read value to compare with given value .
 *
 * @param base SPI peripheral address.
 * @param conf Pointer to the structure spi_polling_config_t.
 *
 int SPI_Master_AutoPolling(SPIM_Type *base, spi_polling_config_t *conf);
 */

 /* Metodo per sync? */
 /*
 int16_t read_status;
 do {
		s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
		SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
									( uint32_t * ) s_command_sequence,
									sizeof( uint32_t ), NULL, 0, 32 );
		s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x05, 8, master_config.qpi );
		SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
									( uint32_t * ) s_command_sequence,
									sizeof( uint32_t ), NULL, 0, 32 );
		s_command_sequence[0] = SPIM_CMD_RX_DATA(32, master_config.qpi, 0);
		SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
									( uint32_t * ) s_command_sequence,
									sizeof( uint32_t ), ( uint8_t * ) &read_status,
									4, 32);
		s_command_sequence[0] = SPIM_CMD_EOT( 0 );
		SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
									( uint32_t * ) s_command_sequence,
									sizeof( uint32_t ), NULL, 0, 32 );
		if( master_config.bitsPerFrame == 8 )
			read_status = read_status & 0xff;
		else if( master_config.bitsPerFrame == 16 )
			read_status = read_status & 0xffff;
		else
			read_status = read_status;

	} while( read_status & ( 0x1 << S25FS256_SR_WIP ) );
	*/


/*****************/
/******************************************************/
/* Part 2*/
/******************************************************/
/*****************/
/*
 //SPI_FLASH_Init( B3, A4, B4, B2);
 SPI_FLASH_Init( SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CSN1_B2, 32, 0, 10000000, 0, 0);

 SPI_FLASH_Write( 0x000, BUF_SIZE, txBuffer );

 SPI_FLASH_Deinit();





/*****************/
/******************************************************/
/* END TRY SPI */
/******************************************************/
/*****************/




#ifndef PID_CONFIG_INTEGRAL_ACTION
	#ifndef PID_CONFIG_DERIVATIVE_ACTION
		lEPImathPIDsetParameters(AccumulationTable.kp, 0, 0, INIT_PID_DT, AccumulationTable.TargetPower); //TBD: should we save init_pid_dt as internal variable?
	#else
		lEPImathPIDsetParameters(AccumulationTable.kp, 0, AccumulationTable.kd, INIT_PID_DT, AccumulationTable.TargetPower); //TBD: should we save init_pid_dt as internal variable?
	#endif
#else
	#ifndef PID_CONFIG_DERIVATIVE_ACTION
		lEPImathPIDsetParameters(AccumulationTable.kp, AccumulationTable.ki, 0, INIT_PID_DT, AccumulationTable.TargetPower); //TBD: should we save init_pid_dt as internal variable?
	#else
		lEPImathPIDsetParameters(AccumulationTable.kp, AccumulationTable.ki, AccumulationTable.kd, INIT_PID_DT, AccumulationTable.TargetPower); //TBD: should we save init_pid_dt as internal variable?
	#endif
#endif



int lEPImathPIDcompute(int lSetPoint, int lInput, size_t xCoreNumber) {

	int outputPID = 0;

	/* Compute Error */
	int error = lSetPoint - lInput;
	//TBD: is it better to compute the error outside and pass only 1 value?

	/* Proportional Term */
	outputPID = _kp * error;

	/* Integral Term */
	#ifdef PID_CONFIG_INTEGRAL_ACTION
		_PIDintegralError[xCoreNumber] += error * _ki;
		#ifdef PID_ANTI_WINDUP_SAT
			if (_PIDintegralError > (PID_ANTI_WINDUP_SAT_COEFF * lSetPoint)) //TODO: var conversion //TBC: * lSetPoint
				_PIDintegralError = PID_ANTI_WINDUP_SAT_COEFF * lSetPoint; //TODO: var conversion //TBC: * lSetPoint
		#endif
		outputPID += _PIDintegralError[xCoreNumber];
	#endif

	/* Derivative Term */
	#ifdef PID_CONFIG_DERIVATIVE_ACTION
		outputPID += _kd * (error - _PIDpreviousError[xCoreNumber]);

		/* Update Error Global Variable */
		_PIDpreviousError[xCoreNumber] = error;
	#endif

	/* Saturation */
	#ifdef PID_CONFIG_SATURATION
		if (outputPID > MAX_PID_OUTPUT_VALUE)
			outputPID = MAX_PID_OUTPUT_VALUE;
		if (outputPID < MIN_PID_OUTPUT_VALUE)
			outputPID = MIN_PID_OUTPUT_VALUE;
	#endif

	return outputPID;
}


#ifdef PID_CONFIG_SATURATION
	#define MAX_PID_OUTPUT_VALUE 				700
	#define MIN_PID_OUTPUT_VALUE 				0
#endif

typedef struct _parameter_table {
	//int SetPoint; //TODO: in the Christian paper the setPoint is constant = Tmax - Tmargin.
	//pid_parameters_t TablePIDParameters; //TBD: to create a struc for PID parameters
	int kp;
	#ifdef PID_CONFIG_INTEGRAL_ACTION
		int ki;
	#endif
	#ifdef PID_CONFIG_DERIVATIVE_ACTION
		int kd;
	#endif
	int TargetPower;

	int Workload;
} parameter_table_t;



#ifdef EPI_CONFIG_FREQ_BINDING
//THIS IS WRONG!
for (char i = 0; i < N_EPI_CORE; i++)
	for (char j = 0; j < N_EPI_CORE; j++)
	{
		if (BindingMatrix[i][j] == 1)
		{
			if (CoreTargetPower[i] <= CoreTargetPower[j])
				CoreTargetPower[j] = CoreTargetPower[i];
			else
				CoreTargetPower[j] = CoreTargetPower[i];
		}








		RMSError = MeasuredPowerVR - AccumulatedPower
		//CostFunction += Lambda * ( RMSError * RMSError );

		GradientCost = (Lamda * ( (2*PowerFormulaCoeff[0]*f*Vdd) + (PowerFormulaCoeff[0]*Vdd^2 + PowerFormulaCoeff[3]*w^PowerFormulaCoeff[4]) +
						( (PowerFormulaCoeff[4]-1) * pow(w,(PowerFormulaCoeff[4]-1)) * (PowerFormulaCoeff[2] + PowerFormulaCoeff[3] * f)) ) );

		for (i = 0; i < 5; i++)
		{
			PowerFormulaCoeff[i] += Mu * RMSError * GradientCost;
		}





















	}

#endif





//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//spi

/*
		if ( (rxData[0] == 0x55) && (rxData[1] == 0xAA) )
		{
			//if (GPIO_ReadPinInput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19) ))
			if (testBool)
			{
				GPIO_WritePinOutput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19), uGPIO_HIGH );
				testBool = 0;
			}
			else
			{
				GPIO_WritePinOutput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19), uGPIO_LOW );
				testBool = 1;
			}
		}
		*/


		/*
		DataTest1[0] = rxData[2];
		rxData[0]=0;
		rxData[1]=0;
		rxData[2]=0;
		vSPI_Read(NumberOfBits, rxData);

		DataTest1[1] = rxData[2];
		rxData[0]=0;
		rxData[1]=0;
		rxData[2]=0;
		vSPI_Read(NumberOfBits, rxData);

		DataTest1[2] = rxData[2];

		char appC = txData[2]+txData[2]+txData[2]-(char)6;

		#ifdef GPIO_TEST
		if ( (rxData[0] == 0x55) && (rxData[1] == 0xAA)
					&& (DataTest1[0] == appC )
					&& (DataTest1[1] == (appC) )
					&& (DataTest1[2] == (appC) )
				)
		{
			//if (GPIO_ReadPinInput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19) ))
			//if (testBool)
			//{
				GPIO_WritePinOutput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19), uGPIO_HIGH );
			/*	testBool = 0;
			}
			else
			{
				GPIO_WritePinOutput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19), uGPIO_LOW );
				testBool = 1;
			}*/
		/*}
		else
			GPIO_WritePinOutput( xGpio_addrs[GET_GPIO_PORT(GPIO_A19)], GET_GPIO_NUM( GPIO_A19), uGPIO_LOW );
		*/
