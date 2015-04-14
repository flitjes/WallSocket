/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/Grace.h>

/*
 *  ======== main ========
 */

int relay_value = 0;
static void print_string(char* output_buffer);
int main(void)
{
    Grace_init();                   // Activate Grace-generated configuration
    
    // >>>>> Fill-in user code here <<<<<
    print_string("MSP430 ready to go!\0");
    return (0);
}

static void print_string(char* output_buffer){
	int count = 0;
	while(output_buffer[count] != '\0'){
		while (!(IFG2&UCA0TXIFG));
			UCA0TXBUF = output_buffer[count];
			count++;
			if(count >= 256) break;
	}
}

char input_buffer[256];
char output_buffer[20];
int input_count = 0;

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR_HOOK(void)
{
    char input = UCA0RXBUF;
    if(input_count >= 256) input_count = 0;
	input_buffer[input_count + 1] = '\0';

	if(input == 0x0D){ //new line
		if(strcmp(input_buffer, "status") == 0){
			sprintf(output_buffer, "Status: %d\n", (P1OUT & BIT0));
			print_string(output_buffer);
		}
		if(strcmp(input_buffer, "toggle") == 0){
			P1OUT ^= 1 << 0;
			print_string("Toggeling GPIO\n");
		}
		char* p = strstr(input_buffer, "set=");
		if(p != NULL){
			p = strstr(input_buffer, "=");
			int value = atoi(p+1);

			if(value)
				P1OUT |= BIT0;
			else
				P1OUT &= ~BIT0;
		}
		print_string(input_buffer);
		input_count = 0;
	} else {
		input_buffer[input_count] = input;
		input_count++;
	}

}
