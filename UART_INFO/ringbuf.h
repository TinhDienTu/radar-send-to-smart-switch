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

#ifndef RINGBUF_H_
#define RINGBUF_H_

/**
 * ÐŸÐ°Ñ€Ð° Ð¿Ñ€Ð¸Ð¼ÐµÑ‡Ð°Ð½Ð¸Ð¹ Ð¿Ð¾ ÐºÐ¾Ð´Ñƒ:
 * 1. Ð’ Ð½ÐµÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ñ… Ð¼ÐµÑ�Ñ‚Ð°Ñ… Ð±Ñ‹Ð»Ð¸ Ð¸Ñ�Ð¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ð½Ð½Ñ‹ ÐºÐ¾Ð½Ñ�Ñ‚Ñ€ÑƒÐºÑ†Ð¸Ð¸ Ñ‚Ð¸Ð¿Ð°
 *     i = i + 1;
 *     if (i == MAX) { i = 0; }
 *    Ð²Ð¼ÐµÑ�Ñ‚Ð¾ Ð¾Ð¿ÐµÑ€Ð°Ñ†Ð¸Ð¸ % (mod). Ð¢Ð°ÐºÐ¾Ðµ Ñ€ÐµÑˆÐµÐ½Ð¸Ðµ Ð±Ñ‹Ð»Ð¾ Ð¿Ñ€Ð¸Ð½Ñ�Ñ‚Ð¾, Ñ‚.Ðº. Ð¾Ð¿ÐµÑ€Ð°Ñ†Ð¸Ð¸ ÑƒÐ¼Ð½Ð¾Ð¶ÐµÐ½Ð¸Ñ�,
 *    Ð´ÐµÐ»ÐµÐ½Ð¸Ñ� Ð¸ Ð¾Ñ�Ñ‚Ð°Ñ‚ÐºÐ° Ð¾Ñ‚ Ð´ÐµÐ»ÐµÐ½Ð¸Ñ� Ð¾Ñ‡ÐµÐ½ÑŒ Ð´Ð¾Ñ€Ð¾Ð³Ð¸ Ð½Ð° Ð¼Ð¸ÐºÑ€Ð¾ÐºÐ¾Ð½Ñ‚Ñ€Ð¾Ð»Ð»ÐµÑ€Ð°Ñ….
 */

#include <stdbool.h>

#define UART_BUFFER_SIZE 64

typedef struct {
	unsigned char buffer[UART_BUFFER_SIZE];

	/**
	 * Ð˜Ð½Ð´ÐµÐºÑ� Ð´Ð»Ñ� Ð¾Ð¿ÐµÑ€Ð°Ñ†Ð¸Ð¸ Ð·Ð°Ð¿Ð¸Ñ�Ð¸.
	 */
	volatile unsigned char head;

	/**
	 * Ð˜Ð½Ð´ÐµÐºÑ� Ð´Ð»Ñ� Ð¾Ð¿ÐµÑ€Ð°Ñ†Ð¸Ð¸ Ñ‡Ñ‚ÐµÐ½Ð¸Ñ�.
	 */
	volatile unsigned char tail;
} ring_buffer;

__inline unsigned int ring_len(ring_buffer *buffer) {
	short first_len = buffer->head - buffer->tail;

	if (first_len >= 0) {
		/*
		 * Ð£ÐºÐ°Ð·Ð°Ñ‚ÐµÐ»ÑŒ Ð´Ð»Ñ� Ð·Ð°Ð¿Ð¸Ñ�Ð¸ Ð½Ð°Ñ…Ð¾Ð´Ð¸Ñ‚Ñ�Ñ� Ñ�Ð¿ÐµÑ€ÐµÐ´Ð¸ Ð¸Ð»Ð¸ Ð½Ð° Ñ‚Ð¾Ð¼ Ð¶Ðµ Ð¼ÐµÑ�Ñ‚Ðµ
		 * ÐºÐ°Ðº Ð¸ ÑƒÐºÐ°Ð·Ð°Ñ‚ÐµÐ»ÑŒ Ð´Ð»Ñ� Ñ‡Ñ‚ÐµÐ½Ð¸Ñ� (Ð² Ñ�Ñ‚Ð¾Ð¼ Ñ�Ð»ÑƒÑ‡Ð°Ðµ Ð±ÑƒÑ„ÐµÑ€ Ð¿ÑƒÑ�Ñ‚).
		 *
		 * |---T-----H----|
		 *
		 * T - buffer->tail
		 * H - buffer->head
		 */
		return first_len;
	} else {
		/*
		 * Ð£ÐºÐ°Ð·Ð°Ñ‚ÐµÐ»ÑŒ Ð´Ð»Ñ� Ð·Ð°Ð¿Ð¸Ñ�Ð¸ Ð½Ð°Ñ…Ð¾Ð´Ð¸Ñ‚Ñ�Ñ� Ð¿Ð¾Ð·Ð°Ð´Ð¸ ÑƒÐºÐ°Ð·Ð°Ñ‚ÐµÐ»Ñ� Ñ‡Ñ‚ÐµÐ½Ð¸Ñ�. Ð­Ñ‚Ð¾
		 * Ð·Ð½Ð°Ñ‡Ð¸Ñ‚ Ñ‡Ñ‚Ð¾ ÑƒÐºÐ°Ð·Ð°Ñ‚ÐµÐ»ÑŒ Ð³Ð¾Ð»Ð¾Ð²Ñ‹ (Ð·Ð°Ð¿Ð¸Ñ�Ð¸) Ð¿ÐµÑ€ÐµÑˆÑ‘Ð» Ð½Ð° Ð½Ð¾Ð²Ñ‹Ð¹ ÐºÑ€ÑƒÐ³, Ð² Ñ‚Ð¾ Ð²Ñ€ÐµÐ¼Ñ� ÐºÐ°Ðº
		 * ÑƒÐºÐ°Ð·Ð°Ñ‚ÐµÐ»ÑŒ Ñ…Ð²Ð¾Ñ�Ñ‚Ð° (Ñ‡Ñ‚ÐµÐ½Ð¸Ñ�) Ð¾Ñ�Ñ‚Ð°Ð»Ñ�Ñ� Ð½Ð° Ð¿Ñ€ÐµÐ¶Ð½ÐµÐ¼ ÐºÑ€ÑƒÐ³Ñƒ.
		 *
		 * |--H---------T-|
		 *
		 * T - buffer->tail
		 * H - buffer->head*
		 */

		return -first_len - 1;
	}
}

/*
 * Ð’Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚, Ð¿ÑƒÑ�Ñ‚Ð¾Ð¹ Ð»Ð¸ ÐºÐ¾Ð»ÑŒÑ†ÐµÐ²Ð¾Ð¹ Ð±ÑƒÑ„Ñ„ÐµÑ€?
 */
__inline bool ring_empty(ring_buffer *buffer) {
	return buffer->head == buffer->tail;
}

/*
 * ÐšÐ»Ð°Ð´ÐµÑ‚ Ð² Ð±ÑƒÑ„Ñ„ÐµÑ€ Ñ�Ð»ÐµÐ¼ÐµÐ½Ñ‚.
 */
__inline void ring_push(ring_buffer *buffer, unsigned char ch) {

	// Ð¡Ð¼. Ð¿Ñ€Ð¸Ð¼. 1.
	unsigned char i = (unsigned char) (buffer->head + 1);
	if (i == UART_BUFFER_SIZE) {
		i = 0;
	}

	if (i != buffer->tail) {
		buffer->buffer[buffer->head] = ch;
		buffer->head = i;
	}
}

/*
 * Ð’Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ð¿Ð¾Ñ�Ð»ÐµÐ´Ð½Ð¸Ð¹ Ñ�Ð»ÐµÐ¼ÐµÐ½Ñ‚ Ð² Ñ�Ð¿Ð¸Ñ�ÐºÐµ.
 */
__inline bool ring_peek(ring_buffer *buffer, unsigned char *ch) {
	if (ring_empty(buffer)) {
		return false;
	}

	*ch = buffer->buffer[buffer->tail];

	return true;
}

/*
 * Ð˜Ð·Ð²Ð»ÐµÐºÐ°ÐµÑ‚ Ð¾Ð´Ð¸Ð½ Ñ�Ð»ÐµÐ¼ÐµÐ½Ñ‚ Ð¸Ð· Ð±ÑƒÑ„Ñ„ÐµÑ€Ð°.
 */
__inline bool ring_pop(ring_buffer *buffer, unsigned char *ch) {
	if (ring_empty(buffer)) {
		return false;
	}

	*ch = buffer->buffer[buffer->tail];

	// Ð¡Ð¼. Ð¿Ñ€Ð¸Ð¼. 1
	unsigned char new_tail = buffer->tail + 1;
	if (new_tail == UART_BUFFER_SIZE) {
		new_tail = 0;
	}

	buffer->tail = new_tail;

	return true;
}

#endif /* RINGBUF_H_ */
