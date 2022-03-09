#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "util.h"

void io_printf(OutputDevice out, const char *format, ...) {
	va_list args;

	/* Initalize arguments to store all values after format */
	va_start(args, format);

	// Allocate buffer size based on size of data
	char buffer[vsnprintf(NULL, 0, format, args) + 1]; //uint8_t
	vsnprintf(buffer, sizeof buffer, format, args);

	va_end(args);

	// Output to correct device
	switch (out) {
	case OUT_USB:
		HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
		break;
	case OUT_XBee:
		HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
		break;
	case OUT_GPS:
		break;
	case OUT_CAN:
		CAN_Transmit((uint8_t *)buffer, strlen(buffer));
		break;
	}
}

void CAN_Transmit(uint8_t * buffer, size_t len) {
	io_printf(OUT_USB, "Transmitting CAN\r\n");
	uint32_t TxMailbox; // To return mailbox used to send
	CAN_TxHeaderTypeDef pHeader; //declare a specific header for message transmissions

	pHeader.DLC=len; //give message size
	pHeader.IDE=CAN_ID_STD; //set identifier to standard
	pHeader.RTR=CAN_RTR_DATA; //set data type to data, rather than remote transmission request
	pHeader.StdId=0x244; //define a standard identifier, used for message identification by filters (switch this for the other microcontroller)

	if(HAL_CAN_AddTxMessage(&hcan1, &pHeader, buffer, &TxMailbox) != HAL_OK) {
		Error_Handler();
	}

	while(HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox));
	io_printf(OUT_USB, "Successfully transmitted CAN\r\n");
}