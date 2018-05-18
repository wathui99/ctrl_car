/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#include <avr/io.h>
#include <stdint.h>
#include "nrf24.h"
#include "my_library.h"
#include "function.h"

/* ------------------------------------------------------------------------- */
/* Software UART routine. */
/* 9600 bps. Transmit only */
/* Transmit pin is: B2 */
/* ------------------------------------------------------------------------- */
/* Hardware specific section ... */
/* ------------------------------------------------------------------------- */
#define F_CPU 16000000UL
#include <util/delay.h>
#include "USART_RS232_H_file.h"
/* ------------------------------------------------------------------------- */
uint8_t temp;
uint8_t q = 0;
uint8_t data_array[4];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
void binary_print (uint8_t x);
/* ------------------------------------------------------------------------- */
int main()
{
    /* init the software uart */
    USART_Init(9600);

    /* init hardware pins */
    nrf24_init();
    
    /* Channel #2 , payload length: 4 */
    nrf24_config(2,4);
 
    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);

    while(1)
    {    
        if(nrf24_dataReady())
        {
            nrf24_getData(data_array);        
            USART_SendString("> ");
            binary_print(data_array[0]);
            binary_print(data_array[1]);
            binary_print(data_array[2]);
            binary_print(data_array[3]);
        }
    }
}
/* ------------------------------------------------------------------------- */
void binary_print (uint8_t x) {
	for (int i=0; i<8 ; i++) {
		if(bit_is_set(x,i)) {
			USART_TxChar('1');
		}
		else USART_TxChar('0');
	}
	USART_TxChar('\n');
}