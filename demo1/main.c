/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "onewire.h"
#include "delay.h"

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/Grace.h>

/*
 *  ======== main ========
 */

void uart_setup()
{
  P1SEL = BIT1 + BIT2;
  P1SEL2 = BIT1 + BIT2;
  P1DIR &= ~ BIT1;
  P1DIR |= BIT2;
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x41;                            // 8MHz 9600
  UCA0BR1 = 0x03;                              // 8MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;
}

/***************************************************************/
static void print_string(char* output_buffer);

void search(onewire_t *ow, uint8_t *id, int depth, int reset)
{
  int i, b1, b2;

  if (depth == 64)
  {
    // we have all 64 bit in this search branch
	print_string("found: id");
    print_string("\n");
    return;
  }

  if (reset)
  {
    if (onewire_reset(ow) != 0) { print_string("reset failed\n"); return; }
    onewire_write_byte(ow, 0xF0); // search ROM command

    // send currently recognized bits
    for (i = 0; i < depth; i++)
    {
      b1 = onewire_read_bit(ow);
      b2 = onewire_read_bit(ow);
      onewire_write_bit(ow, id[i / 8] & (1 << (i % 8)));
    }
  }

  // check another bit
  b1 = onewire_read_bit(ow);
  b2 = onewire_read_bit(ow);
  if (b1 && b2) return; // no response to search
  if (!b1 && !b2) // two devices with different bits on this position
  {
    // check devices with this bit = 0
    onewire_write_bit(ow, 0);
    id[depth / 8] &= ~(1 << (depth % 8));
    search(ow, id, depth + 1, 0);
    // check devices with this bit = 1
    id[depth / 8] |= 1 << (depth % 8);
    search(ow, id, depth + 1, 1); // different branch, reset must be issued
  } else if (b1) {
    // devices have 1 on this position
    onewire_write_bit(ow, 1);
    id[depth / 8] |= 1 << (depth % 8);
    search(ow, id, depth + 1, 0);
  } else if (b2) {
    // devices have 0 on this position
    onewire_write_bit(ow, 0);
    id[depth / 8] &= ~(1 << (depth % 8));
    search(ow, id, depth + 1, 0);
  }
}

int relay_value = 0;

int main(void)
{
    Grace_init();                   // Activate Grace-generated configuration
    onewire_t ow;
    uint8_t id[8];

    // >>>>> Fill-in user code here <<<<<
    print_string("MSP430 ready to go!\0");
    ow.port_out = &P1OUT;
    ow.port_in = &P1IN;
    ow.port_ren = &P1REN;
    ow.port_dir = &P1DIR;
    ow.pin = BIT7;

    print_string("start\n");
    search(&ow, id, 0, 1);
    print_string("done\n");

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
