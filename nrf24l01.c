/*
 * nrf24l01.c
 *
 *  Created on: Jun 7, 2017
 *      Author: nthanhtai
 */

#include "nrf24l01.h"

/* Registers default values */
#define NRF24L01_REG_DEFAULT_VAL_CONFIG         0x08
#define NRF24L01_REG_DEFAULT_VAL_EN_AA          0x3F
#define NRF24L01_REG_DEFAULT_VAL_EN_RXADDR      0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_AW       0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_RETR     0x03
#define NRF24L01_REG_DEFAULT_VAL_RF_CH          0x02
#define NRF24L01_REG_DEFAULT_VAL_RF_SETUP       0x0E
#define NRF24L01_REG_DEFAULT_VAL_STATUS         0x0E
#define NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX     0x00
#define NRF24L01_REG_DEFAULT_VAL_RPD            0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2     0xC3
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3     0xC4
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4     0xC5
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5     0xC6
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4      0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P0       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P1       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P2       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P3       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P4       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P5       0x00
#define NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS    0x11
#define NRF24L01_REG_DEFAULT_VAL_DYNPD          0x00
#define NRF24L01_REG_DEFAULT_VAL_FEATURE        0x00

/* Configuration register*/
#define NRF24L01_MASK_RX_DR     6
#define NRF24L01_MASK_TX_DS     5
#define NRF24L01_MASK_MAX_RT    4
#define NRF24L01_EN_CRC         3
#define NRF24L01_CRCO           2
#define NRF24L01_PWR_UP         1
#define NRF24L01_PRIM_RX        0

/* Enable auto acknowledgment*/
#define NRF24L01_ENAA_P5        5
#define NRF24L01_ENAA_P4        4
#define NRF24L01_ENAA_P3        3
#define NRF24L01_ENAA_P2        2
#define NRF24L01_ENAA_P1        1
#define NRF24L01_ENAA_P0        0

/* Enable rx addresses */
#define NRF24L01_ERX_P5         5
#define NRF24L01_ERX_P4         4
#define NRF24L01_ERX_P3         3
#define NRF24L01_ERX_P2         2
#define NRF24L01_ERX_P1         1
#define NRF24L01_ERX_P0         0

/* Setup of address width */
#define NRF24L01_AW             0 //2 bits

/* Setup of auto re-transmission*/
#define NRF24L01_ARD            4 //4 bits
#define NRF24L01_ARC            0 //4 bits

/* RF setup register*/
#define NRF24L01_PLL_LOCK       4
#define NRF24L01_RF_DR_LOW      5
#define NRF24L01_RF_DR_HIGH     3
#define NRF24L01_RF_DR          3
#define NRF24L01_RF_PWR         1 //2 bits

/* General status register */
#define NRF24L01_RX_DR          6
#define NRF24L01_TX_DS          5
#define NRF24L01_MAX_RT         4
#define NRF24L01_RX_P_NO        1 //3 bits
#define NRF24L01_TX_FULL        0

/* Transmit observe register */
#define NRF24L01_PLOS_CNT       4 //4 bits
#define NRF24L01_ARC_CNT        0 //4 bits

/* FIFO status*/
#define NRF24L01_TX_REUSE       6
#define NRF24L01_FIFO_FULL      5
#define NRF24L01_TX_EMPTY       4
#define NRF24L01_RX_FULL        1
#define NRF24L01_RX_EMPTY       0

//Dynamic length
#define NRF24L01_DPL_P0         0
#define NRF24L01_DPL_P1         1
#define NRF24L01_DPL_P2         2
#define NRF24L01_DPL_P3         3
#define NRF24L01_DPL_P4         4
#define NRF24L01_DPL_P5         5

/* Transmitter power*/
#define NRF24L01_M18DBM         0 //-18 dBm
#define NRF24L01_M12DBM         1 //-12 dBm
#define NRF24L01_M6DBM          2 //-6 dBm
#define NRF24L01_0DBM           3 //0 dBm

/* Data rates */
#define NRF24L01_2MBPS          0
#define NRF24L01_1MBPS          1
#define NRF24L01_250KBPS        2

/* Configuration */
#define NRF24L01_CONFIG         ((1 << NRF24L01_EN_CRC) | (0 << NRF24L01_CRCO))

/* Instruction Mnemonics */
#define NRF24L01_REGISTER_MASK              0x1F

#define NRF24L01_READ_REGISTER_MASK(reg)    (0x00 | (NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_WRITE_REGISTER_MASK(reg)   (0x20 | (NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_R_RX_PAYLOAD_MASK          0x61
#define NRF24L01_W_TX_PAYLOAD_MASK          0xA0
#define NRF24L01_FLUSH_TX_MASK              0xE1
#define NRF24L01_FLUSH_RX_MASK              0xE2
#define NRF24L01_REUSE_TX_PL_MASK           0xE3
#define NRF24L01_ACTIVATE_MASK              0x50
#define NRF24L01_R_RX_PL_WID_MASK           0x60

/* Flush FIFOs */
#define NRF24L01_FLUSH_TX                   do { NRF24L01_CSN_LOW; SPI_bitbang_out(NRF24L01_FLUSH_TX_MASK); NRF24L01_CSN_HIGH; } while (0)
#define NRF24L01_FLUSH_RX                   do { NRF24L01_CSN_LOW; SPI_bitbang_out(NRF24L01_FLUSH_RX_MASK); NRF24L01_CSN_HIGH; } while (0)

#define NRF24L01_TRANSMISSON_OK             0
#define NRF24L01_MESSAGE_LOST               1

#define NRF24L01_CHECK_BIT(reg, bit)       (reg & (1 << bit))

typedef struct {
    uint8_t PayloadSize;                //Payload size
    uint8_t Channel;                    //Channel selected
    NRF24L01_OutputPower_t OutPwr;   //Output power
    NRF24L01_DataRate_t DataRate;    //Data rate
} NRF24L01_t;

/* Private functions */
void SPI_bitbang_out(uint8_t);
uint8_t SPI_bitbang_in();

void NRF24L01_InitPins(void);
void NRF24L01_WriteBit(uint8_t reg, uint8_t bit, uint8_t value);
uint8_t NRF24L01_ReadBit(uint8_t reg, uint8_t bit);
uint8_t NRF24L01_ReadRegister(uint8_t reg);
//void NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count);
void NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count);
void NRF24L01_SoftwareReset(void);
uint8_t NRF24L01_RxFifoEmpty(void);

/* NRF structure */
static NRF24L01_t NRF24L01_Struct;


// Output eight-bit value using selected bit-bang pins
void SPI_bitbang_out(uint8_t value)
{
    uint8_t x;

    for(x=8; x > 0; x--)
    {
        if(value & 0x80)                                     // If bit is high...
            SPI_SIMO_OUT |= SPI_BIT_SIMO; // Set SIMO high...
        else
            SPI_SIMO_OUT &= ~SPI_BIT_SIMO;// Set SIMO low...
        value = value << 1;                                  // Rotate bits

        SPI_CLK_OUT &= ~SPI_BIT_CLK;  // Set clock low
        SPI_CLK_OUT |= SPI_BIT_CLK;   // Set clock high
    }
    SPI_CLK_OUT &= ~SPI_BIT_CLK;  // Set clock low
}

// Input eight-bit value using selected bit-bang pins
uint8_t SPI_bitbang_in()
{
    uint8_t x = 0, shift = 0;
    uint8_t y;

  // Determine how many bit positions SOMI is from least-significant bit
    x = SPI_BIT_SOMI;
    while(x > 1)
    {
        shift++;
        x = x >> 1;
    };

    x = 0;
    for(y=8;y>0;y--)
    {
        SPI_CLK_OUT &= ~SPI_BIT_CLK;// Set clock low
        SPI_CLK_OUT |= SPI_BIT_CLK;// Set clock high

        x = x << 1;                             // Make room for next bit
        x = x | ((SPI_SOMI_IN & SPI_BIT_SOMI) >> shift);
    }                                         // Store next bit
    SPI_CLK_OUT &= ~SPI_BIT_CLK; // Set clock low
    return(x);
}


void NRF24L01_InitPins(void) {
    /* Init pins */
    /* CNS pin */
    NRF24L01_CE_INIT;
    /* CE pin */
    NRF24L01_CSN_INIT;
    /* CSN high = disable SPI */
    NRF24L01_CSN_HIGH;
    /* CE low = disable TX/RX */
    NRF24L01_CE_LOW;
    // Config bitbang pins
    SPI_SIMO_OUT |= SPI_BIT_SIMO;
    SPI_CLK_OUT &= ~SPI_BIT_CLK;
    SPI_SIMO_DIR |= SPI_BIT_SIMO;
    SPI_CLK_DIR |= SPI_BIT_CLK;
}

uint8_t NRF24L01_Init(uint8_t channel, uint8_t payload_size) {
    /* Initialize CE, CSN pins and SPI BITBANG */
    NRF24L01_InitPins();

    /* Max payload is 32bytes */
    if (payload_size > 32) {
        payload_size = 32;
    }

    /* Fill structure */
    NRF24L01_Struct.Channel = !channel; /* Set channel to some different value for NRF24L01_SetChannel() function */
    NRF24L01_Struct.PayloadSize = payload_size;
    NRF24L01_Struct.OutPwr = NRF24L01_OutputPower_0dBm;
    NRF24L01_Struct.DataRate = NRF24L01_DataRate_1M;

    /* Reset nRF24L01+ to power on registers values */
    NRF24L01_SoftwareReset();

    /* Channel select */
    NRF24L01_SetChannel(channel);

    /* Set pipeline to max possible 32 bytes */
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P0, NRF24L01_Struct.PayloadSize); // Auto-ACK pipe
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P1, NRF24L01_Struct.PayloadSize); // Data payload pipe
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P2, NRF24L01_Struct.PayloadSize);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P3, NRF24L01_Struct.PayloadSize);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P4, NRF24L01_Struct.PayloadSize);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P5, NRF24L01_Struct.PayloadSize);

    /* Set RF settings (2mbps, output power) */
    NRF24L01_SetRF(NRF24L01_Struct.DataRate, NRF24L01_Struct.OutPwr);

    /* Config register */
    NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, NRF24L01_CONFIG);

    /* Enable auto-acknowledgment for all pipes */
    NRF24L01_WriteRegister(NRF24L01_REG_EN_AA, 0x3F);

    /* Enable RX addresses */
    NRF24L01_WriteRegister(NRF24L01_REG_EN_RXADDR, 0x3F);

    /* Auto retransmit delay: 1000 (4x250) us and Up to 15 retransmit trials */
    NRF24L01_WriteRegister(NRF24L01_REG_SETUP_RETR, 0x4F);

    /* Dynamic length configurations: No dynamic length */
    NRF24L01_WriteRegister(NRF24L01_REG_DYNPD, (0 << NRF24L01_DPL_P0) | (0 << NRF24L01_DPL_P1) | (0 << NRF24L01_DPL_P2) | (0 << NRF24L01_DPL_P3) | (0 << NRF24L01_DPL_P4) | (0 << NRF24L01_DPL_P5));

    /* Clear FIFOs */
    NRF24L01_FLUSH_TX;
    NRF24L01_FLUSH_RX;

    /* Clear interrupts */
    NRF24L01_CLEAR_INTERRUPTS;

    /* Go to RX mode */
    NRF24L01_PowerUpRx();

    /* Return OK */
    return 1;
}

void NRF24L01_SetMyAddress(uint8_t *adr)
{
    NRF24L01_CE_LOW;
    NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P1, adr, 5);
    NRF24L01_CE_HIGH;
}

void NRF24L01_SetTxAddress(uint8_t *adr)
{
    NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P0, adr, 5);
    NRF24L01_WriteRegisterMulti(NRF24L01_REG_TX_ADDR, adr, 5);
}

void NRF24L01_WriteBit(uint8_t reg, uint8_t bit, uint8_t value) {
    uint8_t tmp;
    /* Read register */
    tmp = NRF24L01_ReadRegister(reg);
    /* Make operation */
    if (value) {
        tmp |= 1 << bit;
    } else {
        tmp &= ~(1 << bit);
    }
    /* Write back */
    NRF24L01_WriteRegister(reg, tmp);
}

uint8_t NRF24L01_ReadBit(uint8_t reg, uint8_t bit) {
    uint8_t tmp;
    tmp = NRF24L01_ReadRegister(reg);
    if (!NRF24L01_CHECK_BIT(tmp, bit)) {
        return 0;
    }
    return 1;
}

uint8_t NRF24L01_ReadRegister(uint8_t reg)
{
    uint8_t value;
    NRF24L01_CSN_LOW;
    SPI_bitbang_out(NRF24L01_READ_REGISTER_MASK(reg));
    value = SPI_bitbang_in();
    NRF24L01_CSN_HIGH;

    return value;
}

void NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count)
{
    uint8_t i;
    NRF24L01_CSN_LOW;
    SPI_bitbang_out(NRF24L01_READ_REGISTER_MASK(reg));
    for (i = 0; i < count; i++)
        data[i] = SPI_bitbang_in();     // Read data
    NRF24L01_CSN_HIGH;
}

void NRF24L01_WriteRegister(uint8_t reg, uint8_t value) {
    NRF24L01_CSN_LOW;
    while (SPI_SOMI_IN & SPI_BIT_SOMI); // Wait MOSI ready
    SPI_bitbang_out(NRF24L01_WRITE_REGISTER_MASK(reg));
    SPI_bitbang_out(value);
    NRF24L01_CSN_HIGH;
}

void NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count)
{
    uint8_t i;
    NRF24L01_CSN_LOW;
    while (SPI_SOMI_IN & SPI_BIT_SOMI); // Wait MOSI ready
    SPI_bitbang_out(NRF24L01_WRITE_REGISTER_MASK(reg));

    for (i = 0; i < count; i++)
        SPI_bitbang_out(data[i]);     // Send data multi

    NRF24L01_CSN_HIGH;
}

void NRF24L01_PowerUpTx(void) {
    NRF24L01_CLEAR_INTERRUPTS;
    NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, NRF24L01_CONFIG | (0 << NRF24L01_PRIM_RX) | (1 << NRF24L01_PWR_UP));
}

void NRF24L01_PowerUpRx(void) {
    /* Disable RX/TX mode */
    NRF24L01_CE_LOW;
    /* Clear RX buffer */
    NRF24L01_FLUSH_RX;
    /* Clear interrupts */
    NRF24L01_CLEAR_INTERRUPTS;
    /* Setup RX mode */
    NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, NRF24L01_CONFIG | 1 << NRF24L01_PWR_UP | 1 << NRF24L01_PRIM_RX);
    /* Start listening */
    NRF24L01_CE_HIGH;
}

void NRF24L01_PowerDown(void) {
    NRF24L01_CE_LOW;
    NRF24L01_WriteBit(NRF24L01_REG_CONFIG, NRF24L01_PWR_UP, 0x01);
}

void NRF24L01_Transmit(uint8_t *data)
{
    uint8_t i;
    uint8_t count = NRF24L01_Struct.PayloadSize;

    /* Chip enable put to low, disable it */
    NRF24L01_CE_LOW;

    /* Go to power up tx mode */
    NRF24L01_PowerUpTx();

    /* Clear TX FIFO from NRF24L01+ */
    NRF24L01_FLUSH_TX;

    /* Send payload to nRF24L01+ */
    NRF24L01_CSN_LOW;
    /* Send write payload command */
    while (SPI_SOMI_IN & SPI_BIT_SOMI); // Wait MOSI ready
    SPI_bitbang_out(NRF24L01_W_TX_PAYLOAD_MASK);

    /* Fill payload with data*/
    for (i = 0; i < count; i++)
      SPI_bitbang_out(data[i]);     // Send data

    /* Disable SPI */
    NRF24L01_CSN_HIGH;

    /* Send data! */
    NRF24L01_CE_HIGH;
}

void NRF24L01_GetData(uint8_t* data)
{
    uint8_t i;
    /* Pull down chip select */
    NRF24L01_CSN_LOW;
    /* Send read payload command*/
    SPI_bitbang_out(NRF24L01_R_RX_PAYLOAD_MASK);
    /* Read payload */
    for (i = 0; i < NRF24L01_Struct.PayloadSize; i++)
    {
        data[i] = SPI_bitbang_in();     // reciver data
    }

//    SPI_SendMulti(NRF24L01_SPI, data, data, NRF24L01_Struct.PayloadSize);
    /* Pull up chip select */
    NRF24L01_CSN_HIGH;

    /* Reset status register, clear RX_DR interrupt flag */
    NRF24L01_WriteRegister(NRF24L01_REG_STATUS, (1 << NRF24L01_RX_DR));
}

uint8_t NRF24L01_DataReady(void) {
    uint8_t status = NRF24L01_GetStatus();

    if (NRF24L01_CHECK_BIT(status, NRF24L01_RX_DR)) {
        return 1;
    }
    return !NRF24L01_RxFifoEmpty();
}

uint8_t NRF24L01_RxFifoEmpty(void) {
    uint8_t reg = NRF24L01_ReadRegister(NRF24L01_REG_FIFO_STATUS);
    return NRF24L01_CHECK_BIT(reg, NRF24L01_RX_EMPTY);
}

uint8_t NRF24L01_GetStatus(void) {
    uint8_t status;

    NRF24L01_CSN_LOW;
    /* First received byte is always status register */
    status = SPI_bitbang_in();
    /* Pull up chip select */
    NRF24L01_CSN_HIGH;

    return status;
}

NRF24L01_Transmit_Status_t NRF24L01_GetTransmissionStatus(void) {
    uint8_t status = NRF24L01_GetStatus();
    if (NRF24L01_CHECK_BIT(status, NRF24L01_TX_DS)) {
        /* Successfully sent */
        return NRF24L01_Transmit_Status_Ok;
    } else if (NRF24L01_CHECK_BIT(status, NRF24L01_MAX_RT)) {
        /* Message lost */
        return NRF24L01_Transmit_Status_Lost;
    }

    /* Still sending */
    return NRF24L01_Transmit_Status_Sending;
}

void NRF24L01_SoftwareReset(void) {
    uint8_t data[5];

    NRF24L01_WriteRegister(NRF24L01_REG_CONFIG,      NRF24L01_REG_DEFAULT_VAL_CONFIG);
    NRF24L01_WriteRegister(NRF24L01_REG_EN_AA,       NRF24L01_REG_DEFAULT_VAL_EN_AA);
    NRF24L01_WriteRegister(NRF24L01_REG_EN_RXADDR,   NRF24L01_REG_DEFAULT_VAL_EN_RXADDR);
    NRF24L01_WriteRegister(NRF24L01_REG_SETUP_AW,    NRF24L01_REG_DEFAULT_VAL_SETUP_AW);
    NRF24L01_WriteRegister(NRF24L01_REG_SETUP_RETR,  NRF24L01_REG_DEFAULT_VAL_SETUP_RETR);
    NRF24L01_WriteRegister(NRF24L01_REG_RF_CH,       NRF24L01_REG_DEFAULT_VAL_RF_CH);
    NRF24L01_WriteRegister(NRF24L01_REG_RF_SETUP,    NRF24L01_REG_DEFAULT_VAL_RF_SETUP);
    NRF24L01_WriteRegister(NRF24L01_REG_STATUS,      NRF24L01_REG_DEFAULT_VAL_STATUS);
    NRF24L01_WriteRegister(NRF24L01_REG_OBSERVE_TX,  NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX);
    NRF24L01_WriteRegister(NRF24L01_REG_RPD,         NRF24L01_REG_DEFAULT_VAL_RPD);

    //P0
    data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4;
    NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P0, data, 5);

    //P1
    data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4;
    NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P1, data, 5);

    NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P2,  NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P3,  NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P4,  NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P5,  NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5);

    //TX
    data[0] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4;
    NRF24L01_WriteRegisterMulti(NRF24L01_REG_TX_ADDR, data, 5);

    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P0,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P0);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P1,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P1);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P2,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P2);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P3,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P3);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P4,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P4);
    NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P5,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P5);
    NRF24L01_WriteRegister(NRF24L01_REG_FIFO_STATUS, NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS);
    NRF24L01_WriteRegister(NRF24L01_REG_DYNPD,       NRF24L01_REG_DEFAULT_VAL_DYNPD);
    NRF24L01_WriteRegister(NRF24L01_REG_FEATURE,     NRF24L01_REG_DEFAULT_VAL_FEATURE);
}

uint8_t NRF24L01_GetRetransmissionsCount(void) {
    /* Low 4 bits */
    return NRF24L01_ReadRegister(NRF24L01_REG_OBSERVE_TX) & 0x0F;
}

void NRF24L01_SetChannel(uint8_t channel) {
    if (channel <= 125 && channel != NRF24L01_Struct.Channel) {
        /* Store new channel setting */
        NRF24L01_Struct.Channel = channel;
        /* Write channel */
        NRF24L01_WriteRegister(NRF24L01_REG_RF_CH, channel);
    }
}

void NRF24L01_SetRF(NRF24L01_DataRate_t DataRate, NRF24L01_OutputPower_t OutPwr) {
    uint8_t tmp = 0;
    NRF24L01_Struct.DataRate = DataRate;
    NRF24L01_Struct.OutPwr = OutPwr;

    if (DataRate == NRF24L01_DataRate_2M) {
        tmp |= 1 << NRF24L01_RF_DR_HIGH;
    } else if (DataRate == NRF24L01_DataRate_250k) {
        tmp |= 1 << NRF24L01_RF_DR_LOW;
    }
    /* If 1Mbps, all bits set to 0 */

    if (OutPwr == NRF24L01_OutputPower_0dBm) {
        tmp |= 3 << NRF24L01_RF_PWR;
    } else if (OutPwr == NRF24L01_OutputPower_M6dBm) {
        tmp |= 2 << NRF24L01_RF_PWR;
    } else if (OutPwr == NRF24L01_OutputPower_M12dBm) {
        tmp |= 1 << NRF24L01_RF_PWR;
    }

    NRF24L01_WriteRegister(NRF24L01_REG_RF_SETUP, tmp);
}

