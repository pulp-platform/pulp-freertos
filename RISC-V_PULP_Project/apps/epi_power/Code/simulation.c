





#include "simulation.h"

#include "EPI_Config.h"
//#include "gap_common.h"
#include "pmsis.h"

const char SPI_AVOID_CONF[SPI_Bytes] = {0x00, 0x55, 0xAA, 0x00};

const char SPI_FIRST_HANDSHAKE[SPI_Bytes] = {0xAA, 0x5A, 0x55, 0xA5};
const char SPI_READ_HANDSHAKE[SPI_Bytes]  = {0x66, 0x55, 0xAA, 0x44};

const char SPI_WRITE_HANDSHAKE[SPI_Bytes] = {0x45, 0x55, 0xAA, 0x65};

const char SPI_WRITE_UI[SPI_Bytes]		   = {0x98, 0x2A, 0x00, 0x89};
const char SPI_UI_HEADER = 0x3F;



const char SPI_COM_READ_TEMP[SPI_Bytes]   = {0x55, 0x11, 0x5A, 0xA5};
const char SPI_COM_WRITE_FREQ[SPI_Bytes]  = {0x55, 0x22, 0x5A, 0xA5};

const char SPI_COM_READ_VRPOW[SPI_Bytes]  = {0x55, 0x33, 0x5A, 0xA5};
const char SPI_COM_READ_NEW_IN[SPI_Bytes] = {0x55, 0x77, 0x5A, 0xA5};

const char SPI_TRUE_NEW_IN[SPI_Bytes]     = {0x00, 0x1, 0x11, 0xA5}; //00, 1, #changes, something.
                                                // instead of #changes, [2]= something else if also force value on lms

extern const char SPI_RX_HEADER = 0xAF;
extern const char SPI_TX_HEADER = 0x6F;

extern uint8_t gpio_toggle = 0;
extern uint8_t gpio_toggle2 = 0;
void GPIO_A17_toggle(void)
{
    if (gpio_toggle)
    {
        pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 0);
        gpio_toggle = 0;
    }
    else
    {
        pi_gpio_pin_write(NULL, PI_GPIO_A17_PAD_31_B11, 1);
        gpio_toggle = 1;
    }
}


void GPIO_A19_toggle(void)
{
    if (gpio_toggle2)
    {
        pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 0);
        gpio_toggle2 = 0;
    }
    else
    {
        pi_gpio_pin_write(NULL, PI_GPIO_A19_PAD_33_B12, 1);
        gpio_toggle2 = 1;
    }
}
