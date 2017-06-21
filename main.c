#include <msp430.h> 
#include "stdio.h"
#include "stdint.h"
#include "nrf24l01.h"
#include "string.h"
#include "uart.h"
#include "info.h"

#define RUN_TX

#ifdef RUN_TX
/* My address */
uint8_t MyAddress[5] = {0xE7};
/* Receiver address */
uint8_t TxAddress1[5] ={0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t TxAddress2[5] ={0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t dataOut[3];
#else
/* Receiver address */
uint8_t TxAddress[] = {0xE7};
/* My address */
uint8_t MyAddress[] = {0x7E};
uint8_t dataIn[3];
#endif

static volatile uint8_t enable;
/* Interrupt status */
NRF24L01_Transmit_Status_t transmissionStatus;

/*
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    /* configure internal digitally controlled oscillator */
    DCOCTL  = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;

    P2IE |=  BIT2;                            // P1.3 interrupt enabled
    P2IES |= BIT2;                            // P1.3 Hi/lo edge
    P2REN |= BIT2;                            // Enable Pull Up on SW2 (P1.3)
    P2IFG &= ~BIT2;                           // P1.3 IFG cleared

    /*Configuare Timer A0 CCR0*/
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    CCR0 = 50000;
    TACTL = TASSEL_2 + MC_1 + ID_3;                  // SMCLK, upmode, DIV 8

    uart_init();
    /* Intiialize LED */
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    P2DIR |= BIT5;
    P2OUT |= BIT5;
    /* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
    /* By default 2Mbps data rate and 0dBm output power */
    /* NRF24L01 goes to RX mode by default */
    NRF24L01_Init(0, 3);

    /* Set 2MBps data rate and -18dBm output power */
    NRF24L01_SetRF(NRF24L01_DataRate_1M, NRF24L01_OutputPower_0dBm);

    /* Set my address, 5 bytes */
  //  NRF24L01_SetMyAddress(MyAddress);

    /* Set TX address, 5 bytes */
    NRF24L01_SetTxAddress(TxAddress2);
    __delay_cycles(500000);
    P1OUT &=~(BIT6 + BIT0);
    __enable_interrupt();

    uart_puts("\r\n start \r\n");
    printDetails();
    uart_puts("end \r\n");

    while(1)
    {
#ifdef RUN_TX
        if(enable > 90)
        {
            enable =0; //reset enable
            dataOut[0] =  'B';
            dataOut[1] =  'B';
            dataOut[2] =  0;
            NRF24L01_Transmit(dataOut);
            /* Set NRF state to sending */
            transmissionStatus = NRF24L01_Transmit_Status_Sending;
        }
        if((transmissionStatus != NRF24L01_Transmit_Status_Sending) && (enable != 0))
        {
            /* Transmission was OK */
            if (transmissionStatus == NRF24L01_Transmit_Status_Ok)
            {
                P2OUT &= ~BIT5;
            }
            /* Message LOST */
            if (transmissionStatus == NRF24L01_Transmit_Status_Lost)
            {
                P1OUT &= ~BIT0;
            }
        }
#else /* RUN_TX */
        /* Something esle */
        __delay_cycles(500000);
        P2OUT &= ~BIT5;
#endif /* !RUN_TX */
        memset(dataOut, 0, 2);
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    enable++;
}

/* Interrupt handler */
#pragma vector=PORT2_VECTOR
__interrupt void Port_1_Handler(void)
{
    if((P2IFG & BIT2))
    {
        /* Read interrupts */
        uint8_t irq = NRF24L01_GET_INTERRUPTS;
#ifdef RUN_TX
        /* Check if transmitted OK */
        if (irq & NRF24L01_IRQ_TRAN_OK)
        {
            /* Save transmission status */
            transmissionStatus = NRF24L01_Transmit_Status_Ok;
            /* Turn on led */
            P2OUT |= BIT5;
            /* Go back to RX mode */
            NRF24L01_PowerUpRx();
        }

        /* Check if max transmission reached and last transmission failed */
        if (irq & NRF24L01_IRQ_MAX_RT)
        {
            /* Save transmission status */
            transmissionStatus = NRF24L01_Transmit_Status_Lost;
            /* Turn on led */
            P1OUT |= BIT0;
            /* Go back to RX mode */
            NRF24L01_PowerUpRx();
        }
#else
        /* If data is ready on NRF24L01+ */
        if (irq & NRF24L01_IRQ_DATA_READY)
        {
            /* Get data from NRF24L01+ */
            NRF24L01_GetData(dataIn);
            if(dataIn[0] == 'a')
                P2OUT |= BIT5;
        }
#endif
        /* Clear interrupts */
        NRF24L01_CLEAR_INTERRUPTS;
        P2IFG &= ~BIT2;                           // P1.3 IFG cleared
    }
}
