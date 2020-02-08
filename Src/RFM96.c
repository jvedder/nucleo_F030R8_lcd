/*
 * RFM96.c
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "RFM96.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* User code -----------------------------------------------------------------*/

void RFM96_Init( void )
{
	// Assert Reset low on the RFM96
	HAL_GPIO_WritePin(RF_RESET_N_GPIO_Port, RF_RESET_N_Pin, GPIO_PIN_RESET);
	Delay_ms(10);
	HAL_GPIO_WritePin(RF_RESET_N_GPIO_Port, RF_RESET_N_Pin, GPIO_PIN_SET);
	Delay_ms(10);

	// Set sleep mode, so we can also set LORA mode:
    RFM96_WriteReg(RFM96_REG_01_OP_MODE, RFM96_MODE_SLEEP | RFM96_LONG_RANGE_MODE);

    // Wait for sleep mode to take over from say, CAD
    Delay_ms(10);

    // Check we are in sleep mode, with LORA set
    if (RFM96_ReadReg(RFM96_REG_01_OP_MODE) != (RFM96_MODE_SLEEP | RFM96_LONG_RANGE_MODE))
    {
    	//	TODO: Throw RFM96 init error
    }

    // Either Rx or Tx can use the entire 256 byte FIFO, but not at same time
    RFM96_WriteReg(RFM96_REG_0E_FIFO_TX_BASE_ADDR, 0x00);
    RFM96_WriteReg(RFM96_REG_0F_FIFO_RX_BASE_ADDR, 0x80);

    // BW = 7: 125 kHz
    // CodingRate = 1:  4/5 code rate
    // ImplicitHeaderModeOn = 1, Implicit Header mode
    RFM96_WriteReg(RFM96_REG_1D_MODEM_CONFIG1, 0x72);

    // SpreadingFactor = 7: 128 chips / symbol,
    // TxContinuousMode = 0 : Normal mode: a single packet is sent
    // RxPayloadCrcOn = 1 : CRC enabled
    // SymbTimeout[9:8] = 0
    RFM96_WriteReg(RFM96_REG_1E_MODEM_CONFIG2, 0x74);

    // LowDataRateOptimize = 1 : Enabled; mandated for when the symbol length exceeds 16ms
    // AgcAutoOn = 0 : LNA gain set by register LnaGain
    RFM96_WriteReg(RFM96_REG_26_MODEM_CONFIG3, 0x04);

    // Preamble Length = 16;
    RFM96_WriteReg(RFM96_REG_20_PREAMBLE_MSB, 0x00);
    RFM96_WriteReg(RFM96_REG_21_PREAMBLE_LSB, 0x10);

    // Set Frequency = 433 MHz
    //   FRF[23:0] = Freq / Fstep
    //   Fstep = Fxosc / 2^^19
    // where:
    //   Freq = 433 MHz
    //   Fxosc = 32 MHz
    uint32_t frf = (uint32_t) ( 433000000.0 / (32000000.0 / 524288.0) ) ;
    RFM96_WriteReg(RFM96_REG_06_FRF_MSB, (frf >> 16) & 0xff);
    RFM96_WriteReg(RFM96_REG_07_FRF_MID, (frf >> 8) & 0xff);
    RFM96_WriteReg(RFM96_REG_08_FRF_LSB, frf & 0xff);

    // PaDac = 4 : Disables the +20dBm option on PA_BOOST pin
    RFM96_WriteReg(RFM96_REG_4D_PA_DAC, 0x04);

    // PaSelect = 1 : PA_BOOST pin (instead of RFO pin).
    // MaxPower = 0 : Pmax=10.8+0.6*MaxPower [dBm]
    // Output Power = 8 : 10dBm from Pout=17-(15-OutputPower) if PaSelect = 1. RadioHead says this is 13 dBm, though
    //RFM96_WriteReg(RFM96_REG_09_PA_CONFIG, 0x88);
    RFM96_WriteReg(RFM96_REG_09_PA_CONFIG, 0xcf);

    return;
}


void RFM96_Send(const uint8_t* data, uint8_t len)
{
    //TODO: Chec (len > RFM96_MAX_MESSAGE_LEN)

    //waitPacketSent(); // Make sure we dont interrupt an outgoing message
    //setModeIdle();

    //if (!waitCAD())
	// return false;  // Check channel activity

    // Position at the beginning of the FIFO
    RFM96_WriteReg(RFM96_REG_0D_FIFO_ADDR_PTR, 0);

    // The payload data
    for(int i=0; i < len; i++)
    {
    	RFM96_WriteReg(RFM96_REG_00_FIFO, data[i]);
    }

    // The message length
    RFM96_WriteReg(RFM96_REG_22_PAYLOAD_LENGTH, len);

    // Start the transmitter
    RFM96_WriteReg(RFM96_REG_01_OP_MODE, RFM96_MODE_TX);

    // Interrupt on DIO0 for TxDone
    RFM96_WriteReg(RFM96_REG_40_DIO_MAPPING1, 0x40);

    return;
}

void RFM96_Receive(uint8_t* data, uint8_t maxlen)
{
	print1("RxCurAddr", RFM96_ReadReg(RFM96_REG_10_FIFO_RX_CURRENT_ADDR));

	RFM96_WriteReg(RFM96_REG_01_OP_MODE, RFM96_MODE_RXCONTINUOUS);

	// Set Interrupt on DIO0 to RxDone
	RFM96_WriteReg(RFM96_REG_40_DIO_MAPPING1, 0x00); // Interrupt on RxDone

	print("WFI...");
	// wait for interrupt
	uint32_t start_time_ms = HAL_GetTick();
	while (! HAL_GPIO_ReadPin(RF_DIO0_GPIO_Port, RF_DIO0_Pin))
	{
		//spin wait

		//turn off the LED after 900 msec (pulse off 100ms in 1 sec Tx cycle)
		if( (HAL_GetTick() - start_time_ms) > 900)
		{
			HAL_GPIO_WritePin(GRN_LED_GPIO_Port, GRN_LED_Pin, GPIO_PIN_RESET);
		}
	}

	// Read the interrupt register
	uint8_t irq_flags = RFM96_ReadReg(RFM96_REG_12_IRQ_FLAGS);
	print1("IRQ Flags:", irq_flags);

	// Number of bytes received
	uint8_t start = RFM96_ReadReg(RFM96_REG_10_FIFO_RX_CURRENT_ADDR);
	uint8_t len = RFM96_ReadReg(RFM96_REG_13_RX_NB_BYTES);
	print1("RxCurAddr: ", start);
	print1("RxNbrBytes:", len);

	// get the read data
	if (len > (maxlen-1)) len = maxlen-1;
	RFM96_WriteReg(RFM96_REG_0D_FIFO_ADDR_PTR, start);
	for (int i = 0; i < len; i++)
	{
		data[i] = RFM96_ReadReg(RFM96_REG_00_FIFO);
	}
	data[len] = '\0';

	// clear all the IRQ flags
	RFM96_WriteReg( RFM96_REG_12_IRQ_FLAGS, 0xFF );

	// Report the SNR and RSSI
	print1("SNR: ", RFM96_ReadReg(RFM96_REG_19_PKT_SNR_VALUE));
	print1("RSSI:", RFM96_ReadReg(RFM96_REG_1A_PKT_RSSI_VALUE));

	// if good CRC
	if ( (irq_flags & RFM96_PAYLOAD_CRC_ERROR) == 0)
	{
		// turn on LED
		HAL_GPIO_WritePin(GRN_LED_GPIO_Port, GRN_LED_Pin, GPIO_PIN_SET);
		// Good beep
		beep(80,0);
	}
	else
	{
		// bad BEEEEP
		beep(160,3);
	}
}

uint8_t RFM96_GetMode( void )
{
	uint8_t mode = RFM96_ReadReg( RFM96_REG_01_OP_MODE );
	return mode & 0x07;
}

void RFM96_ClearInt( void )
{
	// set to clear all the IRQ bits
	RFM96_WriteReg( RFM96_REG_12_IRQ_FLAGS, 0xFF );
}

uint8_t RFM96_ReadReg( uint8_t reg )
{
	HAL_StatusTypeDef status;

	// clear reg msb for read
	reg &= 0x7f;

	// buffers to transmit/receive
	uint8_t txData[] = {reg, 0x00};
	uint8_t rxData[] = {0x00, 0x00};
	const uint16_t	size = sizeof(txData);

	// default data value if error
	uint8_t data = 0x00;

	// Set CS low (active)
	HAL_GPIO_WritePin(RF_CS_N_GPIO_Port, RF_CS_N_Pin, GPIO_PIN_RESET);

	// write 8 bit reg and read 8 bit data
	status = HAL_SPI_TransmitReceive(&hRFSPI, txData, rxData, size, TIMEOUT_1_SEC);

	if (status == HAL_OK)
	{
		//second byte is the register value
		data = rxData[1];
	}
	else
	{
		HAL_UART_Transmit(&huart2, (uint8_t *) &"*HAL_ERROR*\r\n", 13, TIMEOUT_1_SEC);
		// handle errors here
	}

	//print2("RFM96 RD", reg, data );

	// Set CS high (inactive)
	HAL_GPIO_WritePin(RF_CS_N_GPIO_Port, RF_CS_N_Pin, GPIO_PIN_SET);

	return data;
}

void RFM96_WriteReg( uint8_t reg, uint8_t data )
{
	HAL_StatusTypeDef status;

	//print2("RFM96 WR", reg, data );

	//set the reg msb for write
	reg |= 0x80;

	// Transmit buffer
	uint8_t txData[2] = {reg, data};
	const uint16_t size = sizeof(txData);


	// Set CS low (active)
	HAL_GPIO_WritePin(RF_CS_N_GPIO_Port, RF_CS_N_Pin, GPIO_PIN_RESET);

	// write 8 bit reg and read 8 bit data
	status = HAL_SPI_Transmit(&hRFSPI, txData, size, TIMEOUT_1_SEC);

	if (status != HAL_OK)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *) &"*HAL_ERROR*\r\n", 13, TIMEOUT_1_SEC);
		// handle errors here
	}

	//HACK: Wait for SPI transfer to complete
	Delay_ms(1);
	// Set CS high (inactive)
	HAL_GPIO_WritePin(RF_CS_N_GPIO_Port, RF_CS_N_Pin, GPIO_PIN_SET);
}


void Delay_ms( uint32_t delay_ms )
{
	/**
	 * This should correctly handle SysTic roll-overs.
	 * See:
	 *   https://stackoverflow.com/questions/61443/rollover-safe-timer-tick-comparisons
	 */
	uint32_t start_time_ms = HAL_GetTick();
	while ( (HAL_GetTick() - start_time_ms) < delay_ms)
	{
		// spin wait
	}

	return;
}


