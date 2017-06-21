/*
 *                             ZLib license
 *
 * Copyright (C) 2012 Sergey Pepyakin
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Sergey Pepyakin
 */

#include <msp430.h>
#include <stdbool.h>

#include "uart.h"
#include "ringbuf.h"

// #define SMCLK 16000000

/*
 * Ã�â€”Ã�Â°Ã�Â´Ã�ÂµÃ�ÂºÃ�Â»Ã�Â°Ã‘â‚¬Ã�Â¸Ã‘â‚¬Ã�Â¾Ã�Â²Ã�Â°Ã‘â€šÃ‘Å’ Ã�Â¿Ã�Â¸Ã�Â½Ã‘â€¹ Ã�Â´Ã�Â»Ã‘ï¿½ Ã�Â²Ã�Â²Ã�Â¾Ã�Â´Ã�Â°-Ã�Â²Ã‘â€¹Ã�Â²Ã�Â¾Ã�Â´Ã�Â°.
 */
#define RX_PIN		BIT1
#define TX_PIN		BIT2

static ring_buffer rx_buffer;
static ring_buffer tx_buffer;

#ifdef UART_COUNT_RXTX
unsigned short uart_bytes_rx;
unsigned short uart_bytes_tx;
#endif

// Ã�Â¤Ã�Â»Ã�Â°Ã�Â³ Ã�Â¾Ã�Â¶Ã�Â¸Ã�Â´Ã�Â°Ã�Â½Ã�Â¸Ã‘ï¿½ Ã�Â²Ã�Â²Ã�Â¾Ã�Â´Ã�Â°. Ã�Â¡Ã�Â±Ã‘â‚¬Ã�Â¾Ã‘ï¿½ Ã�Â² USCI0RX_ISR.
static volatile bool waiting_for_rx = false;
static volatile bool waiting_for_tx = false;

/**
 * Ã�â€“Ã�Â´Ã�Â°Ã‘â€šÃ‘Å’ Ã�Â²Ã‘â€¹Ã�Â¿Ã�Â¾Ã�Â»Ã�Â½Ã�ÂµÃ�Â½Ã�Â¸Ã‘ï¿½ Ã�Â½Ã�ÂµÃ�ÂºÃ�Â¾Ã‘â€šÃ�Â¾Ã‘â‚¬Ã�Â¾Ã�Â³Ã�Â¾ Ã‘Æ’Ã‘ï¿½Ã�Â»Ã�Â¾Ã�Â²Ã�Â¸Ã‘ï¿½.
 */
static __inline void wait_for_rxtx(volatile bool *condition) {
	// Ã�Â¡Ã�Â¾Ã‘â€¦Ã‘â‚¬Ã�Â½Ã�Â°Ã‘â€šÃ�Â¸Ã‘â€šÃ‘Å’ Ã‘ï¿½Ã�Â¾Ã‘ï¿½Ã‘â€šÃ�Â¾Ã‘ï¿½Ã�Â½Ã�Â¸Ã�Âµ Ã‘â€žÃ�Â»Ã�Â°Ã�Â³Ã�Â° GIE (General Interrupt Enable).
	bool gie_was_enabled = _get_SR_register() & GIE;

	while (*condition) {
		_BIS_SR(LPM0_bits | GIE);
	}

	// Ã�Â£Ã�Â±Ã�Â¸Ã‘â‚¬Ã�Â°Ã�ÂµÃ�Â¼ Ã‘â€žÃ�Â»Ã�Â°Ã�Â³ GIE Ã�Â¸Ã�Â· SR, Ã�ÂµÃ‘ï¿½Ã�Â»Ã�Â¸ Ã�Â¾Ã�Â½ Ã�Â±Ã‘â€¹Ã�Â» Ã‘Æ’Ã‘ï¿½Ã‘â€šÃ�Â°Ã�Â½Ã�Â¾Ã�Â²Ã�Â»Ã�ÂµÃ�Â½.
	if (!gie_was_enabled) {
		_BIC_SR(GIE);
	}
}

void uart_init() {
	// Ã�â€™Ã‘â€¹Ã�Â±Ã‘â‚¬Ã�Â°Ã‘â€šÃ‘Å’ Ã‘ï¿½Ã�Â¿Ã�ÂµÃ‘â€ Ã�Â¸Ã�Â°Ã�Â»Ã‘Å’Ã�Â½Ã‘Æ’Ã‘Å½ Ã‘â€žÃ‘Æ’Ã�Â½Ã�ÂºÃ‘â€ Ã�Â¸Ã‘Å½ Ã�Â´Ã�Â»Ã‘ï¿½ Ã�Â¿Ã�Â¸Ã�Â½Ã�Â¾Ã�Â² RX_PIN Ã�Â¸ TX_PIN

	P1SEL |= RX_PIN | TX_PIN;
	P1SEL2 |= RX_PIN | TX_PIN;

	// As said in 15.3.1 of holy user guide,
	// before initialization of USCI we must reset it.
	UCA0CTL1 = UCSWRST;

	UCA0CTL1 |= UCSSEL_2;

	UCA0CTL0 = 0;
	UCA0ABCTL = 0;

	UCA0BR0 = 64;
	UCA0BR1 = 3;
	UCA0MCTL = UCBRS0;

	// Ã�â€”Ã�Â°Ã�Â¿Ã‘Æ’Ã‘ï¿½Ã‘â€šÃ�Â¸Ã‘â€šÃ‘Å’ Ã‘â€šÃ�Â°Ã�Â¹Ã�Â¼Ã�ÂµÃ‘â‚¬.
	UCA0CTL1 &= ~UCSWRST;

	UC0IE |= UCA0RXIE;
	IE2 |= UCA0RXIE;

#ifdef UART_COUNT_RXTX
	uart_bytes_rx = 0;
	uart_bytes_tx = 0;
#endif
}

void uart_putc(unsigned char ch) {
	// TODO: Ã�â€”Ã�Â°Ã�Â±Ã�Â»Ã�Â¾Ã�ÂºÃ�Â¸Ã‘â‚¬Ã�Â¾Ã�Â²Ã�Â°Ã‘â€šÃ‘Å’, Ã�Â´Ã�Â¾ Ã�Â¾Ã‘ï¿½Ã�Â²Ã�Â¾Ã�Â±Ã�Â¾Ã�Â¶Ã�Â´Ã�ÂµÃ�Â½Ã�Â¸Ã‘ï¿½ Ã�Â±Ã‘Æ’Ã‘â€žÃ�ÂµÃ‘â‚¬Ã�Â°.

	ring_push(&tx_buffer, ch);

	// Ã�â€™Ã�ÂºÃ�Â»Ã‘Å½Ã‘â€¡Ã�Â¸Ã‘â€šÃ‘Å’ Ã�Â¿Ã‘â‚¬Ã�ÂµÃ‘â‚¬Ã‘â€¹Ã�Â²Ã�Â°Ã�Â½Ã�Â¸Ã‘ï¿½ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â´Ã�Â°Ã‘â€¡Ã�Â¸.
	UC0IE |= UCA0TXIE;
}

void uart_puts(const char *str) {
	while (*str) {
		ring_push(&tx_buffer, *str++);
	}

	// Ã�â€™Ã�ÂºÃ�Â»Ã‘Å½Ã‘â€¡Ã�Â¸Ã‘â€šÃ‘Å’ Ã�Â¿Ã‘â‚¬Ã�ÂµÃ‘â‚¬Ã‘â€¹Ã�Â²Ã�Â°Ã�Â½Ã�Â¸Ã‘ï¿½ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â´Ã�Â°Ã‘â€¡Ã�Â¸.
	UC0IE |= UCA0TXIE;
}

void uart_putn(unsigned char *str, unsigned int count) {
	while (count--) {
		ring_push(&tx_buffer, *str++);
	}

	// Ã�â€™Ã�ÂºÃ�Â»Ã‘Å½Ã‘â€¡Ã�Â¸Ã‘â€šÃ‘Å’ Ã�Â¿Ã‘â‚¬Ã�ÂµÃ‘â‚¬Ã‘â€¹Ã�Â²Ã�Â°Ã�Â½Ã�Â¸Ã‘ï¿½ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â´Ã�Â°Ã‘â€¡Ã�Â¸.
	UC0IE |= UCA0TXIE;
}

void uart_flush() {
	if (ring_empty(&tx_buffer)) {
		// Ã�â€˜Ã‘Æ’Ã‘â€žÃ�ÂµÃ‘â‚¬ Ã�Â¸ Ã‘â€šÃ�Â°Ã�Âº Ã�Â¿Ã‘Æ’Ã‘ï¿½Ã‘â€š.
		return;
	}

	waiting_for_tx = true;
	wait_for_rxtx(&waiting_for_tx);
}

unsigned char uart_getc() {
	unsigned char ch;

	if (ring_empty(&rx_buffer)) {
		waiting_for_rx = true;
		wait_for_rxtx(&waiting_for_rx);
	}

	ring_pop(&rx_buffer, &ch);
	return ch;
}

unsigned short uart_getw() {
	unsigned char buf[2];

	buf[0] = uart_getc();
	buf[1] = uart_getc();

	return *(unsigned short *) buf;
}

bool uart_getc_noblock(unsigned char *ch) {
	// Ã�Å¡Ã�Â¾Ã�Â»Ã‘Å’Ã‘â€ Ã�ÂµÃ�Â²Ã�Â¾Ã�Â¹ Ã�Â±Ã‘Æ’Ã‘â€žÃ�ÂµÃ‘â‚¬ Ã�Â·Ã�Â°Ã�Â¹Ã�Â¼Ã�ÂµÃ‘â€šÃ‘ï¿½Ã‘ï¿½ Ã�Â²Ã‘ï¿½Ã�ÂµÃ�Â¹ Ã�Â³Ã‘â‚¬Ã‘ï¿½Ã�Â·Ã�Â½Ã�Â¾Ã�Â¹ Ã‘â‚¬Ã�Â°Ã�Â±Ã�Â¾Ã‘â€šÃ�Â¾Ã�Â¹.
	return ring_pop(&rx_buffer, ch);
}

bool uart_getw_noblock(unsigned short *sh) {
	if (ring_len(&rx_buffer) < 2) {
		return false;
	}

	unsigned char buf[2];
	if (ring_pop(&rx_buffer, &buf[0]) && ring_pop(&rx_buffer, &buf[1])) {
		*sh = *(unsigned short *) buf;

		return true;
	} else {
		return false;
	}
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {

	// Ã�Å¸Ã‘â‚¬Ã�Â¾Ã‘â€¡Ã�Â¸Ã‘â€šÃ�Â°Ã‘â€šÃ‘Å’ Ã‘ï¿½Ã�Â¸Ã�Â¼Ã�Â²Ã�Â¾Ã�Â» Ã�Â¸Ã�Â· Ã�Â±Ã‘Æ’Ã‘â€žÃ�ÂµÃ‘â‚¬Ã�Â°-Ã�Â¿Ã‘â‚¬Ã�Â¸Ã�ÂµÃ�Â¼Ã�Â½Ã�Â¸Ã�ÂºÃ�Â°.
	unsigned char ch = UCA0RXBUF;

	if (waiting_for_rx) {
		// Ã�â€¢Ã‘ï¿½Ã�Â»Ã�Â¸ Ã�Â¼Ã‘â€¹ Ã�Â¶Ã�Â´Ã�ÂµÃ�Â¼ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â´Ã�Â°Ã‘â€¡Ã�Â¸, Ã‘â€šÃ�Â¾ Ã‘ï¿½Ã�Â±Ã‘â‚¬Ã�Â¾Ã‘ï¿½Ã�Â¸Ã‘â€šÃ‘Å’ Ã‘â€žÃ�Â»Ã�Â°Ã�Â³ Ã�Â¾Ã�Â¶Ã�Â¸Ã�Â´Ã�Â°Ã�Â½Ã�Â¸Ã‘ï¿½ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â´Ã�Â°Ã‘â€¡Ã�Â¸ Ã�Â¸
		// Ã�Â²Ã�Â¾Ã�Â·Ã�Â²Ã‘â‚¬Ã�Â°Ã‘â€šÃ�Â¸Ã‘â€šÃ‘Å’ CPU Ã�Â² Ã�Â½Ã�Â¾Ã‘â‚¬Ã�Â¼Ã�Â°Ã�Â»Ã‘Å’Ã�Â½Ã�Â¾Ã�Âµ Ã‘ï¿½Ã�Â¾Ã‘ï¿½Ã‘â€šÃ�Â¾Ã‘ï¿½Ã�Â½Ã�Â¸Ã�Âµ.
		waiting_for_rx = false;
		_BIC_SR_IRQ(LPM0_bits);
	}

	// Ã�Å¸Ã�Â¾Ã�Â»Ã�Â¾Ã�Â¶Ã�Â¸Ã‘â€šÃ‘Å’ Ã�Â²Ã�Â¾ Ã�Â²Ã‘â€¦Ã�Â¾Ã�Â´Ã�Â½Ã�Â¾Ã�Â¹ Ã�Â±Ã‘Æ’Ã‘â€žÃ‘â€žÃ�ÂµÃ‘â‚¬ Ã�Â¿Ã‘â‚¬Ã�Â¸Ã‘Ë†Ã�ÂµÃ�Â´Ã‘Ë†Ã�Â¸Ã�Â¹ Ã‘ï¿½Ã�Â¸Ã�Â¼Ã�Â²Ã�Â¾Ã�Â».
	ring_push(&rx_buffer, ch);

#ifdef UART_COUNT_RXTX
	uart_bytes_rx++;
#endif
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {

	if (ring_empty(&tx_buffer)) {
		// Ã�ËœÃ‘ï¿½Ã‘â€¦Ã�Â¾Ã�Â´Ã‘ï¿½Ã‘â€°Ã�Â¸Ã�Â¹ Ã�Â±Ã‘Æ’Ã‘â€žÃ�ÂµÃ‘â‚¬ Ã�Â¿Ã‘Æ’Ã‘ï¿½Ã‘â€š. Ã�â€™Ã‘â€¹Ã�ÂºÃ�Â»Ã‘Å½Ã‘â€¡Ã�Â°Ã�ÂµÃ�Â¼ Ã�Â¿Ã‘â‚¬Ã�ÂµÃ‘â‚¬Ã‘â€¹Ã�Â²Ã�Â°Ã�Â½Ã�Â¸Ã�Âµ Ã�Â½Ã�Â° Ã�Â²Ã‘â€¹Ã�Â²Ã�Â¾Ã�Â´ USCI.
		UC0IE &= ~UCA0TXIE;

		if (waiting_for_tx) {
			// Ã�â€¢Ã‘ï¿½Ã�Â»Ã�Â¸ Ã�Â¼Ã‘â€¹ Ã�Â¶Ã�Â´Ã�ÂµÃ�Â¼ Ã�Â¾Ã�ÂºÃ�Â¾Ã�Â½Ã‘â€¡Ã�Â°Ã�Â½Ã�Â¸Ã‘ï¿½ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â´Ã�Â°Ã‘â€¡Ã�Â¸ Ã�Â´Ã�Â°Ã�Â½Ã�Â½Ã‘â€¹Ã‘â€¦, Ã‘â€šÃ�Â¾ Ã‘ï¿½Ã�Â±Ã‘â‚¬Ã�Â¾Ã‘ï¿½Ã�Â¸Ã‘â€šÃ‘Å’
			// Ã‘â€žÃ�Â»Ã�Â°Ã�Â³ Ã�Â¸ Ã�Â¿Ã�ÂµÃ‘â‚¬Ã�ÂµÃ�Â¹Ã‘â€šÃ�Â¸ Ã�Â² Ã�Â°Ã�ÂºÃ‘â€šÃ�Â¸Ã�Â²Ã�Â½Ã‘â€¹Ã�Â¹ Ã‘â‚¬Ã�ÂµÃ�Â¶Ã�Â¸Ã�Â¼ CPU.
			waiting_for_tx = false;
			_BIC_SR_IRQ(LPM0_bits);
		}

		return;
	}

	unsigned char ch;

	if (ring_pop(&tx_buffer, &ch)) {
		UCA0TXBUF = ch;

#ifdef UART_COUNT_RXTX
		uart_bytes_tx++;
#endif
	}
}
