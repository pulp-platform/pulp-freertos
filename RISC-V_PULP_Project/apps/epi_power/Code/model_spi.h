
/***************************************************/
/*
* Task and function library.
*
*
*/


#ifndef _MODEL_SPI_H_
#define _MODEL_SPI_H_

#include "EPI_Config.h"
#include "pmsis.h"

#define SPI_Bytes   4
#define SPI_bits    32 //(SPI_arrayLength * 8)

extern struct pi_device spi1_dev;

void vSPI_Init( void /*spi_master_config_t  *xMasterConfig /*, SPI_Code*/);
void vSPI_Write(/*spi_master_config_t  *xMasterConfig, /*uint32_t SendAddr, void buf,*/ uint32_t NumberOfBits, void* txData);
void vSPI_Read(/*spi_master_config_t  *xMasterConfig, /*uint32_t SendAddr, void buf,*/ uint32_t NumberOfBits, void* rxData);
void vSPI_DeInit( void /*spi_master_config_t  *xMasterConfig /*, SPI_Code*/);

void vSPI_Duplex(/*uint32_t NumberOfBits, */ void* txData, void* rxData);

extern SemaphoreHandle_t gSemSPI_write;
extern SemaphoreHandle_t gSemSPI_read;
extern SemaphoreHandle_t gSemSPI_HR;

void callbackRead (void *arg);
//void callbackWrite (void *arg);

extern inline void SPI_AsyncRead(size_t dataSize, void *rx);
extern inline void SPI_AsyncWrite(size_t dataSize, void *tx);

#endif
