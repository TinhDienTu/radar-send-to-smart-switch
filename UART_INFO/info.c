/*
 * info.c
 *
 *  Created on: Jun 16, 2017
 *      Author: Nguyen Nguyen
 */

#include <stdio.h>
#include "msp430.h"
#include "nrf24l01.h"
#include "uart.h"
#include "string.h"

char buff[64];
unsigned char p_buff[5];

void printDetails(void)
{
    sprintf(buff,"Status: %x\r\n", NRF24L01_GetStatus());
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"FIFO_STATUS: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_FIFO_STATUS));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    NRF24L01_ReadRegisterMulti(NRF24L01_REG_RX_ADDR_P0, p_buff, 5);
    sprintf(buff,"RX_ADDR_P0: %x:%x:%x:%x:%x\r\n", p_buff[4], p_buff[3], p_buff[2], p_buff[1], p_buff[0] );
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(120000);

    sprintf(buff,"RX_ADDR_P1: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_ADDR_P1));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_ADDR_P2: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_ADDR_P2));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_ADDR_P3: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_ADDR_P3));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_ADDR_P4: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_ADDR_P4));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_ADDR_P5: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_ADDR_P5));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    NRF24L01_ReadRegisterMulti(NRF24L01_REG_TX_ADDR, p_buff, 5);
    sprintf(buff,"TX_ADDR: %x:%x:%x:%x:%x\r\n", p_buff[4], p_buff[3], p_buff[2], p_buff[1], p_buff[0] );
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(120000);

    sprintf(buff,"RX_PW_P0: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_PW_P0));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_PW_P1: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_PW_P1));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_PW_P2: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_PW_P2));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

    sprintf(buff,"RX_PW_P3: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_PW_P3));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"RX_PW_P4: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_PW_P4));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"RX_PW_P5: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RX_PW_P5));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"EN_AA: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_EN_AA));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"EN_RXADDR: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_EN_RXADDR));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"RF_CH: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RF_CH));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"RF_SETUP: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_RF_SETUP));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"CONFIG: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_CONFIG));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"FEATURE: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_FEATURE));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);
    sprintf(buff,"DYNPD: %x\r\n", NRF24L01_ReadRegister(NRF24L01_REG_DYNPD));
    uart_puts(buff);memset(buff, 0, 64); __delay_cycles(100000);

}
