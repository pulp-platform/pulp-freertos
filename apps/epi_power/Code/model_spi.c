// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/***************************************************/
/*
* Task and function library.
*
*
*/

#include "model_spi.h"

/* FreeRTOS Inclusions. */
#include "pmsis.h"

/* GAP8 Inclusions */
//#include "pmsis_driver/pmsis_drivers.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/spi.h"

/* Libraries Inclusion */
#include "EPI_Config.h"
#ifdef MEASURE_ACTIVE
	#include "measure.h"
#endif

/* Others */
#ifdef FLOAT_NUMBERS
	#include "print_float.h"
#endif
//#include <stdlib.h>

//number of dummy cycles
#define DUMMY 0

struct pi_spi_conf xMasterConfig = {0};
struct pi_device spi1_dev = {0};

//for non-blociking
SemaphoreHandle_t gSemSPI_write;
SemaphoreHandle_t gSemSPI_read;
SemaphoreHandle_t gSemSPI_HR;

//these has to stay here
pi_task_t taskWrite = {0};
pi_task_t taskRead = {0};

void vSPI_Init( void /*spi_master_config_t  *xMasterConfig /*, SPI_Code*/)
{
	//Default Values
	pi_spi_conf_init(&xMasterConfig);

	/* Configuration Parameters */
	xMasterConfig.cs				= 1; //0 //not sure about this!
	xMasterConfig.itf				= 1; //not sure about this! //probably which spi is it.

	xMasterConfig.max_baudrate      = SPI_SPEED_MUL; //random //1000 //5000

	xMasterConfig.wordsize 			= PI_SPI_WORDSIZE_32;
    xMasterConfig.polarity 			= 0; //uSPI_ClockPolarityActiveHigh; //(mode & 0x2) ? uSPI_ClockPolarityActiveLow : uSPI_ClockPolarityActiveHigh;
    xMasterConfig.phase 			= 0; //uSPI_ClockPhaseFirstEdge; //(mode & 0x1) ? uSPI_ClockPhaseSecondEdge : uSPI_ClockPhaseFirstEdge;
	xMasterConfig.big_endian        = 0; //uSPI_MsbFirst;

	pi_open_from_conf(&spi1_dev, &xMasterConfig);

	if (pi_spi_open( &spi1_dev) < 0 )
	{
		printf("error!\n");
		//TODO:
	}
	return;
}

void vSPI_DeInit( void /*spi_master_config_t  *xMasterConfig*/){

	 pi_spi_close( &spi1_dev );

	 return;
}

//-----------------------------------------------------------------------------------
/*** SPI BLOCKING TRANSFER ***/
//-----------------------------------------------------------------------------------

void vSPI_Write(/*spi_master_config_t  *xMasterConfig, /*uint32_t SendAddr, void buf,*/ uint32_t NumberOfBits, void* txData)
{

	//pi_spi_send(&spi1_dev, txData, NumberOfBits, PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE);
	pi_spi_send_async(&spi1_dev, txData, NumberOfBits, PI_SPI_CS_NONE | PI_SPI_LINES_SINGLE, NULL);

	//while (!UDMA_CORE_RX_CFG_PENDING(hal_read32(0x1A102018)) ) { } //0x1A102008
	//while (UDMA_CORE_RX_CFG_PENDING(hal_read32(0x1A102018)) ) { } //0x1A102008
	while ( ( (hal_read32((void*)0x1A102118) & UDMA_CORE_TX_CFG_PENDING_MASK) >> UDMA_CORE_TX_CFG_PENDING_SHIFT ) ) { } //0x1A102008

	return;
}

void vSPI_Read(/*spi_master_config_t  *xMasterConfig, /*uint32_t SendAddr, void buf,*/ uint32_t NumberOfBits, void* rxData)
{

	//pi_spi_receive(&spi1_dev, rxData, NumberOfBits, PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE);
	pi_spi_receive_async(&spi1_dev, rxData, NumberOfBits, PI_SPI_CS_NONE | PI_SPI_LINES_SINGLE, NULL);

	while ( ( (hal_read32((void*)0x1A102108) & UDMA_CORE_TX_CFG_PENDING_MASK) >> UDMA_CORE_TX_CFG_PENDING_SHIFT ) ) { }

	return;
}

void vSPI_Duplex(/*uint32_t NumberOfBits, */ void* txData, void* rxData) {

	pi_spi_transfer(&spi1_dev, txData, rxData, 32, PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE);

	return;

}

//-----------------------------------------------------------------------------------
/*** SPI NON-BLOCKING TRANSFER ***/
//-----------------------------------------------------------------------------------

/* SPI Callback Function */
void callbackRead (void *arg) {

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	// Give the Binary
	if ( xSemaphoreGiveFromISR(gSemSPI_read, &xHigherPriorityTaskWoken) == pdFALSE )
	{
		/* An error occurred. Semaphores are implemented using queues.
		* An error can occur if there is no space on the queue to post a message
		* indicating that the semaphore was not first obtained correctly.
		*/
		// Mutex not Released Correctly
		#ifdef DEBUG_ACTIVE
			printf("SPI Read could not be given!\n");
		#endif
	}

	/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
    should be performed to ensure the interrupt returns directly to the highest
    priority task.  The macro used for this purpose is dependent on the port in
    use and may be called portEND_SWITCHING_ISR(). */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void callbackWrite(void *arg){

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	// Give the Binary
	if ( xSemaphoreGiveFromISR(gSemSPI_write, &xHigherPriorityTaskWoken) == pdFALSE )
	{
		/* An error occurred. Semaphores are implemented using queues.
		* An error can occur if there is no space on the queue to post a message
		* indicating that the semaphore was not first obtained correctly.
		*/
		// Mutex not Released Correctly
		#ifdef DEBUG_ACTIVE
			printf("SPI write could not be given!\n");
		#endif
	}

	/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
    should be performed to ensure the interrupt returns directly to the highest
    priority task.  The macro used for this purpose is dependent on the port in
    use and may be called portEND_SWITCHING_ISR(). */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

inline void SPI_AsyncRead(size_t dataSize, void *rx) {

	pi_task_callback(&taskRead, callbackRead, NULL);
	pi_spi_receive_async(&spi1_dev, rx, dataSize, PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE, &taskRead);

	return;
}

inline void SPI_AsyncWrite(size_t dataSize, void *tx)  {

	pi_task_callback(&taskWrite, callbackWrite, NULL);
	pi_spi_send_async(&spi1_dev, tx, dataSize, PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE, &taskWrite);

}
