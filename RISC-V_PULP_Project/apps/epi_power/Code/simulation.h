


#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "model_spi.h"


#define SE_CT_FAIL_PERIOD           0x1
#define SE_CT_FAIL_SPI_RX           0x2
#define SE_CT_FAIL_SPI_TX           0x4

#define SE_TOS_FAIL_SPI_VM          0x8
#define SE_TOS_FAIL_INPUT           0x10

#define SE_NUMBERS_FAIL             0x20
#define SE_GLOBAL_VAR_FAIL          0x40


const char SPI_AVOID_CONF[SPI_Bytes];

const char SPI_FIRST_HANDSHAKE[SPI_Bytes];
const char SPI_READ_HANDSHAKE[SPI_Bytes];
const char SPI_WRITE_HANDSHAKE[SPI_Bytes];

const char SPI_WRITE_UI[SPI_Bytes];
const char SPI_UI_HEADER;

const char SPI_COM_READ_TEMP[SPI_Bytes];
const char SPI_COM_WRITE_FREQ[SPI_Bytes];

const char SPI_COM_READ_VRPOW[SPI_Bytes];
const char SPI_COM_READ_NEW_IN[SPI_Bytes];
const char SPI_TRUE_NEW_IN[SPI_Bytes];

const char SPI_RX_HEADER;
const char SPI_TX_HEADER;


void GPIO_A17_toggle(void);
extern uint8_t gpio_toggle;

#endif
